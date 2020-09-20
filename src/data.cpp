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
    clock_t current_time = clock();
    _loader.LoadFile(objfile.c_str());
    std::cout << "mesh loading time: " << float( clock() - current_time ) / CLOCKS_PER_SEC << std::endl;
}

std::ostream & operator<<(std::ostream & out, wait_for_rendered_frame_t const & wait_obj)
{
    return out << wait_obj._frame << ' ' << wait_obj._value << std::endl;
}

pending_task_t & exec_env::emitPendingTask(std::string const & description)
{
    pending_task_t *pending = new pending_task_t(PENDING_ALL, description);
    //std::cout << "emit " << pending << std::endl;
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

std::ostream & operator << (std::ostream & out, pending_task_t const & pending)
{
    return out << pending._flags;
}

screenshot_handle_t::screenshot_handle_t() : _data(nullptr){}

size_t screenshot_handle_t::num_elements() const
{
    return _width * _height * _channels;
}

size_t screenshot_handle_t::size() const
{
    return num_elements() * (_datatype == GL_FLOAT ? 4 : 1);
}

bool screenshot_handle_t::operator()() const
{
    return _state == screenshot_state_copied || _state == screenshot_state_error;
}

void screenshot_handle_t::set_state(screenshot_state state) 
{
    std::lock_guard<std::mutex> g(_mtx);//TODO is this this necessary to prevent possible deadlock? (t0:check, t1:set state, t1:notify, t0:wait)
    this->_state = state;
    _cv.notify_all();
}

void* screenshot_handle_t::get_data()
{
    return _data.load();
}

void screenshot_handle_t::wait_until(screenshot_state state)
{
    std::unique_lock<std::mutex> lck(_mtx);
    _cv.wait(lck,[this, state](){return this->_state >= state;});
}

std::ostream & operator <<(std::ostream & out, screenshot_handle_t const & handle)
{
    return out << handle._camera << ' ' << handle._prerendering << ' ' << handle._type << ' ' << handle._width << ' ' << handle._height << ' ' << handle._channels << ' ' << handle._datatype << ' ' << handle._ignore_nan << ' ' << handle._data << ' ' << handle._state << ' ' << handle._bufferAddress << ' ' << handle._textureId << std::endl;
}


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
    std::lock_guard<std::mutex> lockGuard(_mtx);
    _screenshot_handles.push_back(&handle);
    handle.set_state(screenshot_state_queued);
}

camera_t * scene_t::get_camera(std::string const & name)
{
    for (camera_t & obj : _cameras)
    {
        if (obj._name == name)
        {
            return &obj;
        }
    }
    return nullptr;
}

object_t * scene_t::get_object(std::string const & name)
{
    for (object_t & obj : _objects)
    {
        if (obj._name == name)
        {
            return &obj;
        }
    }

    return get_camera(name);
}

object_t & scene_t::get_object(size_t index)
{
    if(index < _cameras.size())
    {
        return _cameras[index];
    }
    else
    {
        return _objects[index - _cameras.size()];
    }
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
    std::cout << "Unset " << _flags << ' ' << flag << std::endl;
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
    std::cout << "Has to wait for " << this->_flags << ' ' << flag << ' ' << this->_description << std::endl;
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

void exec_env::clean_impl()
{
    auto write = _pending_tasks.begin();
    for (auto read = _pending_tasks.begin(); read != _pending_tasks.end(); ++read)
    {
        if ((*read)->is_deletable())
        {
            delete *read;
        }
        else
        {
            *write = *read;
            ++write;
        }
    }
    std::cout << "clean (" << this << ") "<< _pending_tasks.size() << "->" << std::distance(_pending_tasks.begin(), write) << std::endl;
    //std::for_each(write, _pending_tasks.end(), UTIL::delete_functor);
    _pending_tasks.erase(write, _pending_tasks.end());
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

size_t scene_t::num_objects() const
{
    return _cameras.size() + _objects.size();
}
