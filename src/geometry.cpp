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

rotation_t rotation_t::inverse() const{return rotation_t(-x(), -y(), -z(), w());}

void rotation_t::normalize(){(*this)/=norm();}

rotation_t rotation_t::normalized() const{return (*this)/norm();}

rotation_t operator/(rotation_t const & lhs, float value){return rotation_t(lhs[0] / value, lhs[1] / value, lhs[2] / value, lhs[3] / value);}

float & scale_t::x(){return (*this)[0];}
float & scale_t::y(){return (*this)[1];}
float & scale_t::z(){return (*this)[2];}

float const & scale_t::x() const{return (*this)[0];}
float const & scale_t::y() const{return (*this)[1];}
float const & scale_t::z() const{return (*this)[2];}

scale_t::scale_t(float x_, float y_, float z_){x() = x_;y() = y_;z() = z_;}
scale_t::scale_t(){}

rotation_t operator * (rotation_t const & lhs, float value){return rotation_t(lhs[0] * value, lhs[1] * value, lhs[2] * value, lhs[3] * value);}
rotation_t operator * (float value, rotation_t const & rhs){return rhs * value;}

rotation_t & operator += (rotation_t & lhs, rotation_t const & rhs)     {lhs.matharray<float,4>::operator+=(rhs); return lhs;}
rotation_t & operator -= (rotation_t & lhs, rotation_t const & rhs)     {lhs.matharray<float,4>::operator-=(rhs); return lhs;}
rotation_t & operator *= (rotation_t & lhs, float rhs)                  {lhs.matharray<float,4>::operator*=(rhs); return lhs;}
rotation_t & operator /= (rotation_t & lhs, float rhs)                  {lhs.matharray<float,4>::operator/=(rhs); return lhs;}

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

vec3f_t & operator += (vec3f_t & lhs, vec3f_t const & rhs)              {lhs.matharray<float,3>::operator+=(rhs); return lhs;}
vec3f_t & operator -= (vec3f_t & lhs, vec3f_t const & rhs)              {lhs.matharray<float,3>::operator-=(rhs); return lhs;}
vec3f_t & operator /= (vec3f_t & lhs, float rhs)                        {lhs.matharray<float,3>::operator/=(rhs); return lhs;}
rotation_t rotation_t::operator-() const{return rotation_t(-x(), -y(), -z(), -w());}

rotation_t euleraxis2quaternion(float x, float y, float z, float theta)
{
    float tsin = sin(theta * 0.5);
    float tcos = cos(theta * 0.5);
    return {x * tsin, y * tsin, z * tsin, tcos};
}
/*
struct mat44f_t{
    __m128 c[4];

    __m128 operator *= (vec3f_t const & v){return c[0] * v[0] + c[1] * v[1] + c[2] * v[2] + c[3];}
};

mat44f_t operator * (mat44f_t a, mat44f_t b){
    mat44f_t result;
  for (int i=0; i<16; i+=4) {
    vec4 rl = vec4(a) * b.c[i];
    for (int j=1; j<4; j++)
      rl += vec4(&a[j*4]) * vec4(b[i+j]);
    rl >> &r[i];
  }
    
    
}
*/
vec3f_t operator+(const vec3f_t& lhs, const vec3f_t& rhs)   {return vec3f_t(lhs.matharray<float,3>::operator+(rhs));}
vec3f_t operator*(vec3f_t const & lhs, float rhs)           {return vec3f_t(lhs.matharray<float,3>::operator*(rhs));}
vec3f_t & operator*=(vec3f_t& lhs, const float& other)      {lhs.x() *= other; lhs.y() *= other; lhs.z() *= other;return lhs;}
vec3f_t operator/(const vec3f_t& lhs, float other)          {return vec3f_t(lhs.x() / other, lhs.y() / other, lhs.z() / other);}
vec3f_t operator*(float value, vec3f_t const & pos)         {return vec3f_t(pos[0] * value, pos[1] * value, pos[2] * value);}

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
    result += b * (sin(value) * isin);
    return result;
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

    T get(){return _sum / _weight;}
};

template <typename T, typename AddFunction>
T smoothed_impl(std::map<frameindex_t, T> const & map, frameindex_t multiply, frameindex_t begin, frameindex_t end, AddFunction add_fct)
{
    if (map.empty()){throw std::runtime_error("No keyframe found");}
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
        current *= rhs - begin;
        add_fct(current, iter->second * (begin - chs));
        current /= rhs - chs;
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
            current *= rhs - end;
            add_fct(current, iter->second * (end - chs));
            current /= rhs - chs;
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
    rotation_t res = smoothed_impl(map, multiply, begin, end, [](rotation_t & lhs, rotation_t const & rhs){
        if (dot(lhs,rhs)>0)
        {
            return lhs += rhs;
        }
        else
        {
            return lhs -= rhs;
        }
    });
    res.normalize();
    return res;
}

namespace GEOMETRY
{
    inline static __m128 cross_product( __m128 const& vec0, __m128 const& vec1 ) {
    __m128 tmp0 = _mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(3,0,2,1));
    __m128 tmp1 = _mm_shuffle_ps(vec1,vec1,_MM_SHUFFLE(3,1,0,2));
    __m128 tmp2 = _mm_mul_ps(tmp0,vec1);
    __m128 tmp3 = _mm_mul_ps(tmp0,tmp1);
    __m128 tmp4 = _mm_shuffle_ps(tmp2,tmp2,_MM_SHUFFLE(3,0,2,1));
    return _mm_sub_ps(tmp3,tmp4);
}

inline __m128 load_vec(const vec3f_t & value)
{
    return _mm_setr_ps(value.x(),value.y(),value.z(),0);
}

/*    inline __m128 load_vec(const vec3f_t & value)
{
 __m128 x = _mm_load_ss(&value.x());
 __m128 y = _mm_load_ss(&value.y());
 __m128 z = _mm_load_ss(&value.z());
 __m128 xy = _mm_movelh_ps(x, y);
 return _mm_shuffle_ps(xy, z, _MM_SHUFFLE(2, 0, 2, 0));
}*/

vec3f_t CrossV3(const vec3f_t a, const vec3f_t b)
{
    /*__m128 result = cross_product(load_vec(a),load_vec(b));
    return vec3f_t(result[0],result[1],result[2]);
    */
    return vec3f_t(a.y() * b.z() - a.z() * b.y(),
        a.z() * b.x() - a.x() * b.z(),
        a.x() * b.y() - a.y() * b.x());
}

float normdot (const vec3f_t & a, const vec3f_t & b){return dot(a, b) / sqrtf(a.dot() * b.dot());}

float AngleBetweenV3(const vec3f_t a, const vec3f_t b){return acosf(normdot(a,b));}

vec3f_t ProjV3(const vec3f_t a, const vec3f_t b){return b * (dot(a, b) / b.dot());}
}
