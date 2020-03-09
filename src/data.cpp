#include "data.h"
#include "util.h"

mesh_object_t::mesh_object_t(std::string const & name_, std::string const & objfile) : object_t(name_), _vbo(0)
{
    clock_t current_time = clock();
    _loader.LoadFile(objfile.c_str());
    std::cout << "mesh loading time: " << float( clock() - current_time ) / CLOCKS_PER_SEC << std::endl;
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
    std::cout << "assign (" << this<< ")" << flag << std::endl;
    std::lock_guard<std::mutex> g(_mutex);
    _flags = flag;
    _cond_var.notify_all();        
}

void pending_task_t::wait_unset(PendingFlag flag)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond_var.wait(lock, [this, flag]() {
        std::cout << "check (" << this<< ")" << this->_flags << std::endl;
        return (~this->_flags) & flag; });
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

void exec_env::clean()
{
    _mtx.lock();
    auto write = _pending_tasks.begin();
    for (auto read = _pending_tasks.begin(); read != _pending_tasks.end(); ++read)
    {
        if (!(*read)->is_deletable())
        {
            *write = *read;
            ++write;
        }
    }
    std::for_each(write, _pending_tasks.end(), UTIL::delete_functor);
    _pending_tasks.erase(write, _pending_tasks.end());
    _mtx.unlock();
}

size_t scene_t::num_objects() const
{
    return _cameras.size() + _objects.size();
}
