#include "data.h"
#include "util.h"

framelist_t::framelist_t(std::string const & name_, std::vector<size_t> const & framelist_) :_name(name_), _frames(framelist_){}

object_t::object_t(std::string const & name_):
    _name(name_),
    _id(0),
    _transformation({1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}),
    _visible(true),
    _diffrot(true),
    _difftrans(true),
    _trajectory(false) {}

mesh_object_t::mesh_object_t(std::string const & name_, std::string const & objfile) : object_t(name_), _vbo(0)
{
    _loader.LoadFile(objfile.c_str());
}

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

std::ostream & operator << (std::ostream & out, pending_task_t const & pending){return out << pending._flags;}

size_t screenshot_handle_t::num_elements()  const{return _width * _height * _channels;}
size_t screenshot_handle_t::size()          const{return num_elements() * (get_datatype() == gl_type<float> ? 4 : 1);}
bool screenshot_handle_t::operator()()        const{return _state == screenshot_state_copied || _state == screenshot_state_error;}

void screenshot_handle_t::set_datatype(GLint datatype){if (has_data()){throw std::runtime_error("Screenshot already filled with data");} _datatype = datatype;}

GLint screenshot_handle_t::get_datatype() const {return _datatype;}

bool screenshot_handle_t::has_data() const{return _data;}

void screenshot_handle_t::delete_data()
{
    void* ptr = _data;
    if (ptr != nullptr)
    {
        if      (_datatype == gl_type<float>)   {delete[] static_cast<float*>(ptr);}
        else if (_datatype == gl_type<uint8_t>) {delete[] static_cast<uint8_t*>(ptr);}
        else if (_datatype == gl_type<uint16_t>){delete[] static_cast<uint16_t*>(ptr);}
    }
    _data = nullptr;
}

screenshot_handle_t::~screenshot_handle_t(){
    delete_data();
}

void screenshot_handle_t::set_state(screenshot_state state) 
{
    std::lock_guard<std::mutex> g(_mtx);//TODO is this this necessary to prevent possible deadlock? (t0:check, t1:set state, t1:notify, t0:wait)
    this->_state = state;
    _cv.notify_all();
}

void screenshot_handle_t::wait_until(screenshot_state state)
{
    std::unique_lock<std::mutex> lck(_mtx);
    _cv.wait(lck,[this, state](){return this->_state >= state;});
}

std::ostream & operator <<(std::ostream & out, screenshot_handle_t const & task)
{
    screenshot_handle_t const *handle = dynamic_cast<screenshot_handle_t const *>(&task);
    if (handle)
    {
        return out << handle->_camera << ' ' << handle->_prerendering << ' ' << handle->_type << ' ' << handle->_width << ' ' << handle->_height << ' ' << handle->_channels << ' ' << handle->get_datatype() << ' ' << handle->_ignore_nan << ' ' << handle->_state << ' ' << handle->_bufferAddress << ' ' << handle->_textureId << std::endl;
    }
    else
    {
        return out << task._camera << ' ' << task._prerendering << ' ' << task._type << std::endl;
    }
}

texture_t* scene_t::get_texture(std::string const & name)
{
    auto res = std::find_if(_textures.begin(), _textures.end(), [name](texture_t & obj){return obj._name == name;});
    return res == _textures.end() ? nullptr : &*res;
}

std::atomic<size_t> gl_texture_id::count = 0;

gl_texture_id::gl_texture_id(GLuint id, std::function<void(GLuint)> remove) : _id(id), _remove(remove){++count;}

gl_texture_id::~gl_texture_id(){
    if (_remove && _id){
        _remove(_id);
    }else{
        std::cerr << "Can't delete texture " << _id << std::endl;
    }
    _id = 0;
    --count;
}

static unsigned long id_counter = 0;
screenshot_handle_t::screenshot_handle_t() :
    _textureId(invalid_texture),
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
            _textureId(invalid_texture),
            _data(nullptr),
            _id(id_counter++)
            {}

scene_t::scene_t()
{
    _cameras.reserve(1024);
    _objects.reserve(1024);
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

camera_t* scene_t::get_camera(std::string const & name)
{
    auto res = std::find_if(_cameras.begin(), _cameras.end(), [name](camera_t & obj){return obj._name == name;});
    return res == _cameras.end() ? nullptr : &*res;
}

mesh_object_t* scene_t::get_mesh(std::string const & name)
{
    auto res = std::find_if(_objects.begin(), _objects.end(), [name](mesh_object_t & obj){return obj._name == name;});
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
