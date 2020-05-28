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

enum RedrawScedule{REDRAW_ALWAYS, REDRAW_AUTOMATIC, REDRAW_MANUAL};

struct session_t
{    
    size_t          _loglevel = 1;
    int32_t         _diffforward = 1;
    int32_t         _diffbackward = -1;
    int             _perm = 0;
    std::string     _show_only;
    size_t          _smoothing = 0;
    float           _fov = 90;
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
    bool            _debug = false;
    bool            _approximated = false;
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
    viewmode_t      _viewmode = EQUIDISTANT;
    depthbuffer_size_t _depthbuffer_size = DEPTHBUFFER_16_BIT;
    scene_t         _scene;
    std::vector<SessionUpdateType> _scene_updates;
    std::vector<std::function<void(SessionUpdateType)> >_updateListener;
    std::string _screenshot;
    bool        _exit_program = false;
    size_t _rendered_frames;
    std::vector<wait_for_rendered_frame_t*> _wait_for_rendered_frame_handles;

    std::vector<named_image> _images;

    void wait_for_frame(wait_for_rendered_frame_t &);
    
    void scene_update(SessionUpdateType sup);
};


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
            for (float & elem : rot)
            {
                elem = std::stof(*begin);
                ++begin;
            }
            matrix.rotate(to_qquat(rot));
        }
        else if (type == "erot")
        {
            float angle = std::stof(*(begin++));
            float x = std::stof(*(begin++));
            float y = std::stof(*(begin++));
            float z = std::stof(*(begin++));
            matrix.rotate(angle, x, y, z);
        }
    }
}

void exec_impl(std::string input, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task);

void exec(std::string input, std::vector<std::string> const & vars, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task);


#endif
