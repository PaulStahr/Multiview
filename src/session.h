#ifndef SESSION_H
#define SESSION_H

#include "image_util.h"
#include "data.h"
#include <cstdint>
#include <string>
#include <vector>
#include "qt_util.h"

enum SessionUpdateType{UPDATE_NONE = 0x0, UPDATE_ANIMATING = 0x1, UPDATE_REDRAW = 0x2, UPDATE_SESSION = 0x4, UPDATE_SCENE = 0x8, UPDATE_FRAME = 0x10};

inline SessionUpdateType   operator| (SessionUpdateType   a, SessionUpdateType b)   {return   static_cast<SessionUpdateType>(static_cast<int>(a) | static_cast<int>(b));}
inline SessionUpdateType   operator& (SessionUpdateType   a, SessionUpdateType b)   {return   static_cast<SessionUpdateType>(static_cast<int>(a) & static_cast<int>(b));}
inline SessionUpdateType & operator|=(SessionUpdateType & a, SessionUpdateType b)   {return a=static_cast<SessionUpdateType>(static_cast<int>(a) | static_cast<int>(b));}
inline SessionUpdateType & operator&=(SessionUpdateType & a, SessionUpdateType b)   {return a=static_cast<SessionUpdateType>(static_cast<int>(a) & static_cast<int>(b));}

struct session_t
{    
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
    bool            _depth_testing = true;
    float           _depth_scale = 1;
    size_t          _culling = 0;
    RedrawScedule   _animating = REDRAW_ALWAYS;
    bool            _realtime = false;
    size_t          _frames_per_step = 1;
    size_t          _frames_per_second = 60;
    int             _play = 1;
    int             _m_frame;
    depthbuffer_size_t _depthbuffer_size = DEPTHBUFFER_16_BIT;
    scene_t         _scene;
    std::vector<SessionUpdateType> _scene_updates;
    std::vector<std::function<void(SessionUpdateType)>* >_updateListener;
    std::string     _screenshot;
    bool            _exit_program = false;
    size_t          _rendered_frames;
    std::vector<wait_for_rendered_frame_t*> _wait_for_rendered_frame_handles;

    std::vector<named_image> _images;

    void wait_for_frame(wait_for_rendered_frame_t &);
    
    void scene_update(SessionUpdateType sup);

    template <typename T, T session_t::* ptrr, SessionUpdateType sut>
    void set(T value)
    {
        this->*ptrr = value;
        this->scene_update(sut);
    }
};

void assert_argument_count(size_t n, size_t m);

template <typename StringIter>
void read_transformations(QMatrix4x4 & matrix, StringIter begin, StringIter end)
{
    while(begin != end)
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
        else if (type == "mat4x3")//modify headscan transform  pos 0 -2 0 erot 180 0 1 0 erot -90 1 0 0 mat4x3   -187.7345e-006     2.9786e-003   -38.7835e-006    -1.0174e+000   728.9581e-006    83.6161e-006     2.8931e-003    -2.8489e+000     2.8883e-003   172.5015e-006  -732.7123e-006    -1.7257e+000
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
    }
}

void exec_impl(std::string input, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task);

void exec(std::string input, std::vector<std::string> const & vars, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task);


#endif
