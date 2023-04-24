#include <GL/gl.h>
#include <cstdint>
#include <array>
#include <functional>
#include <memory>
#include "types.h"

#ifndef GL_UTIL
#define GL_UTIL

void inline set_activated(GLuint id, bool activated)
{
    if (activated){glEnable(id);}else{glDisable(id);}
}

template <typename T> struct gl_type_struct {enum{value = GL_INVALID_ENUM};};
template <> struct gl_type_struct<uint8_t>  {enum{value = GL_UNSIGNED_BYTE};};
template <> struct gl_type_struct<int8_t>   {enum{value = GL_BYTE};};
template <> struct gl_type_struct<uint16_t> {enum{value = GL_UNSIGNED_SHORT};};
template <> struct gl_type_struct<int16_t>  {enum{value = GL_SHORT};};
template <> struct gl_type_struct<uint32_t> {enum{value = GL_UNSIGNED_INT};};
template <> struct gl_type_struct<int32_t>  {enum{value = GL_INT};};
template <> struct gl_type_struct<uint64_t> {enum{value = GL_INVALID_ENUM};};
template <> struct gl_type_struct<int64_t>  {enum{value = GL_INVALID_ENUM};};
template <> struct gl_type_struct<float>    {enum{value = GL_FLOAT};};
template <> struct gl_type_struct<double>   {enum{value = GL_DOUBLE};};

template <typename T>const GLuint gl_type = gl_type_struct<T>::value;

const std::array<GLuint,10> gl_types = {{
    gl_type<uint8_t>,
    gl_type<int8_t>,
    gl_type<uint16_t>,
    gl_type<int16_t>,
    gl_type<uint32_t>,
    gl_type<int32_t>,
    gl_type<uint64_t>,
    gl_type<int64_t>,
    gl_type<float>,
    gl_type<double>
}};

GLuint inline get_gl_type(PRIMITIVE_TYPE pt)
{
    return gl_types[pt];
}

void setEnabled(GLuint option, bool activated);

template <typename V, typename MakePointer>
inline void gen_resources(size_t count, V output_iter, std::function<void(GLsizei, GLuint*)> allocator, std::function<void(GLuint)> deleter, MakePointer mp)
{
    while (count > 0)
    {
        std::array<GLuint, 32> tmp_id;
        size_t blk = std::min(count, tmp_id.size());
        allocator(blk, &tmp_id[0]);
        for (size_t i = 0; i < blk; ++i)
        {
            *output_iter = mp(tmp_id[i],deleter);
            ++output_iter;
        }
        count -= blk;
    }
}

template <typename T, typename V>
void gen_resources_shared(size_t count, V output_iter, std::function<void(GLsizei, GLuint*)> allocator, std::function<void(GLuint)> deleter)
{
    gen_resources(count, output_iter, allocator, deleter, [](auto id, auto deleter){return std::make_shared<T>(id,deleter);});
}

template <typename T, typename V>
void gen_resources_unique(size_t count, V output_iter, std::function<void(GLsizei, GLuint*)> allocator, std::function<void(GLuint)> deleter)
{
    gen_resources(count, output_iter, allocator, deleter, [](auto id, auto deleter){return std::make_unique<T>(id,deleter);});
}

template <typename T, typename V>
void gen_resources_direct(size_t count, V output_iter, std::function<void(GLsizei, GLuint*)> allocator, std::function<void(GLuint)> deleter)
{
    gen_resources(count, output_iter, allocator, deleter, [](auto id, auto deleter){return T(id,deleter);});
}
#endif
