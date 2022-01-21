#ifndef DATA_H
#define DATA_H

#include <GL/gl.h>
#include <GL/glext.h>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QMatrix4x4>
#include <mutex>
#include <future>
#include <atomic>
#include <vector>
#include <condition_variable>
#include "counting_semaphore.h"
#include "OBJ_Loader.h"
#include "image_util.h"
#include "geometry.h"
#include "gl_util.h"

enum RedrawScedule{REDRAW_ALWAYS, REDRAW_AUTOMATIC, REDRAW_MANUAL, REDRAW_INVALID};

enum viewmode_t
{
    EQUIDISTANT, EQUIDISTANT_APPROX, PERSPECTIVE
};

enum viewtype_t
{
    VIEWTYPE_RENDERED = 0, VIEWTYPE_POSITION = 1, VIEWTYPE_DEPTH = 2, VIEWTYPE_FLOW = 3, VIEWTYPE_INDEX = 4
};

enum coordinate_system_t
{
    COORDINATE_SPHERICAL_APPROXIMATED, COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS, COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS
};

class gl_resource_id
{
public:
    static std::atomic<size_t> count;
private:
    GLuint _id;
    std::function<void(GLuint)> _remove;
    gl_resource_id() = delete;
public:
    gl_resource_id(GLuint id, std::function<void(GLuint)> remove);
    operator GLuint() const { return _id; }

    gl_resource_id & operator=(const gl_resource_id&) = delete;
    
    ~gl_resource_id();
};

class gl_buffer_id : public gl_resource_id{
public:
    gl_buffer_id(GLuint id, std::function<void(GLuint)> remove);
};

class gl_texture_id : public gl_resource_id{
public:
    gl_texture_id(GLuint id, std::function<void(GLuint)> remove);
};

class gl_framebuffer_id : public gl_resource_id{
public:
    gl_framebuffer_id(GLuint id, std::function<void(GLuint)> remove);
};

class gl_renderbuffer_id : public gl_resource_id{
public:
    gl_renderbuffer_id(GLuint id, std::function<void(GLuint)> remove);
};

static std::shared_ptr<gl_texture_id> invalid_texture = std::make_shared<gl_texture_id>(GL_INVALID_VALUE, nullptr);

struct rendered_framebuffer_t
{
    std::shared_ptr<gl_texture_id> _rendered;
    std::shared_ptr<gl_texture_id> _position;
    std::shared_ptr<gl_texture_id> _depth;
    std::shared_ptr<gl_texture_id> _flow;
    std::shared_ptr<gl_texture_id> _index;
    
    std::shared_ptr<gl_texture_id> get(viewtype_t viewtype);

    std::shared_ptr<gl_texture_id> *begin()
    {
        return &_rendered;
    }
};

enum depthbuffer_size_t{DEPTHBUFFER_16_BIT = 0, DEPTHBUFFER_24_BIT = 1, DEPTHBUFFER_32_BIT = 2};

struct wait_for_rendered_frame_t
{
    size_t _frame;
    std::atomic<bool> _value;
    std::condition_variable _cv;

    wait_for_rendered_frame_t() : _value(false){}

    wait_for_rendered_frame_t(size_t value_) :_frame(value_) {}

    inline bool operator()() const{return _value;}
};

std::ostream & operator<<(std::ostream & out, wait_for_rendered_frame_t const & wait_obj);

enum PendingFlag{PENDING_THREAD = 0x1,
     PENDING_SCENE_EDIT         = 0x2,
     PENDING_FILE_WRITE         = 0x4,
     PENDING_TEXTURE_READ       = 0x8,
     PENDING_FILE_READ          = 0x10,
     PENDING_ALL                = 0x1F,
     PENDING_NONE               = 0};

inline PendingFlag operator~   (PendingFlag  a)                { return (PendingFlag)~(int)  a; }
inline PendingFlag operator|   (PendingFlag  a, PendingFlag b) { return (PendingFlag)((int)  a |  (int)b); }
inline PendingFlag operator&   (PendingFlag  a, PendingFlag b) { return (PendingFlag)((int)  a &  (int)b); }
inline PendingFlag operator^   (PendingFlag  a, PendingFlag b) { return (PendingFlag)((int)  a ^  (int)b); }
inline PendingFlag& operator|= (PendingFlag& a, PendingFlag b) { return (PendingFlag&)((int&)a |= (int)b); }
inline PendingFlag& operator&= (PendingFlag& a, PendingFlag b) { return (PendingFlag&)((int&)a &= (int)b); }
inline PendingFlag& operator^= (PendingFlag& a, PendingFlag b) { return (PendingFlag&)((int&)a ^= (int)b); }

struct pending_task_t
{
    std::future<void> _future;
    std::atomic<PendingFlag> _flags;
    std::mutex _mutex;
    std::condition_variable _cond_var;
    std::string const _description;
    
    pending_task_t & operator=(const pending_task_t&) = delete;
    pending_task_t(const pending_task_t&) = delete;
    pending_task_t() = delete;
    pending_task_t(std::future<void> & future_, PendingFlag flags_, std::string const & description_);
    pending_task_t(PendingFlag flags_, std::string const & description_);

    void set(PendingFlag flag);
    void unset(PendingFlag flag);
    void assign(PendingFlag flag);
    void wait_unset(PendingFlag flag);
    void wait_set(PendingFlag flag);
    bool is_deletable() const;
};

std::ostream & operator << (std::ostream & out, pending_task_t const & pending);

enum code_control_type_t{CODE_TRUE_IF, CODE_FALSE_IF};

struct exec_env
{
    std::mutex _mtx;
    std::vector<pending_task_t*> _pending_tasks;
    std::string _script_dir;
    std::vector<code_control_type_t> _code_stack;
    counting_semaphore num_threads_;
    
    exec_env(const exec_env&) = delete;
 
    exec_env(std::string const & script_dir_);
    
    void clean();
    
    void clean_impl();
    
    pending_task_t & emitPendingTask(std::string const & description);
    
    void emplace_back(pending_task_t &task);
    
    /*Wait for all pending tasks except self*/
    void join(pending_task_t const * self, PendingFlag flag);

    void join_impl(pending_task_t const * self, PendingFlag flag);
    
    bool code_active() const;
    
    ~exec_env();
};

enum screenshot_state{
    screenshot_state_inited = 0,
    screenshot_state_queued = 1,
    screenshot_state_rendered_texture = 2,
    screenshot_state_rendered_buffer = 3,
    screenshot_state_copied = 4,
    screenshot_state_saved = 5,
    screenshot_state_error = 6};

enum screenshot_task{
    TAKE_SCREENSHOT = 0,
    SAVE_TEXTURE = 1,
    RENDER_TO_TEXTURE = 2};

struct texture_t
{
    std::string _name;
    size_t _width;
    size_t _height;
    size_t _channels;
    viewtype_t _type;
    size_t _id;
    std::shared_ptr<gl_texture_id> _tex;
    
    texture_t() : _tex(invalid_texture){}
};

class screenshot_handle_t
{
    static std::atomic<size_t> id_counter;
public:
    screenshot_task _task;
    std::string _texture;
    std::string _camera;
    size_t _prerendering;
    viewtype_t _type;
    std::atomic<screenshot_state> _state;
    std::mutex _mtx;
    std::condition_variable _cv;
    bool _flip = false;
    bool _ignore_nan;
    size_t _width;
    size_t _height;
    size_t _channels;
private:
    GLint _datatype;
public:
    std::vector<std::string> _vcam;
    std::shared_ptr<gl_texture_id> _textureId;
    void set_state(screenshot_state state);
    void wait_until(screenshot_state state);
    bool operator()() const;
private:
    std::atomic<void *> _data;
public:
    std::shared_ptr<gl_buffer_id> _bufferAddress;
    size_t _id;

    void set_datatype(GLint datatype);

    GLint get_datatype() const;

    template <typename T>
    T* get_data(){
        if (_datatype != gl_type<T>){throw std::runtime_error("Datatype doesn't match " + std::to_string(_datatype) + " " + std::to_string(gl_type<T>));}
        return reinterpret_cast<T*>(_data.load());
    }

    template <typename T>
    void set_data(T *ptr, size_t size)
    {
        delete_data();
        if (_datatype != gl_type<T>){throw std::runtime_error("Datatype doesn't match " + std::to_string(_datatype) + " " + std::to_string(gl_type<T>));}
        T *tmp = new T[size];
        std::copy(ptr, ptr + size, tmp);
        _data = tmp;
    }

    bool has_data() const;

    void delete_data();

    template <typename T>
    std::vector<T> copy_data(){T* d = get_data<T>(); return std::vector<T>(d, d + _width * _height * _channels);}

    screenshot_handle_t(
        std::string const & camera,
        viewtype_t type,
        size_t width,
        size_t height,
        size_t channels,
        size_t datatype,
        size_t prerendering,
        bool export_nan,
        bool flip,
        std::vector<std::string> const & vcam);
    screenshot_handle_t & operator=(const screenshot_handle_t&) = delete;
    screenshot_handle_t(const screenshot_handle_t&) = delete;
    screenshot_handle_t();
    screenshot_handle_t(
        std::string const & filename,
        size_t width,
        size_t height,
        std::string const & camera,
        viewtype_t type,
        bool export_nan,
        size_t prerendering,
        std::vector<std::string> const & vcam);
    size_t num_elements() const;
    size_t size() const;

    ~screenshot_handle_t();
};

std::ostream & operator <<(std::ostream & out, screenshot_handle_t const & task);

struct arrow_t
{
    float _x0, _y0, _x1, _y1;
};

struct object_t
{
    std::string _name;
    size_t _id;
    std::map<size_t, vec3f_t> _key_pos;
    std::map<size_t, rotation_t> _key_rot;
    QMatrix4x4 _transformation;
    bool _visible;
    bool _diffrot;
    bool _difftrans;
    bool _trajectory;

    object_t(std::string const & name_);
};

struct mesh_object_t: object_t
{
    std::map<std::string, QOpenGLTexture*> _textures;
    objl::Loader _loader;
    std::vector<std::shared_ptr<gl_buffer_id> > _vbo;
    std::vector<std::shared_ptr<gl_buffer_id> > _vbi;

    mesh_object_t(std::string const & name_, std::string const & objfile);
};

struct camera_t : object_t
{
    viewmode_t _viewmode;
    bool _wireframe;
    float _aperture;
    size_t _samples;
    camera_t(std::string const & name_) : object_t(name_), _viewmode(PERSPECTIVE), _wireframe(false), _aperture(0), _samples(5) {}
};


struct premap_t
{
    QMatrix4x4 _world_to_camera_cur;
    camera_t const *_cam;
    size_t _smoothing;
    int32_t _frame;
    bool _diffnormalize;
    bool _difffallback;
    bool _difftrans;
    bool _diffrot;
    bool _diffobj;
    int32_t _diffbackward;
    int32_t _diffforward;
    float _fov;
    size_t _resolution;
    coordinate_system_t _coordinate_system;
    rendered_framebuffer_t _framebuffer;

    bool operator ==(premap_t const & premap) const;
};

struct view_t
{
    std::string const & _camera;
    size_t _x, _y, _width, _height;
    viewtype_t _viewtype;
    std::vector<std::shared_ptr<premap_t> > _premaps;
};

struct named_image
{
    advanced_image_base_t *img;
};

struct framelist_t
{
    std::string _name;
    std::vector<size_t> _frames;

    framelist_t(std::string const & name_, std::vector<size_t> const & framelist_);
};

class destroy_functor
{
private:
    std::function<void()> _f;
public:
    destroy_functor(std::function<void()> f_) : _f(f_){}

    ~destroy_functor(){_f();}
};

struct scene_t
{
    std::vector<camera_t> _cameras;
    std::vector<mesh_object_t> _objects;
    std::vector<framelist_t> _framelists;
    std::vector<screenshot_handle_t *> _screenshot_handles;
    std::vector<texture_t> _textures;
    std::mutex _mtx;
    
    scene_t();
    
    size_t get_camera_index(std::string const & name);
    
    camera_t * get_camera(std::string const & name);
    object_t * get_object(std::string const & name);
    mesh_object_t * get_mesh(std::string const & name);
    object_t & get_object(size_t index);
    texture_t* get_texture(std::string const & name);

    size_t num_objects() const;
    
    void queue_handle(screenshot_handle_t & handle);
};



#endif
