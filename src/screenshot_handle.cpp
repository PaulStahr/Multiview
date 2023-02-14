#include "screenshot_handle.h"
#include <cstdint>
#include <ostream>

gl_command_t::~gl_command_t() noexcept{}

screenshot_handle_t::screenshot_handle_t() :
    _texture(),
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
            _texture(),
            _data(nullptr),
            _id(id_counter++)
            {}

size_t screenshot_handle_t::num_elements()  const{return _width * _height * _channels;}
size_t screenshot_handle_t::size()          const{return num_elements() * (get_datatype() == gl_type<float> ? 4 : 1);}
bool screenshot_handle_t::operator()()        const{return _state == screenshot_state_copied || _state == screenshot_state_error;}

void screenshot_handle_t::set_datatype(GLint datatype){if (has_data()){throw std::runtime_error("Screenshot already filled with data");} _datatype = datatype;}

GLint screenshot_handle_t::get_datatype() const {return _datatype;}

bool screenshot_handle_t::has_data() const{return _data;}

void screenshot_handle_t::set_data(void *ptr, GLint datatype, size_t num_elements)
{
    switch(datatype)
    {
        case gl_type<uint8_t>: set_data(static_cast<uint8_t*> (ptr), num_elements);break;
        case gl_type<int8_t>:  set_data(static_cast<int8_t*>  (ptr), num_elements);break;
        case gl_type<uint16_t>:set_data(static_cast<uint16_t*>(ptr), num_elements);break;
        case gl_type<int16_t>: set_data(static_cast<int16_t*> (ptr), num_elements);break;
        case gl_type<uint32_t>:set_data(static_cast<uint32_t*>(ptr), num_elements);break;
        case gl_type<int32_t>: set_data(static_cast<int32_t*> (ptr), num_elements);break;
        case gl_type<float>:   set_data(static_cast<float*>   (ptr), num_elements);break;
        default: throw std::runtime_error("Unsupported image-type " + std::to_string(datatype));
    }
}

void screenshot_handle_t::delete_data()
{
    void* ptr = _data;
    if (ptr)
    {
        switch(_datatype)
        {
            case gl_type<uint8_t>: delete[] static_cast<uint8_t*> (ptr);break;
            case gl_type<uint16_t>:delete[] static_cast<uint16_t*>(ptr);break;
            case gl_type<uint32_t>:delete[] static_cast<uint32_t*>(ptr);break;
            case gl_type<uint64_t>:delete[] static_cast<uint64_t*>(ptr);break;
            case gl_type<int8_t>  :delete[] static_cast<int8_t*>  (ptr);break;
            case gl_type<int16_t> :delete[] static_cast<int16_t*> (ptr);break;
            case gl_type<int32_t> :delete[] static_cast<int32_t*> (ptr);break;
            case gl_type<float>   :delete[] static_cast<float*>   (ptr);break;
            case gl_type<double>  :delete[] static_cast<double*>  (ptr);break;
            default: throw std::runtime_error("Unknown datatype " + std::to_string(_datatype));
        }
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

screenshot_state screenshot_handle_t::get_state()
{
    return _state;
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
        return out << handle->_camera << ' ' << handle->_prerendering << ' ' << handle->_type << ' ' << handle->_width << ' ' << handle->_height << ' ' << handle->_channels << ' ' << handle->get_datatype() << ' ' << handle->_ignore_nan << ' ' << handle->_state << ' ' << handle->_bufferAddress << ' ' << handle->_name << std::endl;
    }
    else
    {
        return out << task._camera << ' ' << task._prerendering << ' ' << task._type << std::endl;
    }
}

std::atomic<size_t> screenshot_handle_t::id_counter = 0;
