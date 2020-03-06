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

#ifndef QT_UTIL_H
#define QT_UTIL_H

#include <QMatrix4x4>
#include "data.h"
#include "geometry.h"


QQuaternion to_qquat(rotation_t const & rot);


template <typename T>
void flip(T *pixels, size_t width, size_t height, size_t channels)
{
    for (size_t y = 0; y < height / 2; ++y)
    {
        size_t iy = height - y - 1;
        for (size_t x = 0; x < width; ++x)
        {
            for (size_t i = 0; i < channels; ++i)
            {
                std::swap(pixels[(y * width + x) * channels + i],pixels[(iy * width + x) * channels +i]);
            }
        }
    }
}

int take_lazy_screenshot(std::string const & filename, size_t width, size_t height, std::string const & camera, viewtype_t type, bool export_nan, scene_t & scene);

bool contains_nan(QMatrix4x4 const & mat);

#endif
