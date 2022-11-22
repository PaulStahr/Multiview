#ifndef GL_RESOURCE_ID
#define GL_RESOURCE_ID

#include <GL/gl.h>
#include <functional>
#include <atomic>

class gl_resource_id
{
public:
private:
    GLuint _id;
    std::function<void(GLuint)> _remove;
public:
    gl_resource_id();
    gl_resource_id(gl_resource_id &&other);
    gl_resource_id & operator=(gl_resource_id&&);
    gl_resource_id(GLuint id, std::function<void(GLuint)> remove);
    operator GLuint() const { return _id; }

    gl_resource_id & operator=(const gl_resource_id&) = delete;
    gl_resource_id(const gl_resource_id&) = delete;
    
    ~gl_resource_id();
    void destroy();
};

class gl_buffer_id : public gl_resource_id{
public:
    static std::atomic<size_t> count;
    gl_buffer_id();
    gl_buffer_id(gl_buffer_id &&other);
    gl_buffer_id & operator=(gl_buffer_id&&);
    gl_buffer_id(GLuint id, std::function<void(GLuint)> remove);
    ~gl_buffer_id();
    void destroy();
};

class gl_texture_id : public gl_resource_id{
public:
    static std::atomic<size_t> count;
    gl_texture_id();
    gl_texture_id(gl_texture_id &&other);
    gl_texture_id & operator=(gl_texture_id&&);
    gl_texture_id(GLuint id, std::function<void(GLuint)> remove);
    ~gl_texture_id();
    void destroy();
};

class gl_framebuffer_id : public gl_resource_id{
public:
    static std::atomic<size_t> count;
    gl_framebuffer_id();
    gl_framebuffer_id(gl_framebuffer_id &&other);
    gl_framebuffer_id & operator=(gl_framebuffer_id&&);
    gl_framebuffer_id(GLuint id, std::function<void(GLuint)> remove);
    ~gl_framebuffer_id();
    void destroy();
};

class gl_renderbuffer_id : public gl_resource_id{
public:
    static std::atomic<size_t> count;
    gl_renderbuffer_id();
    gl_renderbuffer_id(gl_renderbuffer_id &&other);
    gl_renderbuffer_id & operator=(gl_renderbuffer_id&&);
    gl_renderbuffer_id(GLuint id, std::function<void(GLuint)> remove);
    ~gl_renderbuffer_id();
    void destroy();
};

#endif
