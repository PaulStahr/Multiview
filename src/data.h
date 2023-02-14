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
#include "image_util.h"
#include "types.h"
#include "geometry.h"
#include "gl_util.h"
#include "enums.h"
#include "mesh.h"
#include "screenshot_handle.h"
#include "gl_resource_id.h"
#include "gl_texture.h"

struct rendered_framebuffer_t
{
    std::shared_ptr<gl_texture_id> _rendered;
    std::shared_ptr<gl_texture_id> _position;
    std::shared_ptr<gl_texture_id> _flow;
    std::shared_ptr<gl_texture_id> _index;

    std::shared_ptr<gl_texture_id> get(viewtype_t viewtype);

    std::shared_ptr<gl_texture_id> *begin() {return &_rendered;}
    std::shared_ptr<gl_texture_id> *end()   {return &_index + 1;}

    size_t size(){return 4;}
};

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

void removenan(object_transform_base_t *tr);

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
    object_t(object_t const & other) = default;
    object_t & operator=(object_t &&) = default;
    virtual ~object_t() = 0;
};

struct camera_t;

namespace DRAWTYPE
{
enum drawtype
{
    frameline, line, wireframe, solid, end
};
}

struct mesh_object_t: object_t
{
    std::map<std::string, QOpenGLTexture*> _textures;
    std::vector<objl::Mesh> _meshes;
    std::vector<std::shared_ptr<objl::Material> > _materials;
    std::vector<gl_buffer_id> _vbo;
    std::vector<gl_buffer_id> _vbi;
    std::set<camera_t*> _cameras;
    DRAWTYPE::drawtype _dt;

    mesh_object_t(mesh_object_t const & other);
    mesh_object_t& operator=(mesh_object_t &&);
    mesh_object_t(mesh_object_t && other);
    mesh_object_t(std::string const & name_);
    ~mesh_object_t();
};

struct camera_t : object_t
{
    viewmode_t _viewmode;
    DRAWTYPE::drawtype _dt;
    vec2f_t _aperture;
    size_t _samples;
    std::map<frameindex_t, float> _key_aperture;
    std::set<mesh_object_t*> _meshes;
    camera_t(std::string const & name_) : object_t(name_), _viewmode(PERSPECTIVE), _dt(DRAWTYPE::end), _aperture(0,0), _samples(5) {}
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
    framelist_t(std::string const & name_, std::vector<size_t> && framelist_);
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
    objl::Material _null_material;
    std::mutex _mtx;
    
    scene_t();
    
    size_t get_camera_index(std::string const & name);
    
    camera_t * get_camera(std::string const & name);
    camera_t & add_camera(camera_t && cam);
    camera_t & add_camera(std::string const & name);
    object_t * get_object(std::string const & name);
    mesh_object_t & add_mesh(mesh_object_t && mesh);
    mesh_object_t & add_mesh(mesh_object_t const & mesh);
    framelist_t *get_framelist(std::string const & name);
    framelist_t & add_framelist(framelist_t const & fr);
    framelist_t & add_framelist(std::string const & name, std::string const & filename, bool matlab, bool rangelist);
    mesh_object_t * get_mesh(std::string const & name);
    object_t & get_object(size_t index);
    texture_t* get_texture(std::string const & name);
    std::shared_ptr<object_transform_base_t> get_trajectory(std::string const & name);
    object_transform_base_t * get_trajectory_pt(std::string const & name);

    size_t num_objects() const;
};



#endif
