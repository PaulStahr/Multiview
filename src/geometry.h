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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <array>
#include "io_util.h"

template <typename T, size_t N>
struct matharray : std::array<T,N>{
    T dot() const{
        auto iter = this->cbegin();
        T res = *iter * *iter;
        while(++iter != this->cend())
        {
            res += *iter * *iter;
        }
        return res;
    }
    
    T norm() const {return sqrt(dot());}
};

template <typename T, size_t N>
bool operator==(const matharray <T, N>& lhs, const matharray<T,N> & rhs)
{
    for (size_t i = 0; i < N; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <typename T, size_t N>
bool operator!=(const matharray<T, N>& lhs, const matharray<T,N> & rhs)
{
    return !(lhs == rhs);
}

struct vertex_t : matharray<float, 3>{};

template <typename T, size_t N>
T dot(matharray<T,N> const & lhs, matharray<T,N> const & rhs)
{
    auto liter = lhs.cbegin();
    auto riter = rhs.cbegin();
    T res = *liter * *riter;
    while(++liter != lhs.cend())
    {
        res += *liter * *++riter;
    }
    return res;
}

struct triangle_t : std::array<uint32_t, 3>{};

struct vec2f_t : matharray<float, 2>{
    const float & x() const;
    const float & y() const;
    
    float & x();
    float & y();
    
    vec2f_t();
    vec2f_t(float const x_, float const y_);
};

vec2f_t operator+(const vec2f_t& lhs, const vec2f_t& rhs);
vec2f_t operator-(const vec2f_t& lhs, const vec2f_t& rhs);
vec2f_t operator*(const vec2f_t& lhs, const vec2f_t& rhs);

struct vec3f_t : matharray<float, 3>
{
    const float & x() const;
    const float & y() const;
    const float & z() const;
    
    float & x();
    float & y();
    float & z();
    
    
    vec3f_t(float x_, float y_, float z_);
    
    vec3f_t();
};

vec3f_t operator+(const vec3f_t& lhs, const vec3f_t& rhs);
vec3f_t operator-(const vec3f_t& lhs, const vec3f_t& rhs);
vec3f_t operator*(const vec3f_t& lhs, const float& other);
vec3f_t operator/(const vec3f_t& lhs, const float& other);

struct rotation_t : matharray<float, 4>
{
    const float & x() const;
    const float & y() const;
    const float & z() const;
    const float & w() const;
    
    float & x();
    float & y();
    float & z();
    float & w();
    
    rotation_t normalized() const;
    
    void normalize();
        
    rotation_t operator -() const;
    
    rotation_t(float x_, float y_, float z_, float w_);
    
    rotation_t();
};


rotation_t operator * (rotation_t const & pos, float value);
rotation_t operator * (float value, rotation_t const & pos);
rotation_t operator / (rotation_t const & lhs, float value);
rotation_t & operator += (rotation_t & lhs, rotation_t const & rhs);
rotation_t & operator -= (rotation_t & lhs, rotation_t const & rhs);
rotation_t & operator /= (rotation_t & lhs, float value);

vec3f_t & operator += (vec3f_t & lhs, vec3f_t const & rhs);
vec3f_t & operator -= (vec3f_t & lhs, vec3f_t const & rhs);
vec3f_t & operator /= (vec3f_t & lhs, float value);
vec3f_t operator * (float value, vec3f_t const & pos);

struct scale_t : std::array<float, 3>
{
    float & x();
    float & y();
    float & z();
    
    scale_t(float x_, float y_, float z_);
    
    scale_t();
};

rotation_t interpolate(rotation_t const & a, rotation_t const & b, float value);

//rotation_t interpolate(rotation_t const & a, rotation_t const & b, float value);

vec3f_t interpolate(vec3f_t const & a, vec3f_t const & b, float value);

template <typename T, size_t N>
std::ostream & operator << (std::ostream & out, matharray<T,N> const & array)
{
    return print_elements(out << '(', array.begin(), array.end(),' ') << ')';
}

struct mesh_t
{
    std::vector<vertex_t> _v;
    std::vector<vec2f_t> _uv;
    std::vector<triangle_t> _t;
};

struct configuration_t
{
    vec3f_t _pos;
    rotation_t _rot;
    scale_t _scale;
};


#endif
