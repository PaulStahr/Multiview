#include "gl_resource_id.h"
#include <stdexcept>

std::atomic<size_t> gl_buffer_id::count = 0;
std::atomic<size_t> gl_texture_id::count = 0;
std::atomic<size_t> gl_framebuffer_id::count = 0;
std::atomic<size_t> gl_renderbuffer_id::count = 0;

gl_resource_id::gl_resource_id(GLuint id, std::function<void(GLuint)> remove) : _id(id), _remove(remove){}

gl_resource_id::gl_resource_id(gl_resource_id &&other) : _id(std::move(other._id)), _remove(std::move(other._remove)) {other._id = 0; other._remove = nullptr;}
gl_buffer_id::gl_buffer_id              (gl_buffer_id &&other)          : gl_resource_id(std::move(other)){}
gl_texture_id::gl_texture_id            (gl_texture_id &&other)         : gl_resource_id(std::move(other)){}
gl_framebuffer_id::gl_framebuffer_id    (gl_framebuffer_id &&other)     : gl_resource_id(std::move(other)){}
gl_renderbuffer_id::gl_renderbuffer_id  (gl_renderbuffer_id &&other)    : gl_resource_id(std::move(other)){}

gl_resource_id      & gl_resource_id    ::operator=(gl_resource_id    &&other) {destroy(); _id = std::move(other._id); _remove = std::move(other._remove); other._id = 0; other._remove = nullptr; return *this;}
gl_buffer_id        & gl_buffer_id      ::operator=(gl_buffer_id      &&other) {gl_resource_id::operator=(std::move(other)); return *this;}
gl_texture_id       & gl_texture_id     ::operator=(gl_texture_id     &&other) {gl_resource_id::operator=(std::move(other)); return *this;}
gl_framebuffer_id   & gl_framebuffer_id ::operator=(gl_framebuffer_id &&other) {gl_resource_id::operator=(std::move(other)); return *this;}
gl_renderbuffer_id  & gl_renderbuffer_id::operator=(gl_renderbuffer_id&&other) {gl_resource_id::operator=(std::move(other)); return *this;}

gl_buffer_id::gl_buffer_id              () : gl_resource_id(0, nullptr){}
gl_texture_id::gl_texture_id            () : gl_resource_id(0, nullptr){}
gl_framebuffer_id::gl_framebuffer_id    () : gl_resource_id(0, nullptr){}
gl_renderbuffer_id::gl_renderbuffer_id  () : gl_resource_id(0, nullptr){}

gl_buffer_id::gl_buffer_id              (GLuint id, std::function<void(GLuint)> remove) : gl_resource_id(id, remove){if (id){++count;}}
gl_texture_id::gl_texture_id            (GLuint id, std::function<void(GLuint)> remove) : gl_resource_id(id, remove){if (id){++count;}}
gl_framebuffer_id::gl_framebuffer_id    (GLuint id, std::function<void(GLuint)> remove) : gl_resource_id(id, remove){if (id){++count;}}
gl_renderbuffer_id::gl_renderbuffer_id  (GLuint id, std::function<void(GLuint)> remove) : gl_resource_id(id, remove){if (id){++count;}}

void gl_resource_id::destroy(){
    if (_remove && _id){
        _remove(_id);
    }else if (_id){
        throw std::runtime_error("Can't delete texture " + _id);
    }
    _id = 0;
}

void gl_buffer_id::destroy()        {if (*this){--count;}gl_resource_id::destroy();}
void gl_texture_id::destroy()       {if (*this){--count;}gl_resource_id::destroy();}
void gl_framebuffer_id::destroy()   {if (*this){--count;}gl_resource_id::destroy();}
void gl_renderbuffer_id::destroy()  {if (*this){--count;}gl_resource_id::destroy();}

gl_resource_id::~gl_resource_id(){destroy();}

gl_buffer_id::      ~gl_buffer_id()      {destroy();}
gl_texture_id::     ~gl_texture_id()     {destroy();}
gl_framebuffer_id:: ~gl_framebuffer_id() {destroy();}
gl_renderbuffer_id::~gl_renderbuffer_id(){destroy();}
