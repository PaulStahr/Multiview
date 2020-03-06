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

#include <QtGui/QPixmap>

QQuaternion to_qquat(rotation_t const & rot)
{
    return QQuaternion(rot[0], rot[1], rot[2], rot[3]);
}

bool contains_nan(QMatrix4x4 const & mat)
{
    return std::any_of(mat.constData(), mat.constData() + 16, UTIL::isnan<float>);
}

int take_lazy_screenshot(std::string const & filename, size_t width, size_t height, std::string const & camera, viewtype_t type, bool export_nan, scene_t & scene)
{
    screenshot_handle_t handle;
    handle._camera= camera;
    handle._type = type;
    handle._width = width;
    handle._height = height;
    handle._channels = ends_with(filename, ".exr") ? 0 : 3;
    handle._datatype = (type == VIEWTYPE_POSITION || type == VIEWTYPE_DEPTH || type == VIEWTYPE_FLOW) && ends_with(filename, ".exr") ?  GL_FLOAT : GL_UNSIGNED_BYTE;
    handle._ignore_nan = export_nan;
    handle._data = nullptr;
    handle._error_code = 0;
    scene._mtx.lock();
    std::cout << "add handle " << filename << std::endl;
    scene._screenshot_handles.push_back(&handle);
    scene._mtx.unlock();
    std::unique_lock<std::mutex> lck(handle._mtx);
    handle._cv.wait(lck,std::ref(handle));
    if (handle._error_code != 0)
    {
        std::cout << "no screenshot was taken " << handle._error_code << std::endl;
        return 1;
    }
    std::cout << "writing " << filename << std::endl;

    if (ends_with(filename, ".exr"))
    {
        if (handle._datatype == GL_FLOAT)
        {
            float *pixels = reinterpret_cast<float*>(handle._data);
            flip(pixels, width, height, handle._channels);
            switch (handle._channels){
                case 1:writeGZ1 (filename, pixels, handle._width, handle._height) ;break;
                case 2:
                {
                    float *red = new float[handle._width * handle._height];
                    float *green = new float[handle._width * handle._height];
                    for (size_t i = 0; i < handle._width * handle._height; ++i)
                    {
                        red[i]   = pixels[i * 2];
                        green[i] = pixels[i * 2 + 1];
                    }

                    writeGZ1 (filename,red,green,handle._width, handle._height) ;
                    delete[] red;
                    delete[] green;
                    break;
                }
                case 3:
                {
                    
                    float *red = new float[handle._width * handle._height];
                    float *green = new float[handle._width * handle._height];
                    float *blue = new float[handle._width * handle._height];
                    for (size_t i = 0; i < handle._width * handle._height; ++i)
                    {
                        blue[i]  = pixels[i * 3];
                        red[i]   = pixels[i * 3 + 1];
                        green[i] = pixels[i * 3 + 2];
                    }

                    //Libs: -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf
                    writeGZ1 (filename, red, green, blue, handle._width, handle._height) ;
                    delete[] red;
                    delete[] green;
                    delete[] blue;
                    break;
                }
                default: std::cout << "Error, invalid number of channels: " << handle._channels << std::endl;break;
            }
            std::cout << "written " << filename << std::endl;

            delete[] pixels;
        }
        else
        {
            std::cout << "Error, invalid datatype" << std::endl;
        }
    }
    else
    {
        uint8_t *pixels = reinterpret_cast<uint8_t*>(handle._data);
        flip(pixels, width, height, 3);
        //brg_to_rgb(pixels, width, height);
        QPixmap pixmap(handle._width,handle._height);

        QByteArray pixData;
        pixData.append(QString("P6 %1 %2 255 ").arg(handle._width).arg(handle._height));
        pixData.append(reinterpret_cast<char*>(pixels), handle._width * handle._height*3);
        if (pixmap.loadFromData(reinterpret_cast<uchar *>(pixData.data()), pixData.size()))

            //if(pixmap.loadFromData(pixels,handle._width * handle._height*3))
        {
            pixmap.save(filename.c_str());
            std::cout << "written " << filename << std::endl;
        }
        else
        {
            std::cout << "error" << std::endl;
        }
        delete[] pixels;
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
