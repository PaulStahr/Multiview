#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <memory>

#include "gl_util.h"
#include "enums.h"
#include "gl_resource_id.h"
#include "gl_texture.h"

#ifndef SCREENSHOT_HANDLE_T
#define SCREENSHOT_HANDLE_T

struct gl_command_t{
    virtual ~gl_command_t();
};

enum screenshot_state{
    screenshot_state_inited = 0,
    screenshot_state_queued = 1,
    screenshot_state_gl_queued = 2,
    screenshot_state_rendered_texture = 3,
    screenshot_state_rendered_buffer = 4,
    screenshot_state_copied = 5,
    screenshot_state_saved = 6,
    screenshot_state_error = 7};

enum screenshot_task{
    TAKE_SCREENSHOT = 0,
    SAVE_TEXTURE = 1,
    RENDER_TO_TEXTURE = 2};

class screenshot_handle_t : public gl_command_t
{
    static std::atomic<size_t> id_counter;
public:
    screenshot_task _task;
    std::string _name;
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
    GLsync _sync;
private:
    GLint _datatype;
public:
    std::vector<std::string> _vcam;
    texture_t _texture;
    void set_state(screenshot_state state);
    screenshot_state get_state();
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

    void set_data(void *ptr, GLint datatype, size_t num_elements);

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

#endif
