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

vec3f_t::vec3f_t(float x_, float y_, float z_)
{
    x() = x_;
    y() = y_;
    z() = z_;
}

vec3f_t::vec3f_t(){
    x() = 0;
    y() = 0;
    z() = 0;
}

float const & vec3f_t::x() const{return (*this)[0];}
float const & vec3f_t::y() const{return (*this)[1];}
float const & vec3f_t::z() const{return (*this)[2];}

float & vec3f_t::x(){return (*this)[0];}
float & vec3f_t::y(){return (*this)[1];}
float & vec3f_t::z(){return (*this)[2];}

rotation_t::rotation_t(float x_, float y_, float z_, float w_)
{
    x() = x_;
    y() = y_;
    z() = z_;
    w() = w_;
}

rotation_t::rotation_t::rotation_t(){}

float const & rotation_t::x() const{return (*this)[0];}
float const & rotation_t::y() const{return (*this)[1];}
float const & rotation_t::z() const{return (*this)[2];}
float const & rotation_t::w() const{return (*this)[3];}

float & rotation_t::x(){return (*this)[0];}
float & rotation_t::y(){return (*this)[1];}
float & rotation_t::z(){return (*this)[2];}
float & rotation_t::w(){return (*this)[3];}

float const & vec2f_t::x() const{return (*this)[0];}
float const & vec2f_t::y() const{return (*this)[1];}

float & vec2f_t::x(){return (*this)[0];}
float & vec2f_t::y(){return (*this)[1];}

void rotation_t::normalize(){(*this)/=norm();}

rotation_t rotation_t::normalized() const{return (*this)/norm();}

rotation_t operator/(rotation_t const & lhs, float value){return rotation_t(lhs[0] / value, lhs[1] / value, lhs[2] / value, lhs[3] / value);}

float & scale_t::x(){return (*this)[0];}
float & scale_t::y(){return (*this)[1];}
float & scale_t::z(){return (*this)[2];}

scale_t::scale_t(float x_, float y_, float z_){x() = x_;y() = y_;z() = z_;}
scale_t::scale_t(){}

vec3f_t operator * (vec3f_t const & pos, float value){return vec3f_t(pos[0] * value, pos[1] * value, pos[2] * value);}

rotation_t operator * (rotation_t const & lhs, float value){return rotation_t(lhs[0] * value, lhs[1] * value, lhs[2] * value, lhs[3] * value);}
rotation_t operator * (float value, rotation_t const & rhs){return rhs * value;}

rotation_t & operator += (rotation_t & lhs, rotation_t const & rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

rotation_t & operator -= (rotation_t & lhs, rotation_t const & rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] -= rhs[i];
    }
    return lhs;
}


vec3f_t & operator += (vec3f_t & lhs, vec3f_t const & rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

vec3f_t & operator -= (vec3f_t & lhs, vec3f_t const & rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

rotation_t & operator /= (rotation_t & lhs, float value)
{
    for (float & x : lhs)
    {
        x /= value;
    }
    return lhs;
}
rotation_t rotation_t::operator-() const{return rotation_t(-x(), -y(), -z(), -w());}

vec3f_t & operator /= (vec3f_t & lhs, float value)
{
    for (float & x : lhs)
    {
        x /= value;
    }
    return lhs;
}

vec3f_t operator+(const vec3f_t& lhs, const vec3f_t& rhs){return vec3f_t(lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z());}
vec3f_t operator-(const vec3f_t& lhs, const vec3f_t& rhs){return vec3f_t(lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z());}
vec3f_t operator*(const vec3f_t& lhs, const float& other){return vec3f_t(lhs.x() * other, lhs.y() * other, lhs.z() * other);}
vec3f_t operator/(const vec3f_t& lhs, const float& other){return vec3f_t(lhs.x() / other, lhs.y() / other, lhs.z() / other);}
vec3f_t operator * (float value, vec3f_t const & pos){return vec3f_t(pos[0] * value, pos[1] * value, pos[2] * value);}

vec2f_t::vec2f_t(){x() = 0.0f;y() = 0.0f;}
vec2f_t::vec2f_t(float x_, float y_){x() = x_;y() = y_;}
    
vec2f_t operator+(const vec2f_t& lhs, const vec2f_t& rhs){return vec2f_t(lhs.x() + rhs.x(), lhs.y() + rhs.y());}
vec2f_t operator-(const vec2f_t& lhs, const vec2f_t& rhs){return vec2f_t(lhs.x() - rhs.x(), lhs.y() - rhs.y());}
vec2f_t operator*(const vec2f_t& lhs, const vec2f_t& rhs){return vec2f_t(lhs.x() * rhs.x(), lhs.y() * rhs.y());}

rotation_t interpolate(rotation_t const & a, rotation_t const & b, float value)
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

vec3f_t interpolate(vec3f_t const & a, vec3f_t const & b, float value)
{
    vec3f_t ret = (1 - value) * a;
    return ret += value * b;
}

