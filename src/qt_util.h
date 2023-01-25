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

#include "data.h"
#include "geometry.h"

class QMatrix4x4;

template <size_t N>
void transform_matrices(
    object_transform_base_t *tr,
    bool invert,
    std::array<QMatrix4x4, N> & matrices,
    std::array<frameindex_t, N> rotframe,
    std::array<frameindex_t, N> posframe,
    frameindex_t framedenominator,
    frameindex_t r_smooth,
    frameindex_t t_smooth);

QQuaternion to_qquat(rotation_t const & rot);

QMatrix4x3 get_affine(QMatrix4x4 const & mat);

namespace QT_UTIL
{
void translate(QMatrix4x4 & mat, matharray<float, 3> const & pos);
void scale(QMatrix4x4 & mat, matharray<float, 3> const & s);
}

template <typename InputIter, typename OutputIter>
void flip_transpose(InputIter input, OutputIter output, size_t width, size_t height, size_t channels)
{
    InputIter inputrow = input + width * height * channels;
    do
    {
        inputrow -= width * channels;
        for (InputIter inputpx = inputrow; inputpx != inputrow + channels * width; inputpx += channels)
        {
            for (size_t c =0; c < channels; ++c)
            {
                output[width * height * c] = inputpx[c];
            }
            ++output;
        }
    }
    while(inputrow != input);
}

bool contains_nan(QMatrix4x4 const & mat);

std::ostream & operator<<(std::ostream & out, QMatrix4x4 const & mat);
#endif
