#include "data.h"
#include "util.h"

framelist_t::framelist_t(std::string const & name_, std::vector<size_t> const & framelist_) :_name(name_), _frames(framelist_){}

object_t::object_t(std::string const & name_):_name(name_), _id(0),  _transformation({1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}), _visible(true) {}

mesh_object_t::mesh_object_t(std::string const & name_, std::string const & objfile) : object_t(name_), _vbo(0),_flow(true)
{
    clock_t current_time = clock();
    _loader.LoadFile(objfile.c_str());
    std::cout << "mesh loading time: " << float( clock() - current_time ) / CLOCKS_PER_SEC << std::endl;
}

pending_task_t & exec_env::emitPendingTask()
{
    pending_task_t *pending = new pending_task_t(PENDING_ALL);
    //std::cout << "emit " << pending << std::endl;
    std::lock_guard<std::mutex> lockGuard(_mtx);
    _pending_tasks.emplace_back(pending);
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

void exec_env::join_impl(pending_task_t const * self, PendingFlag flag)
{
    for (size_t i = 0; i < _pending_tasks.size(); ++i)
    {
        if (_pending_tasks[i] != self)
        {
            _pending_tasks[i]->wait_unset(flag);
        }
    }
}

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
    this->_state = state;
    _cv.notify_all();
}

size_t scene_t::get_camera_index(std::string const & name)
{
    camera_t *tmp = get_camera(name);
    return tmp ? std::distance(_cameras.data(), tmp) : std::numeric_limits<size_t>::max();
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
    _flags |= flag;
    _cond_var.notify_all();
}

void pending_task_t::unset(PendingFlag flag)
{
    std::lock_guard<std::mutex> g(_mutex);
    _flags &= ~flag;
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
    std::unique_lock<std::mutex> lock(_mutex);
    _cond_var.wait(lock, [this, flag]() {
    //std::cout << "check (" << this<< "):" << this->_flags << " for " << flag << "->" << (this->_flags & flag) << std::endl;
    return !(this->_flags & flag); });
}

void pending_task_t::wait_set(PendingFlag flag)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond_var.wait(lock, [this, flag]() { return (~this->_flags) & flag; });
}

pending_task_t::pending_task_t(std::future<void> & future_, PendingFlag flags_): _future(std::move(future_)), _flags(flags_){}

pending_task_t::pending_task_t(PendingFlag flags_): _future(), _flags(flags_){}

pending_task_t::pending_task_t(pending_task_t&& other)
{
    _future = std::move(other._future);
    _flags = std::move(other._flags);
} 

pending_task_t& pending_task_t::operator=(pending_task_t&& other)     // <-- rvalue reference in input  
{  
    if (this == &other) 
        return *this;
    _future = std::move(other._future);
    _flags = std::move(other._flags);
    return *this;
}

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
    std::lock_guard<std::mutex> lockGuard(_mtx);
    clean_impl();
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
