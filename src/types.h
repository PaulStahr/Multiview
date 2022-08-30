#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

typedef int64_t frameindex_t;

enum PRIMITIVE_TYPE{
    PRIMITIVE_TYPE_UINT8_T  = 0,
    PRIMITIVE_TYPE_INT8_T   = 1,
    PRIMITIVE_TYPE_UINT16_T = 2,
    PRIMITIVE_TYPE_INT16_T  = 3,
    PRIMITIVE_TYPE_UINT32_T = 4,
    PRIMITIVE_TYPE_INT32_T  = 5,
    PRIMITIVE_TYPE_UINT64_T = 6,
    PRIMITIVE_TYPE_INT64_T  = 7,
    PRIMITIVE_TYPE_FLOAT    = 8,
    PRIMITIVE_TYPE_DOUBLE   = 9,
    PRIMITIVE_TYPE_END      =10
};

template <typename T> struct primitive_type_struct {enum{value = PRIMITIVE_TYPE_END};};
template <> struct primitive_type_struct<uint8_t>  {enum{value = PRIMITIVE_TYPE_UINT8_T};};
template <> struct primitive_type_struct<int8_t>   {enum{value = PRIMITIVE_TYPE_INT8_T};};
template <> struct primitive_type_struct<uint16_t> {enum{value = PRIMITIVE_TYPE_UINT16_T};};
template <> struct primitive_type_struct<int16_t>  {enum{value = PRIMITIVE_TYPE_INT16_T};};
template <> struct primitive_type_struct<uint32_t> {enum{value = PRIMITIVE_TYPE_UINT32_T};};
template <> struct primitive_type_struct<int32_t>  {enum{value = PRIMITIVE_TYPE_INT32_T};};
template <> struct primitive_type_struct<uint64_t> {enum{value = PRIMITIVE_TYPE_UINT64_T};};
template <> struct primitive_type_struct<float>    {enum{value = PRIMITIVE_TYPE_FLOAT};};
template <> struct primitive_type_struct<double>   {enum{value = PRIMITIVE_TYPE_DOUBLE};};

template <typename T>const PRIMITIVE_TYPE primitive_type_enum = PRIMITIVE_TYPE(primitive_type_struct<T>::value);

#endif
