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

#include "geometry.h"
#include <numeric>

rotation_t::rotation_t(float x_, float y_, float z_, float w_) : matharray<float,4>({x_,y_,z_,w_}){}

rotation_t::rotation_t::rotation_t() : matharray<float,4>(){}

float const & rotation_t::x() const{return (*this)[0];}
float const & rotation_t::y() const{return (*this)[1];}
float const & rotation_t::z() const{return (*this)[2];}
float const & rotation_t::w() const{return (*this)[3];}

float & rotation_t::x(){return (*this)[0];}
float & rotation_t::y(){return (*this)[1];}
float & rotation_t::z(){return (*this)[2];}
float & rotation_t::w(){return (*this)[3];}

void rotation_t::normalize(){(*this)/=norm();}

rotation_t rotation_t::normalized() const{return (*this)/norm();}

rotation_t operator/(rotation_t const & lhs, float value){return rotation_t(lhs[0] / value, lhs[1] / value, lhs[2] / value, lhs[3] / value);}

float & scale_t::x(){return (*this)[0];}
float & scale_t::y(){return (*this)[1];}
float & scale_t::z(){return (*this)[2];}

scale_t::scale_t(float x_, float y_, float z_){x() = x_;y() = y_;z() = z_;}
scale_t::scale_t(){}


rotation_t operator * (rotation_t const & lhs, float value){return rotation_t(lhs[0] * value, lhs[1] * value, lhs[2] * value, lhs[3] * value);}
rotation_t operator * (float value, rotation_t const & rhs){return rhs * value;}

rotation_t & operator += (rotation_t & lhs, rotation_t const & rhs)
{
    for (size_t i = 0; i < 4; ++i){lhs[i] += rhs[i];}
    return lhs;
}

rotation_t & operator *= (rotation_t & lhs, float rhs)
{
    for (size_t i = 0; i < 4; ++i){lhs[i] *= rhs;}
    return lhs;
}

rotation_t operator -(rotation_t const & lhs, rotation_t const & rhs)
{
    rotation_t result = lhs;
    result -= rhs;
    return result;
}

rotation_t operator +(rotation_t const & lhs, rotation_t const & rhs)
{
    rotation_t result = lhs;
    result += rhs;
    return result;
}

rotation_t & operator -= (rotation_t & lhs, rotation_t const & rhs)
{
    for (size_t i = 0; i < 4; ++i){lhs[i] -= rhs[i];}
    return lhs;
}

vec3f_t & operator += (vec3f_t & lhs, vec3f_t const & rhs)
{
    for (int i = 0; i < 3; ++i){lhs[i] += rhs[i];}
    return lhs;
}

vec3f_t & operator -= (vec3f_t & lhs, vec3f_t const & rhs)
{
    for (int i = 0; i < 3; ++i){lhs[i] -= rhs[i];}
    return lhs;
}

rotation_t & operator /= (rotation_t & lhs, float value)
{
    for (float & x : lhs){x /= value;}
    return lhs;
}
rotation_t rotation_t::operator-() const{return rotation_t(-x(), -y(), -z(), -w());}

vec3f_t & operator /= (vec3f_t & lhs, float value)
{
    for (float & x : lhs){x /= value;}
    return lhs;
}

vec3f_t operator+(const vec3f_t& lhs, const vec3f_t& rhs){return vec3f_t(lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z());}
vec3f_t operator*(vec3f_t const & pos, float value){return vec3f_t(pos[0] * value, pos[1] * value, pos[2] * value);}
vec3f_t & operator*=(vec3f_t& lhs, const float& other){lhs.x() *= other; lhs.y() *= other; lhs.z() *= other;return lhs;}
vec3f_t operator/(const vec3f_t& lhs, float other){return vec3f_t(lhs.x() / other, lhs.y() / other, lhs.z() / other);}
vec3f_t operator*(float value, vec3f_t const & pos){return vec3f_t(pos[0] * value, pos[1] * value, pos[2] * value);}

vec2f_t operator+(const vec2f_t& lhs, const vec2f_t& rhs){return vec2f_t(lhs.x() + rhs.x(), lhs.y() + rhs.y());}
vec2f_t operator-(const vec2f_t& lhs, const vec2f_t& rhs){return vec2f_t(lhs.x() - rhs.x(), lhs.y() - rhs.y());}
vec2f_t operator*(const vec2f_t& lhs, const vec2f_t& rhs){return vec2f_t(lhs.x() * rhs.x(), lhs.y() * rhs.y());}

template <>
rotation_t lerp(rotation_t const & a, rotation_t const & b, float value)
{
    float d = dot(a,b);
    rotation_t flipped = a;
    if (d < 0.0f) {
        flipped = -a;
        d = -d;
    }
    
    const double DOT_THRESHOLD = 0.9995;
    if (d > DOT_THRESHOLD) {
        rotation_t result = value * b;
        result += (1 - value) * flipped;
        result.normalize();
        return result;
    }
    float omega = acos(d);
    float isin = 1./sin(omega);
    value *= omega;
    rotation_t result = flipped * (sin(omega - value) * isin);
    return result += b * (sin(value) * isin);
}

/*rotation_t interpolate(rotation_t const & a, rotation_t const & b, float value)
{
    rotation_t result = (1 - value) * a;
    if (dot(a,b) > 0)
    {
        return result += b * value;
    }
    else
    {
        return result -= b * value;
    }
    return result.normalized();
}*/

uint32_t& triangle_t::operator*(){return (*this)[0];}

rotation_t smoothed(std::map<frameindex_t, rotation_t> const & map, frameindex_t frame, frameindex_t smoothing)
{
    rotation_t result(0,0,0,0);
    for (frameindex_t i = frame - smoothing; i <= frame + smoothing; ++i)
    {
        rotation_t const & tmp = interpolated(map, i + frame);
        if (dot(result, tmp) < 0)
        {
            result -= tmp;
        }
        else
        {
            result += tmp;
        }
    }
    return result.normalized();
}

vec3f_t smoothed(std::map<frameindex_t, vec3f_t> const & map, frameindex_t frame, frameindex_t smoothing)
{
    vec3f_t result(0,0,0);
    for (frameindex_t i = 0; i < smoothing * 2 + 1; ++i)
    {
        result += interpolated(map, i + frame - smoothing);
    }
    return result /= smoothing * 2 + 1;
}

template <typename T>
T divceil(T x, T y)
{
    assert(y != 0);
    return x / y + (x % y > 0);
}


template <typename T>
T divfloor(T x, T y)
{
    assert(y != 0);
    return x / y - (x % y < 0);
}

template <typename T, typename V, typename AddFunction>
struct weighted_average_t{
    T _sum;
    V _weight;
    AddFunction _add_fct;

    weighted_average_t(T value_, V weight_, AddFunction add_fct_):_sum(value_ * weight_), _weight(weight_), _add_fct(add_fct_){}

    void add(T value_, V weight_){_add_fct(_sum, value_ * weight_); _weight += weight_;}

    T get(){std::cout << std::endl; return _sum / _weight;}
};

template <typename T, typename AddFunction>
T smoothed_impl(std::map<frameindex_t, T> const & map, frameindex_t multiply, frameindex_t begin, frameindex_t end, AddFunction add_fct)
{
    begin *= 2;
    end *= 2;
    multiply *= 2;
    auto iter = map.upper_bound(divfloor(begin, multiply));  //First frame lower or equal begin
    if (iter != map.begin()){--iter;}
    frameindex_t chs = iter->first * multiply;
    T current = iter->second;
    ++iter;
    if (chs >= end || iter == map.end()){return current;} //Fast path, only one element relevant
    frameindex_t rhs = iter->first * multiply;
    if (chs <= begin && end <= rhs)  //Fast path, no element inside interval
    {
        frameindex_t center = (begin + end) / 2;
        weighted_average_t<T,frameindex_t, AddFunction> result(current, rhs - center, add_fct);
        result.add(iter->second, center - chs);
        return result.get();
    }
    if (chs < begin)
    {
        current = (current * (rhs - begin) + iter->second * (begin - chs)) / (rhs - chs);
        chs = begin;
    }
    weighted_average_t<T,frameindex_t, AddFunction> result(current, std::min(rhs,end) - begin + chs - begin, add_fct);
    while (rhs < end)
    {
        current = iter->second;
        ++iter;
        frameindex_t lhs = chs;
        chs = rhs;
        rhs = iter == map.end() ? end : iter->first * multiply;
        result.add(current, std::min(end, rhs) - lhs);
    }
    if (rhs > chs)
    {
        if (iter != map.end())
        {
            current = (current * (rhs - end) + iter->second * (end - chs)) / (rhs - chs);
        }
        result.add(current, end - chs);
    }
    return result.get();
}

float smoothed(std::map<frameindex_t, float> const & map, size_t multiply, frameindex_t begin, frameindex_t end)
{
    return smoothed_impl(map, multiply, begin, end, [](float & lhs, float const & rhs){lhs += rhs;});
}

vec3f_t smoothed(std::map<frameindex_t, vec3f_t> const & map, size_t multiply, frameindex_t begin, frameindex_t end)
{
    return smoothed_impl(map, multiply, begin, end, [](vec3f_t & lhs, vec3f_t const & rhs){lhs += rhs;});
}

rotation_t smoothed(std::map<frameindex_t, rotation_t> const & map, size_t multiply, frameindex_t begin, frameindex_t end)
{
    return smoothed_impl(map, multiply, begin, end, [](rotation_t & lhs, rotation_t const & rhs){
        if (dot(lhs,rhs)>0)
        {
            return lhs += rhs;
        }
        else
        {
            return lhs -= rhs;
        }
    });
}

