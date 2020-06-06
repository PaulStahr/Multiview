/*
Copyright (c) 2018 Paul Stahr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "qt_util.h"

#include <algorithm>
#include <cmath>
#include "util.h"
#include "image_io.h"

#include <QMatrix4x4>
#include <QtGui/QPixmap>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

QQuaternion to_qquat(rotation_t const & rot)
{
    return QQuaternion(rot[0], rot[1], rot[2], rot[3]);
}

bool contains_nan(QMatrix4x4 const & mat)
{
    return std::any_of(mat.constData(), mat.constData() + 16, UTIL::isnan<float>);
}

int take_save_lazy_screenshot(std::string const & filename, size_t width, size_t height, std::string const & camera, viewtype_t type, bool export_nan, size_t prerendering, scene_t & scene)
{
    screenshot_handle_t handle;
    queue_lazy_screenshot_handle(filename, width, height, camera, type, export_nan, prerendering, scene, handle);
    handle.wait_until(screenshot_state_copied);
    return handle._data != 0 ? save_lazy_screenshot(filename, handle) : 1;
}

void queue_lazy_screenshot_handle(std::string const & filename, size_t width, size_t height, std::string const & camera, viewtype_t type, bool export_nan, size_t prerendering, scene_t & scene, screenshot_handle_t & handle)
{
    handle._camera= camera;
    handle._type = type;
    handle._width = width;
    handle._height = height;
    handle._prerendering = prerendering;
    handle._channels = ends_with(filename, ".exr") ? 0 : type == VIEWTYPE_INDEX ? 1 : 3;
    handle._datatype = ends_with(filename, ".exr") ? GL_FLOAT : GL_UNSIGNED_BYTE;
    handle._ignore_nan = export_nan;
    handle._data = nullptr;
    handle._state = screenshot_state_inited;
    handle._flip = true;
    scene.queue_handle(handle);
}

int save_lazy_screenshot(std::string const & filename, screenshot_handle_t & handle)
{
    if (!handle._data)
    {
        std::cout << "error no data to write" << std::endl;
        return 1;
    }
    std::cout << "writing " << filename << std::endl;
    
    fs::create_directories(fs::path(filename).parent_path());
    if (ends_with(filename, ".exr"))
    {
#ifdef OPENEXR
        if (handle._datatype == GL_FLOAT)
        {
            float *pixels = reinterpret_cast<float*>(handle.get_data());
            if (handle._flip)
            {
                flip(pixels, handle._width * handle._channels, handle._height);
            }
            if (handle._channels == 1)
            {
                while(true){
                    try{
                        writeGZ1 (filename, pixels, handle._width, handle._height);
                        break;
                    }catch(std::system_error const & error){
                        if (error.what() == std::string("Resource temporarily unavailable"))
                        {
                            std::cout << "Resource temporarily unavailable" << std::endl;
                            continue;
                        }
                    }
                }
                delete[] pixels;
            }
            else
            {
                float *output = new float[handle._width * handle._height * handle._channels];
                UTIL::transpose(pixels, output, handle._width * handle._height, handle._channels);
                delete[] pixels;
                switch (handle._channels){
                    case 2:
                    {
                        float *red = output, *green = output + handle._width * handle._height;
                        while(true){
                            try{
                                writeGZ1 (filename,red,green,handle._width, handle._height);
                                break;
                            }catch(std::system_error const & error){
                                if (error.what() == std::string("Resource temporarily unavailable"))
                                {
                                    std::cout << "Resource temporarily unavailable" << std::endl;
                                    continue;
                                }
                            }
                            break;
                        }
                        break;
                    }
                    case 3:
                    {
                        float *red = output, *green = output + handle._width * handle._height, *blue = output + handle._width * handle._height * 2;
                        //Libs: -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf
                        while(true){
                            try{
                                writeGZ1 (filename, red, green, blue, handle._width, handle._height);
                                break;
                            }catch(std::system_error const & error){
                                if (error.what() == std::string("Resource temporarily unavailable"))
                                {
                                    std::cout << "Resource temporarily unavailable" << std::endl;
                                    continue;
                                }
                            }
                            break;
                        }
                        break;
                    }
                    default: std::cout << "Error, invalid number of channels: " << handle._channels << std::endl;break;
                }
                delete[] output;
            }
            std::cout << "written " << filename << std::endl;
            handle._data = nullptr;
        }
        else
        {
            std::cout << "Error, invalid datatype" << std::endl;
        }
#else
        std::cout << "Error Openexr not compiled" << std::endl;
#endif
    }
    else
    {
        uint8_t *pixels = reinterpret_cast<uint8_t*>(handle.get_data());
        if (handle._flip)
        {
            flip(pixels, handle._width * handle._channels, handle._height);
        }
        //brg_to_rgb(pixels, width, height);
        QPixmap pixmap(handle._width,handle._height);

        QByteArray pixData;
        //Write image as portable Anymap, see https://de.wikipedia.org/wiki/Portable_Anymap
        size_t type;
        switch(handle._channels)
        {
            case 1: type = 5;break;
            case 3: type = 6;break;
            default:type = 7;break;
        }
        size_t maxvalue;
        switch (handle._datatype)
        {
            case GL_UNSIGNED_BYTE:  maxvalue = 0xFF;    break;
            case GL_UNSIGNED_SHORT: maxvalue = 0xFFFF;  break;
            default:                throw std::runtime_error("Unsopported pixel-depth");
        }
        pixData.append(QString("P%1 %2 %3 %4 ").arg(type).arg(handle._width).arg(handle._height).arg(maxvalue));
        pixData.append(reinterpret_cast<char*>(pixels), handle._width * handle._height* handle._channels);
        std::cout << QString("P%1 %2 %3 %4 ").arg(type).arg(handle._width).arg(handle._height).arg(maxvalue).toStdString() << std::endl;
        if (pixmap.loadFromData(reinterpret_cast<uchar *>(pixData.data()), pixData.size()))
        {
            while(true){
                try{
                    pixmap.save(filename.c_str());
                    break;
                }catch(std::system_error const & error){
                    if (error.what() == std::string("Resource temporarily unavailable"))
                    {
                        std::cout << "Resource temporarily unavailable" << std::endl;
                        continue;
                    }
                }
            }
            std::cout << "written " << filename << std::endl;
        }
        else
        {
            std::cout << "error" << std::endl;
        }
        delete[] pixels;
        handle._data = nullptr;
    }

    /*char tempstring[50];
    sprintf(tempstring,filename.c_str(),1);
    FILE *image;
    if((image=fopen(tempstring, "wb"))==NULL)
    {
        std::cout << "can't open file " << tempstring << std::endl;
        return 1;
    }
    uint8_t TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
    width = handle._width;
    height = handle._height;
    uint8_t header[6]={((uint8_t)(width%256)), ((uint8_t)(width/256)), ((uint8_t)(height%256)), ((uint8_t)(height/256)),24,0};  // TGA header schreiben
    fwrite(TGAheader, sizeof(unsigned char), 12, image);  // Header schreiben
    fwrite(header, sizeof(unsigned char), 6, image);
    fwrite(pixels, sizeof(unsigned char), width * height *3, image);
    fclose(image);
    delete [] pixels;*/
    return 0;
}

std::ostream & operator<<(std::ostream & out, QMatrix4x4 const & mat)
{
    float const *data = mat.data();
    out << '(';
    for (size_t i = 0; i < 4; ++i)
    {
        print_elements(out, data + i * 4, data + i * 4 + 4, ' ') << ',';
    }
    return out << ')';
}


