#ifndef SESSION_H
#define SESSION_H

#include "image_util.h"
#include "enums.h"
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "qt_util.h"
#include "data.h"
#include "types.h"

enum SessionUpdateType{UPDATE_NONE = 0x0, UPDATE_ANIMATING = 0x1, UPDATE_REDRAW = 0x2, UPDATE_SESSION = 0x4, UPDATE_SCENE = 0x8, UPDATE_FRAME = 0x10};

inline SessionUpdateType   operator| (SessionUpdateType   a, SessionUpdateType b)   {return   static_cast<SessionUpdateType>(static_cast<int>(a) | static_cast<int>(b));}
inline SessionUpdateType   operator& (SessionUpdateType   a, SessionUpdateType b)   {return   static_cast<SessionUpdateType>(static_cast<int>(a) & static_cast<int>(b));}
inline SessionUpdateType & operator|=(SessionUpdateType & a, SessionUpdateType b)   {return a=static_cast<SessionUpdateType>(static_cast<int>(a) | static_cast<int>(b));}
inline SessionUpdateType & operator&=(SessionUpdateType & a, SessionUpdateType b)   {return a=static_cast<SessionUpdateType>(static_cast<int>(a) & static_cast<int>(b));}

struct session_updater_t
{
    virtual bool operator()(SessionUpdateType sut) = 0;
};

namespace program_error
{
enum action
{
    ignore,
    skip,
    panic,
    action_end
};

enum error_type
{
    file           = 0x001, 
    key            = 0x002,
    animation      = 0x004,
    object         = 0x008,
    camera         = 0x010,
    mesh           = 0x020,
    texture        = 0x040,
    syntax         = 0x080,
    error_handling = 0x100,
    transformation = 0x200,
    error_type_end = 0x400
};

inline error_type   operator| (error_type   a, error_type b)   {return   static_cast<error_type>(static_cast<int>(a) | static_cast<int>(b));}
inline error_type   operator& (error_type   a, error_type b)   {return   static_cast<error_type>(static_cast<int>(a) & static_cast<int>(b));}
inline error_type & operator|=(error_type & a, error_type b)   {return a=static_cast<error_type>(static_cast<int>(a) | static_cast<int>(b));}
inline error_type & operator&=(error_type & a, error_type b)   {return a=static_cast<error_type>(static_cast<int>(a) & static_cast<int>(b));}

struct error_rule
{
    error_type  _type;
    action      _action;

    inline error_rule(error_type et, action a) : _type(et), _action(a){}
    inline error_rule()                        : _type(error_type_end), _action(action_end){}

    inline bool applies(error_type et)
    {
        return (et & ~_type) == 0;
    }
};

inline bool operator== (error_rule const & lhs, error_rule const & rhs){return lhs._type == rhs._type && lhs._action == rhs._action;}

struct program_exception : std::runtime_error
{
    error_type _type;
    program_exception( const std::string& what_arg, error_type et);
    program_exception( const char* what_arg, error_type et);
    program_exception( const program_exception& other ) = default;
};
}

class session_t
{
public:
    size_t          _loglevel = 1;
    int32_t         _diffforward = 1;
    int32_t         _diffbackward = -1;
    bool            _diffnormalize = true;
    bool            _difffallback = true;
    bool            _diffflipy = true;
    int             _perm = 0;
    std::string     _show_only;
    size_t          _smoothing = 0;
    float           _fov = 90;
    bool            _crop = true;
    size_t          _preresolution = 512;
    bool            _auto_update_gui = true;
    bool            _reload_shader = false;
    bool            _diffrot = true;
    size_t          _octree_batch_size = 100000;
    int32_t         _max_premaps = -1;
    bool            _difftrans = true;
    bool            _diffobjects = true;
    bool            _show_raytraced = true;
    bool            _show_flow = true;
    bool            _show_index = false;
    bool            _show_position = false;
    bool            _show_depth = true;
    bool            _show_curser = false;
    bool            _show_arrows = true;
    bool            _show_framelists = true;
    bool            _debug = false;
    coordinate_system_t _coordinate_system = COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS;
    bool            _show_rendered_visibility = true;
    bool            _show_visibility = false;
    bool            _depth_testing = true;
    float           _depth_scale = 1;
    size_t          _culling = 0;
    RedrawScedule   _animating = REDRAW_ALWAYS;
    bool            _realtime = false;
    size_t          _frames_per_step = 1;
    size_t          _frames_per_second = 60;
    int             _play = 1;
    bool            _indirect_rendering = true;
    bool            _show_debug_info = true;
    frameindex_t    _m_frame;
    frameindex_t    _motion_blur = 1;
    motion_blur_curve_t _motion_blur_curve = MOTION_BLUR_CONSTANT;
    std::map<frameindex_t, float> _motion_blur_custom_curve;
    frameindex_t    _framedenominator = 1;
    depthbuffer_size_t _depthbuffer_size = DEPTHBUFFER_16_BIT;
    scene_t         _scene;
    std::vector<SessionUpdateType> _scene_updates;
    std::string     _screenshot;
    bool            _exit_program = false;
    size_t          _rendered_frames;
    std::vector<wait_for_rendered_frame_t*> _wait_for_rendered_frame_handles;
    std::vector<program_error::error_rule> error_handling_rules;

    std::vector<named_image> _images;

    void wait_for_frame(wait_for_rendered_frame_t &);
    
    void screenshot(
        pending_task_t & pending_task,
        std::string const & output,
        viewtype_t viewtype,
        std::string & camera,
        int width,
        int height,
        std::vector<std::string> & vcam,
        bool ignore_nan);

    void scene_update(SessionUpdateType sup);

    template <typename T, T session_t::* ptrr, SessionUpdateType sut>
    void set(T value)
    {
        this->*ptrr = value;
        this->scene_update(sut);
    }

    void add_update_listener(std::shared_ptr<session_updater_t> & sut);
    program_error::action handle_error(program_error::error_type error);
private:
    std::vector<std::shared_ptr<session_updater_t> >_updateListener;
};

void assert_argument_count(size_t n, size_t m);

template <typename StringIter>
StringIter read_transformation(QMatrix4x4 & matrix, StringIter begin, StringIter end)
{
    std::string const & type = *begin;
    ++begin;
    if (type == "pos")
    {
        vec3f_t pos;
        for (float & elem : pos)
        {
            elem = std::stof(*begin);
            ++begin;
        }
        matrix.translate(pos[0],pos[1],pos[2]);
    }
    else if (type == "scale")
    {
        scale_t scale;
        assert_argument_count(3, std::distance(begin,end));
        for (float & elem : scale)
        {
            elem = std::stof(*begin);
            ++begin;
        }
        matrix.scale(scale[0],scale[1],scale[2]);
    }
    else if (type == "rot")
    {
        rotation_t rot;
        assert_argument_count(4, std::distance(begin,end));
        for (float & elem : rot)
        {
            elem = std::stof(*begin);
            ++begin;
        }
        matrix.rotate(to_qquat(rot));
    }
    else if (type == "erot")
    {
        assert_argument_count(4, std::distance(begin,end));
        float angle = std::stof(*(begin++));
        float x = std::stof(*(begin++));
        float y = std::stof(*(begin++));
        float z = std::stof(*(begin++));
        matrix.rotate(angle, x, y, z);
    }
    else if (type == "mat4x3")
    {
        assert_argument_count(12, std::distance(begin,end));
        float entry[16];
        for (size_t i = 0; i <3; ++i)
        {
            for(size_t j = 0; j < 4; ++j)
            {
                entry[i * 4 + j] = std::stof(*(begin++));
            }
        }
        std::fill(entry + 12, entry + 15, 0);
        entry[15] = 1;
        matrix *= QMatrix4x4(entry);
    }
    else
    {
        return --begin;
    }
    return begin;
}

template <typename StringIter>
void read_transformations(QMatrix4x4 & matrix, StringIter begin, StringIter end)
{
    while(begin != end)
    {
        StringIter tmp = read_transformation(matrix, begin, end);
        if (begin == tmp){throw std::runtime_error("Could not read transformation " + *begin);}
        begin = tmp;
    }
}

void exec_impl(std::string input, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task);

void exec(std::string input, std::vector<std::string> const & vars, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task);

void exec_stdout(std::string input, std::vector<std::string> const & vars, exec_env & env, session_t & session, pending_task_t & pending_task);
#endif
