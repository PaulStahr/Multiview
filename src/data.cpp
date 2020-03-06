#include "data.h"

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

size_t scene_t::num_objects() const
{
    return _cameras.size() + _objects.size();
}
