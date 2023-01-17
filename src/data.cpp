#include "data.h"
#include "util.h"
#include "io_util.h"
#include <ostream>
#include <fstream>
#include <iostream>

framelist_t::framelist_t(std::string const & name_, std::vector<size_t> const & framelist_) :_name(name_), _frames(framelist_){}

framelist_t::framelist_t(std::string const & name_, std::vector<size_t> && framelist_) :_name(name_), _frames(framelist_){}

object_t::object_t(std::string const & name_):
    _name(name_),
    _id(0),
    _transformation({1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}),
    _visible(true),
    _diffrot(true),
    _difftrans(true),
    _trajectory(false) {}

mesh_object_t::mesh_object_t(std::string const & name) : object_t(name), _vbo(0){}

mesh_object_t::mesh_object_t(const mesh_object_t& other) :
    object_t::object_t(other),
    _textures(other._textures),
    _meshes(other._meshes),
    _materials(other._materials),
    _vbo(0),
    _vbi(0),
    _cameras(),
    _dt(other._dt)
{}

std::ostream & operator<<(std::ostream & out, wait_for_rendered_frame_t const & wait_obj)
{
    return out << wait_obj._frame << ' ' << wait_obj._value << std::endl;
}

pending_task_t & exec_env::emitPendingTask(std::string const & description)
{
    pending_task_t *pending = new pending_task_t(PENDING_ALL, description);
    emplace_back(*pending);
    return *pending;
}

void exec_env::emplace_back(pending_task_t &task)
{
    std::lock_guard<std::mutex> lockGuard(_mtx);
    _pending_tasks.emplace_back(&task);
}

void exec_env::join(pending_task_t const * self, PendingFlag flag)
{
    std::lock_guard<std::mutex> lockGuard(_mtx);
    join_impl(self, flag);
}

bool exec_env::code_active() const
{
    return std::find(_code_stack.begin(), _code_stack.end(), CODE_FALSE_IF) == _code_stack.end();
}

void exec_env::join_impl(pending_task_t const * self, PendingFlag flag)
{
    for (pending_task_t * p : _pending_tasks)
    {
        if (p != self)
        {
            p->wait_unset(flag);
        }
    }
}

mesh_object_t::mesh_object_t(mesh_object_t && other) : object_t(other._name){mesh_object_t::operator=(std::move(other));}

mesh_object_t& mesh_object_t::operator=(mesh_object_t && other)
{
    object_t::operator=(std::move(other));
    _textures   = std::move(other._textures);
    _meshes     = std::move(other._meshes);
    _materials  = std::move(other._materials);
    _vbo        = std::move(other._vbo);
    _vbi        = std::move(other._vbi);
    _cameras    = std::move(other._cameras);
    _dt         = std::move(other._dt);
    for (camera_t *cam : _cameras)
    {
        cam->_meshes.erase(&other);
        cam->_meshes.insert(this);
    }
    return *this;
}

mesh_object_t::~mesh_object_t()
{
    for (camera_t *cam : _cameras)
    {
        cam->_meshes.erase(this);
    }
}

camera_t::camera_t(camera_t && other) : object_t(other._name){camera_t::operator=(std::move(other));}

camera_t::~camera_t()
{
    for (mesh_object_t *mesh : _meshes)
    {
        mesh->_cameras.erase(this);
    }
}

camera_t& camera_t::operator=(camera_t && other)
{
    object_t::operator=(std::move(other));
    _viewmode       = std::move(other._viewmode);
    _dt             = std::move(other._dt);
    _aperture       = std::move(other._aperture);
    _samples        = std::move(other._samples);
    _key_aperture   = std::move(other._key_aperture);
    _meshes         = std::move(other._meshes);
    for (mesh_object_t *mesh : _meshes)
    {
        mesh->_cameras.erase(&other);
        mesh->_cameras.insert(this);
    }
    return *this;
}

object_t::~object_t() {}

std::ostream & operator << (std::ostream & out, pending_task_t const & pending){return out << pending._flags;}

texture_t* scene_t::get_texture(std::string const & name)
{
    auto res = std::find_if(_textures.begin(), _textures.end(), [& name](texture_t & obj){return obj._name == name;});
    return res == _textures.end() ? nullptr : &*res;
}

std::shared_ptr<object_transform_base_t> scene_t::get_trajectory(std::string const & name)
{
    auto res = std::find_if(_trajectories.begin(), _trajectories.end(), [& name](std::shared_ptr<object_transform_base_t> & obj){return obj->_name == name;});
    return res == _trajectories.end() ? nullptr : *res;
}

object_transform_base_t * scene_t::get_trajectory_pt(std::string const & name)
{
    return get_trajectory(name).get();
}

void removenan(object_transform_base_t *tr)
{
    dynamic_trajectory_t<vec3f_t>    *translation = dynamic_cast<dynamic_trajectory_t<vec3f_t>    *>(tr);
    dynamic_trajectory_t<rotation_t> *rotation    = dynamic_cast<dynamic_trajectory_t<rotation_t> *>(tr);
    if (translation)
    {
        ITER_UTIL::erase_if(translation->_key_transforms, [](std::pair<const long int, vec3f_t> const & trans){return contains_nan(trans.second);});
    }
    if (rotation)
    {
        ITER_UTIL::erase_if(rotation->_key_transforms, [](std::pair<const long int, rotation_t> const & rot){return contains_nan(rot.second);});
    }
}

screenshot_handle_t::screenshot_handle_t() :
    _textureId(nullptr),
    _data(nullptr),
    _id(id_counter++){}

screenshot_handle_t::screenshot_handle_t(
        std::string const & camera,
        viewtype_t type,
        size_t width,
        size_t height,
        size_t channels,
        size_t datatype,
        size_t prerendering,
        bool export_nan,
        bool flip,
        std::vector<std::string> const & vcam) :
            _camera(camera),
            _prerendering(prerendering),
            _type(type),
            _state(screenshot_state_inited),
            _flip(flip),
            _ignore_nan(export_nan),
            _width(width),
            _height(height),
            _channels(channels),
            _datatype(datatype),
            _vcam(vcam),
            _textureId(nullptr),
            _data(nullptr),
            _id(id_counter++)
            {}

scene_t::scene_t()
{
    _cameras.reserve(1024);
    _objects.reserve(1024);
    _null_material.Kd = {0.5,0.5,0.5};
}

size_t scene_t::get_camera_index(std::string const & name)
{
    camera_t *tmp = get_camera(name);
    return tmp ? std::distance(_cameras.data(), tmp) : std::numeric_limits<size_t>::max();
}

void scene_t::queue_handle(screenshot_handle_t & handle)
{
    assert(handle._state == screenshot_state_inited);
    std::lock_guard<std::mutex> lockGuard(_mtx);
    _screenshot_handles.push_back(&handle);
    handle.set_state(screenshot_state_queued);
}

framelist_t* scene_t::get_framelist(std::string const & name)
{
    auto res = std::find_if(_framelists.begin(), _framelists.end(), [& name](framelist_t & fr){return fr._name == name;});
    return res == _framelists.end() ? nullptr : &*res;
}

framelist_t& scene_t::add_framelist(framelist_t const & fr)
{
    _framelists.emplace_back(fr);
    return _framelists.back();
}

framelist_t& scene_t::add_framelist(std::string const & name, std::string const & filename, bool matlab, bool rangelist)
{
    std::ifstream framefile(filename);
    std::vector<size_t> framelist = rangelist ? IO_UTIL::parse_framelist(framefile) : IO_UTIL::parse_rangelist(framefile, matlab);
    framelist_t *result;
    if (matlab){std::for_each(framelist.begin(), framelist.end(), UTIL::pre_decrement);}
    {
        std::lock_guard<std::mutex> lck(this->_mtx);
        this->_framelists.emplace_back(name, std::move(framelist));
        result = &this->_framelists.back();
    }
    framefile.close();
    return *result;
}

camera_t* scene_t::get_camera(std::string const & name)
{
    auto res = std::find_if(_cameras.begin(), _cameras.end(), [& name](camera_t & obj){return obj._name == name;});
    return res == _cameras.end() ? nullptr : &*res;
}

namespace SCENE
{
    void connect(camera_t & cam, mesh_object_t & mesh)
    {
        mesh._cameras.insert(&cam);
        cam._meshes.insert(&mesh);
    }

    void disconnect(camera_t & cam, mesh_object_t & mesh)
    {
        mesh._cameras.erase(&cam);
        cam._meshes.erase(&mesh);
    }
}

camera_t & scene_t::add_camera(camera_t && cam)
{
    _cameras.push_back(std::move(cam));
    camera_t & inserted = _cameras.back();
    for (mesh_object_t & mesh : _objects)
    {
        SCENE::connect(inserted, mesh);
    }
    return inserted;
}

camera_t & scene_t::add_camera(std::string const &name)
{
    return add_camera(camera_t(name));
}

mesh_object_t & scene_t::add_mesh(mesh_object_t && mesh)
{
    std::lock_guard<std::mutex> lck(_mtx);
    _objects.push_back(std::move(mesh));
    mesh_object_t & inserted = _objects.back();
    for (camera_t & cam : _cameras)
    {
        SCENE::connect(cam, inserted);
    }
    return inserted;
}

mesh_object_t & scene_t::add_mesh(mesh_object_t const & mesh)
{
    std::lock_guard<std::mutex> lck(_mtx);
    _objects.emplace_back(mesh);
    mesh_object_t & inserted = _objects.back();
    for (camera_t & cam : _cameras)
    {
        SCENE::connect(cam, inserted);
    }
    return inserted;
}

mesh_object_t* scene_t::get_mesh(std::string const & name)
{
    auto res = std::find_if(_objects.begin(), _objects.end(), [& name](mesh_object_t & obj){return obj._name == name;});
    return res == _objects.end() ? nullptr : &*res;
}

object_t* scene_t::get_object(std::string const & name)
{
    mesh_object_t *m = get_mesh(name);
    if (m){return m;}
    return get_camera(name);
}

object_t & scene_t::get_object(size_t index)
{
    return index < _cameras.size() ? static_cast<object_t &>(_cameras[index]) : static_cast<object_t &>(_objects[index - _cameras.size()]);
}

void pending_task_t::set(PendingFlag flag)
{
    std::lock_guard<std::mutex> g(_mutex);
    _flags = _flags | flag;
    _cond_var.notify_all();
}

void pending_task_t::unset(PendingFlag flag)
{
    std::lock_guard<std::mutex> g(_mutex);
    _flags = _flags & ~flag;
    _cond_var.notify_all();
}

void pending_task_t::assign(PendingFlag flag)
{
    //std::cout << "assign (" << this<< "):" << flag << std::endl;
    std::lock_guard<std::mutex> g(_mutex);
    _flags = flag;
    _cond_var.notify_all();
}

void pending_task_t::wait_unset(PendingFlag flag)
{
    std::unique_lock<std::mutex> lck(_mutex);
    if (!(this->_flags & flag)){return;}
    _cond_var.wait(lck, [this, flag]() {return !(this->_flags & flag); });
}

void pending_task_t::wait_set(PendingFlag flag)
{
    std::unique_lock<std::mutex> lck(_mutex);
    _cond_var.wait(lck, [this, flag]() { return (~this->_flags) & flag; });
}

pending_task_t::pending_task_t(std::future<void> & future_, PendingFlag flags_, std::string const & description_): _future(std::move(future_)), _flags(flags_), _description(description_){}

pending_task_t::pending_task_t(PendingFlag flags_, std::string const & description_): _future(), _flags(flags_), _description(description_){}

bool pending_task_t::is_deletable() const
{
    return (!_future.valid() || _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) && _flags == 0; 
}

exec_env::exec_env(std::string const & script_dir_) :_script_dir(script_dir_), num_threads_(64){}

void exec_env::clean_impl()
{
    _pending_tasks.erase(std::remove_if(_pending_tasks.begin(), _pending_tasks.end(), [](pending_task_t *& task){
        bool deletable = task->is_deletable();
        if (deletable){delete task;}
        return deletable;
    }), _pending_tasks.end());
}

void exec_env::clean()
{
    if (_mtx.try_lock())
    {
        clean_impl();
        _mtx.unlock();
    }
}

exec_env::~exec_env()
{
    std::lock_guard<std::mutex> lockGuard(_mtx);
    join_impl(nullptr, PENDING_ALL);
    clean_impl();
}

size_t scene_t::num_objects() const{return _cameras.size() + _objects.size();}

const std::array<std::shared_ptr<gl_texture_id> rendered_framebuffer_t::*, 6> viewtype_texture_ids = {{
    &rendered_framebuffer_t::_rendered,
    &rendered_framebuffer_t::_position,
    &rendered_framebuffer_t::_position,
    &rendered_framebuffer_t::_flow,
    &rendered_framebuffer_t::_index,
    &rendered_framebuffer_t::_rendered}};

std::shared_ptr<gl_texture_id> rendered_framebuffer_t::get(viewtype_t viewtype)
{
    if (viewtype < viewtype_texture_ids.size())
    {
        return this->*viewtype_texture_ids[viewtype];
    }
    throw std::runtime_error("unsupported type " + std::to_string(viewtype));
}
