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
#include <map>
#include <numeric>
#include <immintrin.h>
#include "io_util.h"
#include "types.h"

template <typename T, size_t N>
struct matharray : std::array<T,N>{
    T dot() const;

    /*inline matharray(){}

    inline explicit matharray(T init) {
        for (size_t i = 0; i < N; ++i)
        {
            (*this)[i] = init;
        }
    }

    */
    /*template <typename... Args>
    inline matharray(std::initializer_list<T> init) : std::array<T,N>({init}) {}
    */
    template <typename... Args>
    inline matharray(Args &&... args) : std::array<T,N>({T(std::forward<Args>(args))...}) {}

    explicit inline matharray(T value){std::fill(std::array<T,N>::begin(), std::array<T,N>::end(), value);}

    //inline matharray(matharray<T,N> const & other) = default;

    T norm() const;
    T sum() const;

    inline matharray<T, N> operator - (matharray<T,N> const & other)
    {
        matharray<T, N> result;
        for (size_t i = 0; i < N;++i)
        {
            result[i] = (*this)[i] - other[i];
        }
        return result;
    }

    inline matharray<T, N> operator + (matharray<T,N> const & other)
    {
        matharray<T, N> result;
        for (size_t i = 0; i < N;++i)
        {
            result[i] = (*this)[i] + other[i];
        }
        return result;
    }

    inline matharray<bool, N> operator < (matharray<T,N> const & other)
    {
        matharray<bool, N> result;
        for (size_t i = 0; i < N;++i)
        {
            result[i] = (*this)[i] < other[i];
        }
        return result;
    }

    inline matharray<bool, N> operator > (matharray<T,N> const & other)
    {
        matharray<bool, N> result;
        for (size_t i = 0; i < N;++i)
        {
            result[i] = (*this)[i] > other[i];
        }
        return result;
    }

    template <typename V>
    inline matharray<T,N> & operator +=(matharray<V,N> const & other)
    {
        for (size_t i = 0; i < N; ++i)
        {
            (*this)[i] += other[i];
        }
        return *this;
    }

    template <typename V>
    inline matharray<T,N> & operator *=(matharray<V,N> const & other)
    {
        for (size_t i = 0; i < N; ++i)
        {
            (*this)[i] *= other[i];
        }
        return *this;
    }

    inline matharray<T,N>& operator *=(T other)
    {
        for (size_t i = 0; i < N; ++i)
        {
            (*this)[i] *= other;
        }
        return *this;
    }

    inline matharray<T,N>& operator /=(T other)
    {
        for (size_t i = 0; i < N; ++i)
        {
            (*this)[i] /= other;
        }
        return *this;
    }

    inline matharray<T,N> operator *(matharray<T,N> const & other) const
    {
        matharray<T,N> result = *this;
        result *= other;
        return result;
    }

    inline matharray<T,N> operator *(T other) const
    {
        matharray<T,N> result = *this;
        result *= other;
        return result;
    }

    template <typename V>
    inline matharray<T,N> & operator /=(matharray<V,N> const & other)
    {
        for (size_t i = 0; i < N; ++i)
        {
            (*this)[i] /= other[i];
        }
        return *this;
    }

    inline matharray<T,N> operator /(T other) const
    {
        matharray<T,N> result = *this;
        result /= other;
        return result;
    }

    inline matharray<T,N> operator -() const
    {
        matharray<T,N> result;
        for (size_t i = 0; i < N; ++i)
        {
            result[i] = -(*this)[i];
        }
        return result;
    }
};

template <typename T, size_t N>
inline matharray<T,N> operator /(T const & lhs, matharray<T,N> const & rhs)
{
    matharray<T,N> result;
    for (size_t i = 0; i < N; ++i)
    {
        result[i] = lhs / rhs[i];
    }
    return result;
}

template<typename T, size_t N>
inline matharray<T,N> sse2matharray(__m128 x){
    throw std::runtime_error("Not implemented");
}


template<typename T, size_t N>
inline matharray<T,N> sse2matharray(__m128i x){
    throw std::runtime_error("Not implemented");
}

template <>
inline matharray<int32_t,3>  sse2matharray(__m128i  x) {
    return matharray<int32_t,3>({(int32_t)_mm_extract_epi32(x,0),(int32_t)_mm_extract_epi32(x,1),(int32_t)_mm_extract_epi32(x,2)});
}

template <>
inline matharray<uint32_t,3>  sse2matharray(__m128i  x) {
    return matharray<uint32_t,3>({(uint32_t)_mm_extract_epi32(x,0),(uint32_t)_mm_extract_epi32(x,1),(uint32_t)_mm_extract_epi32(x,2)});
}

template <>
inline matharray<float,3>  sse2matharray(__m128  x) {
    float result[4];
    _mm_store_ps (&result[0], x);
    return matharray<float,3>({result[0],result[1],result[2]});
}

template <size_t N>
struct matharray<bool, N> : std::array<bool,N>{
    inline matharray<bool, N> & operator |= (matharray<bool,N> const & other)
    {
        for (size_t i = 0; i < N;++i)
        {
           (*this)[i] |= other[i];
        }
        return *this;
    }

    inline matharray<bool, N> & operator &= (matharray<bool,N> const & other)
    {
        for (size_t i = 0; i < N;++i)
        {
           (*this)[i] &= other[i];
        }
        return *this;
    }

    inline matharray<bool, N> operator ! () const
    {
        matharray<bool, N> result;
        for (size_t i = 0; i < N;++i)
        {
           result[i] = !(*this)[i];
        }
        return result;
    }

    inline matharray<bool, N> operator & (matharray<bool,N> const & other) const
    {
        matharray<bool, N> result = *this;
        result &= other;
        return result;
    }
};

template <typename T, size_t N>
T matharray<T,N>::dot() const
{
    auto iter = this->cbegin();
    T res = *iter * *iter;
    while(++iter != this->cend())
    {
        res += *iter * *iter;
    }
    return res;
}

template <typename T, size_t N>
T matharray<T,N>::sum() const {return std::accumulate(this->cbegin() + 1, this->cend(), (*this)[0]);}

template <typename T, size_t N>
T matharray<T,N>::norm() const{return sqrt(dot());}

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

template <typename T, size_t N>
T distQ(matharray<T,N> const & lhs, matharray<T,N> const & rhs)
{
    auto liter = lhs.cbegin();
    auto riter = rhs.cbegin();
    T res = *liter - *riter;
    res *= res;
    while(++liter != lhs.cend())
    {
        auto tmp = *liter - *++riter;
        res += tmp * tmp;
    }
    return res;
}

struct triangle_t : std::array<uint32_t, 3>{
    uint32_t& operator*();
};

template <typename T>
struct vec2_t : matharray<T, 2>{
    inline const T & x() const;
    inline const T & y() const;
    
    inline T & x();
    inline T & y();
    
    inline vec2_t();
    inline vec2_t(T const x_, T const y_);
    explicit inline vec2_t(T const x_);
};

typedef vec2_t<float> vec2f_t;
typedef vec2_t<uint16_t> vec2us_t;

template<typename T> T const & vec2_t<T>::x() const{return (*this)[0];}
template<typename T> T const & vec2_t<T>::y() const{return (*this)[1];}

template<typename T> T & vec2_t<T>::x(){return (*this)[0];}
template<typename T> T & vec2_t<T>::y(){return (*this)[1];}

template<typename T> vec2_t<T>::vec2_t(){}
template<typename T> vec2_t<T>::vec2_t(T s) : matharray<T,2>(s){}
template<typename T> vec2_t<T>::vec2_t(T x_, T y_) : matharray<T,2>({x_,y_}){}
    
vec2f_t operator+(const vec2f_t& lhs, const vec2f_t& rhs);
vec2f_t operator-(const vec2f_t& lhs, const vec2f_t& rhs);
vec2f_t operator*(const vec2f_t& lhs, const vec2f_t& rhs);

template <typename T>
struct vec3_t : matharray<T, 3>
{
    inline const T & x() const;
    inline const T & y() const;
    inline const T & z() const;

    inline T & x();
    inline T & y();
    inline T & z();

    inline vec3_t<T> operator -() const;

    inline vec3_t();
    inline vec3_t(T init_);
    inline vec3_t(T x_, T y_, T z_);

    inline vec3_t<T> &normalize();

    template <typename V> vec3_t<V> convert_normalized() const;
};

template <typename T, typename V> vec3_t<V> convert_vector_normalized(vec3_t<T> const & v){return vec3_t<V>(v[0] * std::numeric_limits<V>::max(),v[1] * std::numeric_limits<V>::max(),v[2] * std::numeric_limits<V>::max());}  
template <typename T> vec3_t<T> & vec3_t<T>::normalize(){T div = this->norm();x()/=div; y()/=div; z()/=div; return *this;}

typedef vec3_t<float> vec3f_t;
typedef vec3_t<uint16_t> vec3us_t;
typedef vec3_t<int16_t> vec3s_t;

template <typename T>
template <typename V> 
vec3_t<V> vec3_t<T>::convert_normalized() const{return convert_vector_normalized<T,V>(*this);}

template <typename T>T const & vec3_t<T>::x() const{return (*this)[0];}
template <typename T>T const & vec3_t<T>::y() const{return (*this)[1];}
template <typename T>T const & vec3_t<T>::z() const{return (*this)[2];}

template <typename T>T & vec3_t<T>::x(){return (*this)[0];}
template <typename T>T & vec3_t<T>::y(){return (*this)[1];}
template <typename T>T & vec3_t<T>::z(){return (*this)[2];}

template <typename T>vec3_t<T> operator-(const vec3_t<T>& lhs, const vec3_t<T>& rhs){return vec3_t<T>(lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z());}
template <typename T>vec3_t<T>  vec3_t<T>::operator-() const{return vec3_t<T> (-x(), -y(), -z());}
//template <typename T>vec3_t<T> vec3_t<T>::operator-(vec3_t<T> const & rhs) const{return vec3_t<T>(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());}

template <typename T> vec3_t<T>::vec3_t(){}
template <typename T> vec3_t<T>::vec3_t(T init): matharray<T,3>({init, init, init}){}
template <typename T> vec3_t<T>::vec3_t(T x_, T y_, T z_) : matharray<T,3>({x_,y_,z_}){}

vec3f_t operator+(const vec3f_t& lhs, const vec3f_t& rhs);
//vec3f_t operator-(const vec3f_t& lhs, const vec3f_t& rhs);
vec3f_t operator*(const vec3f_t& lhs, float other);
vec3f_t operator/(const vec3f_t& lhs, float other);

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

rotation_t operator - (rotation_t const & lhs, rotation_t const & rhs);
rotation_t operator + (rotation_t const & lhs, rotation_t const & rhs);
rotation_t operator * (rotation_t const & pos, float value);
rotation_t operator * (float value, rotation_t const & pos);
rotation_t operator / (rotation_t const & lhs, float value);
rotation_t & operator += (rotation_t & lhs, rotation_t const & rhs);
rotation_t & operator -= (rotation_t & lhs, rotation_t const & rhs);
rotation_t & operator *= (rotation_t & lhs, float value);
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

template <typename T>
T lerp(T const & a, T const & b, float t)
{
    T res = a;
    res *= (1 - t);
    return res += t * b;
}

template <>
rotation_t lerp(rotation_t const & a, rotation_t const & b, float t);

template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type lerp(T a, T b, float t)
{
    return a + t * (b - a);
}

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

template <typename T>
T interpolated(std::map<frameindex_t, T> const & map, frameindex_t frame)
{
    auto up = map.lower_bound(frame);
    if (up->first == frame)
    {
        return up->second;
    }
    auto low = up;
    --low;
    //auto up = map.upper_bound(frame);
    float value = static_cast<float>(frame - low->first) / (up->first - low->first);
    //std::cout << value << '=' << '(' << frame  << '-' << low->first << ") / (" << up->first << '-'<< low->first << ')'<< std::endl;
    return lerp(low->second, up->second, value);
}

float       smoothed(std::map<frameindex_t, float>      const & map, size_t multiply, frameindex_t begin, frameindex_t end);
vec3f_t     smoothed(std::map<frameindex_t, vec3f_t>    const & map, size_t multiply, frameindex_t begin, frameindex_t end);
rotation_t  smoothed(std::map<frameindex_t, rotation_t> const & map, size_t multiply, frameindex_t begin, frameindex_t end);

#endif
