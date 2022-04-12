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
#include <set>
#include <condition_variable>
#include "counting_semaphore.h"
#include "OBJ_Loader.h"
#include "image_util.h"
#include "types.h"
#include "geometry.h"
#include "gl_util.h"

enum RedrawScedule{REDRAW_ALWAYS, REDRAW_AUTOMATIC, REDRAW_MANUAL, REDRAW_END};

enum viewmode_t
{
    EQUIDISTANT, EQUIDISTANT_APPROX, PERSPECTIVE
};

enum viewtype_t
{
    VIEWTYPE_RENDERED = 0, VIEWTYPE_POSITION = 1, VIEWTYPE_DEPTH = 2, VIEWTYPE_FLOW = 3, VIEWTYPE_INDEX = 4, VIEWTYPE_VISIBILITY = 5, VIEWTYPE_END = 6
};

enum coordinate_system_t
{
    COORDINATE_SPHERICAL_APPROXIMATED, COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS, COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS, COORDINATE_EQUIRECTANGULAR, COORDINATE_END
};

class gl_resource_id
{
public:
    static std::atomic<size_t> count;
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
    
    void destroy();
    ~gl_resource_id();
};

class gl_buffer_id : public gl_resource_id{
public:
    gl_buffer_id();
    gl_buffer_id(GLuint id, std::function<void(GLuint)> remove);
};

class gl_texture_id : public gl_resource_id{
public:
    gl_texture_id();
    gl_texture_id(GLuint id, std::function<void(GLuint)> remove);
};

class gl_framebuffer_id : public gl_resource_id{
public:
    gl_framebuffer_id();
    gl_framebuffer_id(GLuint id, std::function<void(GLuint)> remove);
};

class gl_renderbuffer_id : public gl_resource_id{
public:
    gl_renderbuffer_id();
    gl_renderbuffer_id(GLuint id, std::function<void(GLuint)> remove);
};

struct rendered_framebuffer_t
{
    std::shared_ptr<gl_texture_id> _rendered;
    std::shared_ptr<gl_texture_id> _position;
    std::shared_ptr<gl_texture_id> _flow;
    std::shared_ptr<gl_texture_id> _index;

    std::shared_ptr<gl_texture_id> get(viewtype_t viewtype);

    std::shared_ptr<gl_texture_id> *begin() {return &_rendered;}
    std::shared_ptr<gl_texture_id> *end()   {return &_index;}

    size_t size(){return 4;}
};

enum depthbuffer_size_t{DEPTHBUFFER_16_BIT = 0, DEPTHBUFFER_24_BIT = 1, DEPTHBUFFER_32_BIT = 2, DEPTHBUFFER_END = 3};

enum motion_blur_curve_t{MOTION_BLUR_CONSTANT,MOTION_BLUR_LINEAR,MOTION_BLUR_QUADRATIC,MOTION_BLUR_CUBIC,MOTION_BLUR_CUSTOM,MOTION_BLUR_END};

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

    void set        (PendingFlag flag);
    void unset      (PendingFlag flag);
    void assign     (PendingFlag flag);
    void wait_unset (PendingFlag flag);
    void wait_set   (PendingFlag flag);
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
    GLuint _datatype;
    size_t _id;
    bool _defined;
    std::shared_ptr<gl_texture_id> _tex;
    
    texture_t() : _defined(false), _tex(nullptr){}
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

struct object_transform_base_t
{
    std::string _name;
    virtual ~object_transform_base_t(){}
};

template <typename T>
struct constant_transform_t : object_transform_base_t
{
    T _transform;
    
    constant_transform_t(){}
    
    constant_transform_t(T const & t):_transform(t){}
};

template <typename T>
struct dynamic_trajectory_t : object_transform_base_t
{
    std::map<frameindex_t, T> _key_transforms;
};

struct object_t
{
    std::string _name;
    size_t _id;
    QMatrix4x4 _transformation;
    std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool> > _transform_pipeline;
    bool _visible;
    bool _diffrot;
    bool _difftrans;
    bool _trajectory;

    object_t(std::string const & name_);
    object_t(object_t && other) = default;
    object_t & operator=(object_t &&) = default;
    virtual ~object_t() = 0;
};

struct camera_t;

struct mesh_object_t: object_t
{
    std::map<std::string, QOpenGLTexture*> _textures;
    objl::Loader _loader;
    std::vector<gl_buffer_id> _vbo;
    std::vector<gl_buffer_id> _vbi;
    std::set<camera_t*> _cameras;
    mesh_object_t(mesh_object_t & other) = delete;
    mesh_object_t& operator=(mesh_object_t &&);
    mesh_object_t(mesh_object_t && other);

    mesh_object_t(std::string const & name_);
    mesh_object_t(std::string const & name_, std::string const & objfile);
    ~mesh_object_t();
};

struct camera_t : object_t
{
    viewmode_t _viewmode;
    bool _wireframe;
    vec2f_t _aperture;
    size_t _samples;
    std::map<frameindex_t, float> _key_aperture;
    std::set<mesh_object_t*> _meshes;
    camera_t(std::string const & name_) : object_t(name_), _viewmode(PERSPECTIVE), _wireframe(false), _aperture(0,0), _samples(5) {}
    camera_t(camera_t & other) = delete;
    camera_t & operator=(camera_t &&);
    camera_t(camera_t && other);
    ~camera_t();
};

namespace SCENE
{
    void connect(camera_t & cam, mesh_object_t & mesh);

    void disconnect(camera_t & cam, mesh_object_t & mesh);
}

struct premap_t
{
    QMatrix4x4 _world_to_camera_pre;
    QMatrix4x4 _world_to_camera_cur;
    QMatrix4x4 _world_to_camera_post;
    camera_t const *_cam;
    frameindex_t _smoothing;
    frameindex_t _frame;
    frameindex_t _framedenominator;
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
    std::vector<std::shared_ptr<object_transform_base_t> > _trajectories;
    std::mutex _mtx;
    
    scene_t();
    
    size_t get_camera_index(std::string const & name);
    
    camera_t * get_camera(std::string const & name);
    camera_t & add_camera(camera_t && cam);
    object_t * get_object(std::string const & name);
    mesh_object_t & add_mesh(mesh_object_t && mesh);
    mesh_object_t * get_mesh(std::string const & name);
    object_t & get_object(size_t index);
    texture_t* get_texture(std::string const & name);
    std::shared_ptr<object_transform_base_t> get_trajectory(std::string const & name);

    size_t num_objects() const;
    
    void queue_handle(screenshot_handle_t & handle);
};



#endif
