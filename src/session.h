#ifndef SESSION_H
#define SESSION_H

#include "image_util.h"
#include "data.h"
#include <cstdint>
#include <string>
#include <vector>
#include "qt_util.h"

enum SessionUpdateType{UPDATE_ANIMATING, UPDATE_REDRAW, UPDATE_SESSION, UPDATE_FRAME};

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
    std::vector<wait_for_rendered_frame*> _wait_for_rendered_frame_handles;

    std::vector<named_image> _images;

    void scene_update(SessionUpdateType sup)
    {
        _scene_updates.emplace_back(sup);
        for (auto & f : _updateListener)
        {
            f(sup);
        }
    }
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

void exec(std::string input, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task);


#endif
