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

struct uv_coord_t : matharray<float, 2>{};

struct position_t : matharray<float, 3>
{
    const float & x() const;
    const float & y() const;
    const float & z() const;
    
    float & x();
    float & y();
    float & z();
    
    
    position_t(float x_, float y_, float z_);
    
    position_t();
};

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


position_t operator * (position_t const & pos, float value);
rotation_t operator * (rotation_t const & pos, float value);
rotation_t operator * (float value, rotation_t const & pos);

rotation_t & operator += (rotation_t & lhs, rotation_t const & rhs);

rotation_t & operator -= (rotation_t & lhs, rotation_t const & rhs);

rotation_t operator /(rotation_t const & lhs, float value);

position_t & operator += (position_t & lhs, position_t const & rhs);

position_t & operator -= (position_t & lhs, position_t const & rhs);

rotation_t & operator /= (rotation_t & lhs, float value);

position_t & operator /= (position_t & lhs, float value);

position_t operator * (float value, position_t const & pos);

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

position_t interpolate(position_t const & a, position_t const & b, float value);



template <typename T, size_t N>
std::ostream & operator << (std::ostream & out, matharray<T,N> const & array)
{
    return print_elements(out << '(', array.begin(), array.end(),' ') << ')';
}

struct mesh_t
{
    std::vector<vertex_t> _v;
    std::vector<uv_coord_t> _uv;
    std::vector<triangle_t> _t;
};



struct configuration_t
{
    position_t _pos;
    rotation_t _rot;
    scale_t _scale;
};


#endif
