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
#include <png.h>
#include <iostream>
#include "util.h"
#include "image_io.h"
#include "image_util.h"
#include "io_util.h"

#include <QMatrix4x4>
#include <QtGui/QPixmap>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

template <size_t N>
void transform_matrices(
    object_transform_base_t *tr,
    bool invert,
    std::array<QMatrix4x4, N> & matrices,
    std::array<frameindex_t, N> rotframe,
    std::array<frameindex_t, N> posframe,
    frameindex_t framedenominator,
    frameindex_t r_smooth,
    frameindex_t t_smooth)
{
    constant_transform_t<QMatrix4x4> *constant_transform    = dynamic_cast<constant_transform_t<QMatrix4x4>* >(tr);
    dynamic_trajectory_t<rotation_t> *rotation_transform    = dynamic_cast<dynamic_trajectory_t<rotation_t>* >(tr);
    dynamic_trajectory_t<vec3f_t>    *translation_transform = dynamic_cast<dynamic_trajectory_t<vec3f_t>* >   (tr);
    if (constant_transform)
    {
        if (invert){
            QMatrix4x4 inverted = constant_transform->_transform.inverted();
            for (QMatrix4x4 & m : matrices){m *= inverted;}
        }
        else
        {
            QMatrix4x4 & transform = constant_transform->_transform;
            for (QMatrix4x4 & m : matrices){m *= transform;}
        }
    }
    else if (rotation_transform)
    {
        auto & key_transforms = rotation_transform->_key_transforms;
        if (invert)
        {
            for (size_t i = 0; i < N; ++i)
            {
                matrices[i].rotate(to_qquat(smoothed(key_transforms, framedenominator, rotframe[i]  - r_smooth, rotframe[i] + r_smooth)).inverted());
            }
        }
        else
        {
            for (size_t i = 0; i < N; ++i)
            {
                matrices[i].rotate (to_qquat(smoothed(key_transforms, framedenominator, rotframe[i]  - r_smooth, rotframe[i]  + r_smooth)));
            }
        }
    }
    else if (translation_transform)
    {
        auto & key_transforms = translation_transform->_key_transforms;
        if (invert)
        {
            for (size_t i = 0; i < N; ++i)
            {
                QT_UTIL::translate(matrices[i], -smoothed(key_transforms, framedenominator, posframe[i]  - t_smooth, posframe[i]  + t_smooth));
            }
        }
        else
        {
            for (size_t i = 0; i < N; ++i)
            {
                QT_UTIL::translate(matrices[i], smoothed(key_transforms, framedenominator, posframe[i]  - t_smooth, posframe[i]  + t_smooth));
            }
        }
    }
}

template void transform_matrices<1> (
    object_transform_base_t *tr,
    bool invert,
    std::array<QMatrix4x4, 1> & matrices,
    std::array<frameindex_t, 1> rotframe,
    std::array<frameindex_t, 1> posframe,
    frameindex_t framedenominator,
    frameindex_t r_smooth,
    frameindex_t t_smooth);

template void transform_matrices<3> (
    object_transform_base_t *tr,
    bool invert,
    std::array<QMatrix4x4, 3> & matrices,
    std::array<frameindex_t, 3> rotframe,
    std::array<frameindex_t, 3> posframe,
    frameindex_t framedenominator,
    frameindex_t r_smooth,
    frameindex_t t_smooth);

QQuaternion to_qquat(rotation_t const & rot)
{
    return QQuaternion(rot.w(), rot.x(), rot.y(), rot.z());
}

bool contains_nan(QMatrix4x4 const & mat)
{
    return std::any_of(mat.constData(), mat.constData() + 16, UTIL::isnan<float>);
}

namespace QT_UTIL
{
void translate(QMatrix4x4 & mat, matharray<float, 3> const & pos){mat.translate(pos[0], pos[1], pos[2]);}
void scale(QMatrix4x4 & mat, matharray<float, 3> const & s){mat.scale(s[0], s[1], s[2]);}
}

QMatrix4x3 get_affine(QMatrix4x4 const & mat)
{
    std::array<float, 16> values;
    mat.copyDataTo(values.data());
    return QMatrix4x3(values.data());
}

namespace IMG_IO{

bool write_png(const char* filename, size_t width, size_t height, size_t channels, uint8_t *data)
{
    FILE *fp = fopen(filename, "wb");
    if(!fp) return false;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return false;

    png_infop info = png_create_info_struct(png);
    if (!info) return false;

    if (setjmp(png_jmpbuf(png))) return false;

    png_init_io(png, fp);

    size_t color_type;
    switch (channels)
    {
        case 1: color_type = PNG_COLOR_TYPE_GRAY;break;
        case 3: color_type = PNG_COLOR_TYPE_RGB;break;
        case 4: color_type = PNG_COLOR_TYPE_RGBA;break;
        default: throw std::runtime_error("Channel number not supported " + std::to_string(channels));
    }
    png_set_IHDR(
        png,
        info,
        width, height,
        8,
        color_type,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);
    png_bytep * row_pointers = new png_bytep[height];
    for(size_t y = 0; y < height; ++y) {
        row_pointers[y] = const_cast<uint8_t*>(data + y * width * channels);
    }

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    delete []row_pointers;

    fclose(fp);

    png_destroy_write_struct(&png, &info);
    return true;
}
}
int save_lazy_screenshot(std::string const & filename, screenshot_handle_t & handle)
{
    if (!handle.has_data())
    {
        std::cout << "error no data to write" << std::endl;
        return 1;
    }
    std::cout << handle._id << " writing " << filename << ' ' << '(' << handle._width << '*' << handle._height << '*' << handle._channels << ')'<< std::endl;
    
    fs::create_directories(fs::path(filename).parent_path());
    if (ends_with(filename, ".exr"))
    {
#ifdef OPENEXR
        if (handle.get_datatype() == GL_FLOAT)
        {
            float *pixels = handle.get_data<float>();
            if (handle._flip){flip(pixels, handle._width * handle._channels, handle._height);}
            if (handle._channels == 1)
            {
                while(true){
                    try{
                        writeGZ1 (filename, pixels, handle._width, handle._height);
                    }catch(std::system_error const & error){
                        if (error.what() == std::string("Resource temporarily unavailable"))
                        {
                            std::cout << "Resource temporarily unavailable" << std::endl;
                            continue;
                        }
                    }
                    break;
                }
                handle.delete_data();
            }
            else
            {
                std::unique_ptr<float[]> output(new float[handle._width * handle._height * handle._channels]);
                UTIL::transpose(pixels, output.get(), handle._width * handle._height, handle._channels);
                handle.delete_data();
                switch (handle._channels){
                    case 2:
                    {
                        float *red = output.get(), *green = output.get() + handle._width * handle._height;
                        while(true){
                            try{
                                writeGZ1 (filename,red,green,handle._width, handle._height);
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
                        float *red = output.get(), *green = output.get() + handle._width * handle._height, *blue = output.get() + handle._width * handle._height * 2;
                        while(true){
                            try{
                                writeGZ1 (filename, red, green, blue, handle._width, handle._height);
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
            }
            std::cout << handle._id << " written " << filename << std::endl;
        }
        else
        {
            std::cout << handle._id << "Error, invalid datatype " << handle.get_datatype()<< std::endl;
        }
#else
        std::cout << "Error Openexr not compiled" << std::endl;
#endif
    }
    else if (ends_with(filename, ".png") && handle.get_datatype() == gl_type<uint8_t>)
    {
        uint8_t *pixels = handle.get_data<uint8_t>();
        if (handle._flip){flip(pixels, handle._width * handle._channels, handle._height);}
        while(true){
            try{
                IMG_IO::write_png(filename.c_str(), handle._width, handle._height, handle._channels, pixels);
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
        handle.delete_data();
    }
    else
    {
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
        char* ptr;
        size_t bytes_per_value;
        switch (handle.get_datatype())
        {
            case gl_type<uint8_t>:
            {
                maxvalue = 0xFF;
                uint8_t *pixels = handle.get_data<uint8_t>();
                if (handle._flip){flip(pixels, handle._width * handle._channels, handle._height);}
                ptr = reinterpret_cast<char*>(pixels);
                bytes_per_value = 1;
                break;
            }
            case gl_type<uint16_t>:
            {
                maxvalue = 0xFFFF;
                uint16_t *pixels = handle.get_data<uint16_t>();
                if (handle._flip){flip(pixels, handle._width * handle._channels, handle._height);}
                ptr = reinterpret_cast<char*>(pixels);
                bytes_per_value = 2;
                break;
            }
            default:                throw std::runtime_error("Unsopported pixel-depth");
        }
        pixData.append(QString("P%1 %2 %3 %4 ").arg(type).arg(handle._width).arg(handle._height).arg(maxvalue));
        pixData.append(ptr, handle._width * handle._height* handle._channels * bytes_per_value);
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
        handle.delete_data();
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


