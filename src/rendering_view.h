/*
Copyright (c) 2020 Paul Stahr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef RENDERING_VIEW
#define RENDERING_VIEW

#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <qt5/QtGui/QMouseEvent>
#include <qt5/QtGui/QGenericMatrix>
#include <qt5/QtGui/QMatrix4x4>
#include <GL/gl.h>
#include <GL/glext.h>
#include <algorithm>
#include <qt5/QtGui/QOpenGLTexture>
#include <chrono>
#include <qt5/QtGui/QOpenGLShaderProgram>
#include <qt5/QtGui/QPainter>
#include <qt5/QtGui/QOpenGLPaintDevice>
#include <memory>
#include "OBJ_Loader.h"
#include "session.h"
#include "io_util.h"
#include "geometry.h"
#include "transformation.h"
#include "image_io.h"
#include "shader.h"
#include "qt_util.h"
#include "openglwindow.h"

struct other_view_information_t
{
    std::shared_ptr<gl_texture_id> _position_texture;
    QMatrix4x4 _world_to_camera;
    other_view_information_t(QMatrix4x4 const & world_to_camera_, std::shared_ptr<gl_texture_id> position_texture_) : _position_texture(position_texture_), _world_to_camera(world_to_camera_){}
};

struct render_setting_t
{
    viewtype_t _viewtype;
    QMatrix4x4 _transform;
    QMatrix4x4 _color_transformation;
    std::shared_ptr<gl_texture_id> _position_texture;
    std::shared_ptr<gl_texture_id> _rendered_texture;
    bool _flipped;
    std::vector<other_view_information_t> _other_views;
};

struct premap_t
{
    QMatrix4x4 _world_to_camera_cur;
    camera_t const *_cam;
    size_t _smoothing;
    int32_t _frame;
    bool _diffnormalize;
    bool _difffallback;
    bool _difftrans;
    bool _diffrot;
    bool _diffobj;
    int32_t _diffbackward;
    int32_t _diffforward;
    float _fov;
    size_t _resolution;
    coordinate_system_t _coordinate_system;
    rendered_framebuffer_t _framebuffer;
    
    bool operator ==(premap_t const & premap) const;
};

struct active_camera_t
{
    camera_t const * _cam;
    QMatrix4x4 _world_to_cam;
    
    active_camera_t(camera_t const * cam_) : _cam(cam_){}
};

typedef std::chrono::time_point<std::chrono::high_resolution_clock> high_res_clock;

class RenderingWindow : public OpenGLWindow
{
private:
    std::shared_ptr<destroy_functor> _exit_handler;
    std::vector<GLuint> _to_remove_textures;
    std::vector<GLuint> _to_remove_buffers;
    std::vector<GLuint> _to_remove_framebuffers;
    std::vector<GLuint> _to_remove_renderbuffers;
public:
    RenderingWindow(std::shared_ptr<destroy_functor> exit_handler);
    void mouseMoveEvent(QMouseEvent *e) override;
    session_t session;
    void initialize() override;
    void render() override;
    bool destroyed = false;
    std::vector<arrow_t> arrows;

    high_res_clock last_rendertime;
    std::deque<high_res_clock> last_rendertimes;
    std::deque<high_res_clock> last_screenshottimes;
    spherical_approximation_shader_t approximation_shader;
    perspective_shader_t perspective_shader;
    cubemap_shader_t cubemap_shader;
    remapping_spherical_shader_t remapping_spherical_shader;
    remapping_identity_shader_t remapping_identity_shader;
    QOpenGLPaintDevice *qogpd = nullptr;
    std::vector<std::shared_ptr<premap_t> > _premaps;
    ~RenderingWindow();
private:
    std::thread::id _context_id;
    std::vector<view_t> views;
    std::vector<QPointF> marker;
    std::vector<active_camera_t> _active_cameras;
    QMatrix4x4 cubemap_camera_to_view[6];
    std::vector<vec2f_t> _curser_flow;
    std::vector<std::shared_ptr<screenshot_handle_t> > _arrow_handles;
    bool _updating;
    std::function<void(SessionUpdateType)> _update_handler;
    void render_to_texture(screenshot_handle_t & current, render_setting_t const & render_setting, size_t loglevel, bool debug, remapping_shader_t & remapping_shader);
    std::shared_ptr<gl_texture_id> create_texture(size_t swidth, size_t sheight, viewtype_t vtype);
    std::mutex _delete_mtx;
    void delete_texture(GLuint);
    void delete_buffer(GLuint);
    void delete_framebuffer(GLuint);
    void delete_renderbuffer(GLuint);
    void clean();
    void dmaTextureCopy(screenshot_handle_t & current, bool debug);
    void render_objects(
    std::vector<mesh_object_t> & meshes,
    rendering_shader_t & shader,
    int m_frame,
    bool diffobj,
    int32_t diffbackward,
    int32_t diffforward,
    bool diffnormalize,
    bool difffallback,
    size_t smoothing,
    QMatrix4x4 const & world_to_view,
    QMatrix4x4 const & world_to_camera_pre,
    QMatrix4x4 const & world_to_camera_cur,
    QMatrix4x4 const & world_to_camera_post,
    bool debug);
    std::shared_ptr<premap_t> render_premap(premap_t & premap, scene_t & scene);
    template <typename T>
    void gen_textures(size_t count, T output_iter)
    {
        while (count > 0)
        {
            std::array<GLuint, 32> tmp_id;
            size_t blk = std::min(count, tmp_id.size());
            glGenTextures(blk, &tmp_id[0]);
            for (size_t i = 0; i < blk; ++i)
            {
                GLint id = tmp_id[i];
                *output_iter = std::make_shared<gl_texture_id>(id, [=](GLuint ){delete_texture(id);});
                ++output_iter;
            }
            count -= blk;
        }
    }
    
    template <typename T>
    void gen_buffers(size_t count, T output_iter)
    {
        while (count > 0)
        {
            std::array<GLuint, 32> tmp_id;
            size_t blk = std::min(count, tmp_id.size());
            glGenBuffers(blk, &tmp_id[0]);
            for (size_t i = 0; i < blk; ++i)
            {
                GLint id = tmp_id[i];
                *output_iter = std::make_shared<gl_buffer_id>(id, [=](GLuint ){delete_buffer(id);});
                ++output_iter;
            }
            count -= blk;
        }
    }
    
    template <typename T>
    void gen_framebuffers(size_t count, T output_iter)
    {
        while (count > 0)
        {
            std::array<GLuint, 32> tmp_id;
            size_t blk = std::min(count, tmp_id.size());
            glGenFramebuffers(blk, &tmp_id[0]);
            for (size_t i = 0; i < blk; ++i)
            {
                GLint id = tmp_id[i];
                *output_iter = std::make_shared<gl_framebuffer_id>(id, [=](GLuint ){delete_framebuffer(id);});
                ++output_iter;
            }
            count -= blk;
        }
    }
    
    template <typename T>
    void gen_renderbuffers(size_t count, T output_iter)
    {
        while (count > 0)
        {
            std::array<GLuint, 32> tmp_id;
            size_t blk = std::min(count, tmp_id.size());
            glGenRenderbuffers(blk, &tmp_id[0]);
            for (size_t i = 0; i < blk; ++i)
            {
                GLint id = tmp_id[i];
                *output_iter = std::make_shared<gl_renderbuffer_id>(id, [=](GLuint ){delete_renderbuffer(id);});
                ++output_iter;
            }
            count -= blk;
        }
    }
};


void print_models(objl::Loader & Loader, std::ostream & file);

#define BUFFER_OFFSET(i) ((void*)(i))


void load_meshes(mesh_object_t & mesh);

void load_textures(mesh_object_t & mesh);

void destroy(mesh_object_t & mesh);

//RenderingWindow *window;

std::ostream & print_gl_errors(std::ostream & out, std::string const & message, bool endl);

void transform_matrix(object_t const & obj, QMatrix4x4 & matrix, size_t mt_frame, size_t t_smooth, size_t mr_frame, size_t r_smooth);

static const GLfloat g_quad_texture_coords[] = {
    1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    -1.0f, -1.0f,
};

static const GLfloat g_quad_texture_coords_flipped[] = {
    1.0f,  -1.0f,
    1.0f, 1.0f,
    -1.0f,  -1.0f,
    1.0f, 1.0f,
    -1.0f,  -1.0f,
    -1.0f, 1.0f,
};

static const GLfloat g_quad_vertex_buffer_data[] = {
    1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    -1.0f, -1.0f,
};

void render_map(GLuint *renderedTexture, remapping_shader_t &);

//void render_to_screenshot(screenshot_handle_t & current, GLuint **cubemaps, size_t loglevel, scene_t & scene, remapping_spherical_shader_t & remapping_shader);

void render_to_pixel_buffer(screenshot_handle_t & current, render_setting_t const & render_setting, size_t loglevel, bool debug, remapping_shader_t & remapping_shader);

std::string getGlErrorString();

void copy_pixel_buffer_to_screenshot(screenshot_handle_t & current, bool debug);

void setShaderBoolean(QOpenGLShaderProgram & prog, GLuint attr, const char *name, bool value);


#endif
