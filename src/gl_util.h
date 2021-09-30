#include <GL/gl.h>
#include <cstdint>

#ifndef GL_UTIL
#define GL_UTIL

void inline set_activated(GLuint id, bool activated)
{
    if (activated){glEnable(id);}else{glDisable(id);}
}

template <typename T> struct gl_type_struct{enum {value = 0};};
template <> struct gl_type_struct<uint8_t>{enum{value = GL_UNSIGNED_BYTE};};
template <> struct gl_type_struct<int8_t>{enum{value = GL_BYTE};};
template <> struct gl_type_struct<uint16_t>{enum{value = GL_UNSIGNED_SHORT};};
template <> struct gl_type_struct<int16_t>{enum{value = GL_SHORT};};
template <> struct gl_type_struct<uint32_t>{enum{value = GL_UNSIGNED_INT};};
template <> struct gl_type_struct<int32_t>{enum{value = GL_INT};};
template <> struct gl_type_struct<float>{enum{value = GL_FLOAT};};

template <typename T>GLint gl_type = gl_type_struct<T>::value;
#endif
