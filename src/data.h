#ifndef DATA_H
#define DATA_H

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QMatrix4x4>
#include <mutex>
#include <future>
#include "OBJ_Loader.h"
#include "image_util.h"
#include "geometry.h"

enum viewmode_t
{
    EQUIDISTANT, EQUIDISTANT_APPROX, PERSPECTIVE
};

enum viewtype_t
{
    VIEWTYPE_RENDERED, VIEWTYPE_POSITION, VIEWTYPE_DEPTH, VIEWTYPE_FLOW, VIEWTYPE_INDEX
};



struct wait_for_rendered_frame
{
    size_t _frame;
    volatile bool _value = false;
    std::condition_variable _cv;

    wait_for_rendered_frame(size_t value_) :_frame(value_) {}

    inline bool operator()() const
    {
        return _value;
    }
};


struct exec_env
{
    std::mutex _mtx;
    std::vector<std::future<void> > _pending_futures;

    exec_env() {}
};

struct screenshot_handle_t
{
    std::string _camera;
    viewtype_t _type;
    size_t _width;
    size_t _height;
    size_t _channels;
    size_t _datatype;
    bool _ignore_nan;
    void *_data = nullptr;
    size_t _error_code;
    std::mutex _mtx;
    std::condition_variable _cv;

    bool operator()()
    {
        return _data != nullptr || _error_code != 0;
    }
};

struct arrow_t
{
    float _x0, _y0, _x1, _y1;
};
struct view_t
{
    std::string const & _camera;
    GLuint *_cubemap_texture;
    size_t _x, _y, _width, _height;
    bool _diff;
    bool _depth;
};

struct named_image
{
    advanced_image_base_t *img;
};

struct framelist_t
{
    std::string _name;
    std::vector<size_t> _frames;

    framelist_t(std::string const & name_, std::vector<size_t> const & framelist_) :_name(name_), _frames(framelist_)
    {
    }
};
struct object_t
{
    std::string _name;
    size_t _id;
    std::map<size_t, vec3f_t> _key_pos;
    std::map<size_t, rotation_t> _key_rot;
    QMatrix4x4 _transformation;
    bool _visible;

    object_t(std::string const & name_):_name(name_), _id(0),  _transformation({1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}), _visible(true) {}
};

struct mesh_object_t: object_t
{
    QOpenGLTexture *_tex_Ka = nullptr;
    // Diffuse Texture Map
    QOpenGLTexture *_tex_Kd = nullptr;
    // Specular Texture Map
    QOpenGLTexture *_tex_Ks;
    // Specular Hightlight Map
    QOpenGLTexture *_tex_Ns;
    // Alpha Texture Map
    QOpenGLTexture *_tex_d;
    // Bump Map
    QOpenGLTexture *_tex_bump;
    objl::Loader _loader;
    std::vector<GLuint> _vbo;
    std::vector<GLuint> _vbi;

    mesh_object_t(std::string const & name_, std::string const & objfile);
};

struct camera_t : object_t
{
    viewmode_t _viewmode;
    camera_t(std::string const & name_) : object_t(name_), _viewmode(PERSPECTIVE) {}
};

struct scene_t
{
    std::vector<camera_t> _cameras;
    std::vector<mesh_object_t> _objects;
    std::vector<framelist_t> _framelists;
    std::vector<screenshot_handle_t *> _screenshot_handles;
    std::mutex _mtx;
    
    camera_t * get_camera(std::string const & name);

    object_t * get_object(std::string const & name);

    object_t & get_object(size_t index);

    size_t num_objects() const;
};



#endif
