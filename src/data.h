#ifndef DATA_H
#define DATA_H

#include <GL/gl.h>
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

enum PendingFlag{PENDING_THREAD = 0x1, PENDING_SCENE_EDIT = 0x2, PENDING_FILE_WRITE = 0x4, PENDING_TEXTURE_READ = 0x8, PENDING_FILE_READ = 0x16, PENDING_ALL = 0x31, PENDING_NONE = 0};

inline PendingFlag operator~   (PendingFlag           a)                { return (PendingFlag)~(int)  a; }
inline PendingFlag operator|   (PendingFlag           a, PendingFlag b) { return (PendingFlag)((int)  a |  (int)b); }
inline PendingFlag operator&   (PendingFlag           a, PendingFlag b) { return (PendingFlag)((int)  a &  (int)b); }
inline PendingFlag operator^   (PendingFlag           a, PendingFlag b) { return (PendingFlag)((int)  a ^  (int)b); }
inline PendingFlag& operator|= (PendingFlag&          a, PendingFlag b) { return (PendingFlag&)((int&)a |= (int)b); }
inline PendingFlag& operator|= (volatile PendingFlag& a, PendingFlag b) { return (PendingFlag&)((int&)a |= (int)b); }
inline PendingFlag& operator&= (PendingFlag&          a, PendingFlag b) { return (PendingFlag&)((int&)a &= (int)b); }
inline PendingFlag& operator&= (volatile PendingFlag& a, PendingFlag b) { return (PendingFlag&)((int&)a &= (int)b); }
inline PendingFlag& operator^= (PendingFlag&          a, PendingFlag b) { return (PendingFlag&)((int&)a ^= (int)b); }

struct pending_task_t
{
    std::future<void> _future;
    PendingFlag volatile _flags;
    std::mutex _mutex;
    std::condition_variable _cond_var;
    
    void set(PendingFlag flag);
    void unset(PendingFlag flag);
    void assign(PendingFlag flag);
    void wait_unset(PendingFlag flag);
    void wait_set(PendingFlag flag);
    pending_task_t(std::future<void> & future_, PendingFlag flags_);
    pending_task_t(PendingFlag flags_);
    pending_task_t(pending_task_t&& other);
    pending_task_t& operator=(pending_task_t&& other);
    bool is_deletable() const;
};

struct exec_env
{
    std::mutex _mtx;
    std::vector<pending_task_t*> _pending_tasks;
    std::string _script_dir;
    
    exec_env(std::string const & script_dir_) :_script_dir(script_dir_) {}
    
    void clean();
    
    pending_task_t & emitPendingTask();
    
    void emplace_back(pending_task_t &task);
    
    void join(pending_task_t const * self, PendingFlag flag);
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
    GLuint _bufferAddress;
    
    size_t num_elements() const;
    
    size_t size() const;

    bool operator()() const;
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
