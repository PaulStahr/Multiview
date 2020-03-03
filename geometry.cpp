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

position_t::position_t(float x_, float y_, float z_)
{
    x() = x_;
    y() = y_;
    z() = z_;
}

position_t::position_t(){}

float const & position_t::x() const{return (*this)[0];}
float const & position_t::y() const{return (*this)[1];}
float const & position_t::z() const{return (*this)[2];}

float & position_t::x(){return (*this)[0];}
float & position_t::y(){return (*this)[1];}
float & position_t::z(){return (*this)[2];}

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

void rotation_t::normalize(){(*this)/=norm();}

rotation_t rotation_t::normalized() const{return (*this)/norm();}

rotation_t operator/(rotation_t const & lhs, float value){return rotation_t(lhs[0] / value, lhs[1] / value, lhs[2] / value, lhs[3] / value);}

float & scale_t::x(){return (*this)[0];}
float & scale_t::y(){return (*this)[1];}
float & scale_t::z(){return (*this)[2];}

scale_t::scale_t(float x_, float y_, float z_)
{
    x() = x_;
    y() = y_;
    z() = z_;
}

scale_t::scale_t(){}

position_t operator * (position_t const & pos, float value){return position_t(pos[0] * value, pos[1] * value, pos[2] * value);}

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


position_t & operator += (position_t & lhs, position_t const & rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

position_t & operator -= (position_t & lhs, position_t const & rhs)
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


position_t & operator /= (position_t & lhs, float value)
{
    for (float & x : lhs)
    {
        x /= value;
    }
    return lhs;
}

position_t operator * (float value, position_t const & pos)
{
    return position_t(pos[0] * value, pos[1] * value, pos[2] * value);
}

rotation_t rotation_t::operator-() const
{
    return rotation_t(-x(), -y(), -z(), -w());
}

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

position_t interpolate(position_t const & a, position_t const & b, float value)
{
    position_t ret = (1 - value) * a;
    return ret += value * b;
}

