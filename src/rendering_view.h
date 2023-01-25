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

#include <iosfwd>
#include <qt5/QtGui/QMouseEvent>
#include <qt5/QtGui/QGenericMatrix>
#include <qt5/QtGui/QMatrix4x4>
#include <GL/gl.h>
#include <GL/glext.h>
#include <algorithm>
#include <qt5/QtGui/QOpenGLTexture>
#include <chrono>
#include <qt5/QtGui/QOpenGLShaderProgram>
#include <qt5/QtGui/QOpenGLPaintDevice>
#include <memory>
#include <set>
#include <deque>
#include "mesh.h"
#include "session.h"
#include "geometry.h"
#include "shader.h"
#include "qt_util.h"
#include "openglwindow.h"
#include "statistics.h"

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

struct active_camera_t
{
    camera_t const * _cam;
    std::vector<QMatrix4x4> _world_to_cam_pre;
    std::vector<QMatrix4x4> _world_to_cam_cur;
    std::vector<QMatrix4x4> _world_to_cam_post;
    
    active_camera_t(camera_t const * cam_) : _cam(cam_){}
};

typedef std::chrono::time_point<std::chrono::high_resolution_clock> high_res_clock;

class RenderingWindow;

struct rendering_view_update_handler_t : session_updater_t
{
    RenderingWindow *_rw;

    rendering_view_update_handler_t(RenderingWindow *rw_) : _rw(rw_){}
    
    bool operator()(SessionUpdateType sut);
};

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
    bool poll_asynchronous_tasks() override;
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
    remapping_equirectangular_shader_t remapping_equirectangular_shader;
    std::unique_ptr<QOpenGLPaintDevice> qogpd = nullptr;
    std::vector<std::shared_ptr<premap_t> > _premaps;
    void session_update(SessionUpdateType sut);
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
    frameindex_t _last_rendered_frame;
    std::shared_ptr<session_updater_t> _update_handler;
    void render_to_texture(screenshot_handle_t & current, render_setting_t const & render_setting, bool blend, size_t loglevel, bool debug, remapping_shader_t & remapping_shader);
    std::shared_ptr<gl_texture_id> create_texture(size_t swidth, size_t sheight, viewtype_t vtype);
    std::shared_ptr<gl_texture_id> create_texture(size_t swidth, size_t sheight, size_t channels, GLuint type);
    std::mutex _delete_mtx;
    void delete_texture(GLuint);
    void delete_buffer(GLuint);
    void delete_framebuffer(GLuint);
    void delete_renderbuffer(GLuint);
    bool copy_screenshots();
    void clean();
    void dmaTextureCopy(screenshot_handle_t & current, bool debug);
    void load_meshes(mesh_object_t & mesh);
    void render_objects(
        std::set<mesh_object_t*> const & meshes,
        objl::Material & null_material,
        rendering_shader_t & shader,
        frameindex_t m_frame,
        frameindex_t denumerator,
        bool diffobj,
        int32_t diffbackward,
        int32_t diffforward,
        bool diffnormalize,
        bool difffallback,
        size_t smoothing,
        coordinate_system_t coordinate_system,
        QMatrix4x4 const & world_to_view,
        QMatrix4x4 const & world_to_camera_pre,
        QMatrix4x4 const & world_to_camera_cur,
        QMatrix4x4 const & world_to_camera_post,
        frame_stats_t & frame_stats,
        bool debug);
    std::shared_ptr<premap_t> render_premap(premap_t & premap, std::set<mesh_object_t*> const & meshes, objl::Material & null_material, frame_stats_t & frame_stats);
    std::function<void(GLuint)> _texture_deleter;
    std::function<void(GLuint)> _buffer_deleter;
    std::function<void(GLuint)> _renderbuffer_deleter;
    std::function<void(GLuint)> _framebuffer_deleter;
    std::atomic<bool> _scene_updated;
    std::unique_ptr<QOpenGLTexture> _texture_white;

    template <typename T>
    void gen_textures_shared        (size_t count, T output_iter){gen_resources_shared<gl_texture_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenTextures(size, data);}, _texture_deleter);}
    
    template <typename T>
    void gen_buffers_shared         (size_t count, T output_iter){gen_resources_shared<gl_buffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenBuffers(size, data);}, _buffer_deleter);}
    
    template <typename T>
    void gen_framebuffers_shared    (size_t count, T output_iter){gen_resources_shared<gl_framebuffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenFramebuffers(size, data);}, _framebuffer_deleter);}
    
    template <typename T>
    void gen_renderbuffers_shared   (size_t count, T output_iter){gen_resources_shared<gl_renderbuffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenRenderbuffers(size, data);}, _renderbuffer_deleter);}

    template <typename T>
    void gen_textures_unique        (size_t count, T output_iter){gen_resources_unique<gl_texture_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenTextures(size, data);}, _texture_deleter);}
    
    template <typename T>
    void gen_framebuffers_unique    (size_t count, T output_iter){gen_resources_unique<gl_framebuffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenFramebuffers(size, data);}, _framebuffer_deleter);}
    
    template <typename T>
    void gen_renderbuffers_unique   (size_t count, T output_iter){gen_resources_unique<gl_renderbuffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenRenderbuffers(size, data);}, _renderbuffer_deleter);}

    template <typename T>
    void gen_textures_direct        (size_t count, T output_iter){gen_resources_direct<gl_texture_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenTextures(size, data);}, _texture_deleter);}
    
    template <typename T>
    void gen_framebuffers_direct    (size_t count, T output_iter){gen_resources_direct<gl_framebuffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenFramebuffers(size, data);}, _framebuffer_deleter);}
    
    template <typename T>
    void gen_renderbuffers_direct   (size_t count, T output_iter){gen_resources_direct<gl_renderbuffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenRenderbuffers(size, data);}, _renderbuffer_deleter);}

    template <typename T>
    void gen_buffers_direct         (size_t count, T output_iter){gen_resources_direct<gl_buffer_id>(count, output_iter, [this](GLsizei size, GLuint* data){this->glGenBuffers(size, data);}, _buffer_deleter);}
};

#define BUFFER_OFFSET(i) ((void*)(i))

void load_textures(mesh_object_t & mesh);

void destroy(mesh_object_t & mesh);

//RenderingWindow *window;

std::ostream & print_gl_errors(std::ostream & out, std::string const & message, bool endl);

void transform_matrix(object_t const & obj, QMatrix4x4 & matrix, size_t mt_frame, size_t t_smooth, size_t mr_frame, size_t r_smooth);

void render_map(GLuint *renderedTexture, remapping_shader_t &);

//void render_to_screenshot(screenshot_handle_t & current, GLuint **cubemaps, size_t loglevel, scene_t & scene, remapping_spherical_shader_t & remapping_shader);

void render_to_pixel_buffer(screenshot_handle_t & current, render_setting_t const & render_setting, size_t loglevel, bool debug, remapping_shader_t & remapping_shader);

std::string getGlErrorString();

void copy_pixel_buffer_to_screenshot(screenshot_handle_t & current, bool debug);

void setShaderBoolean(QOpenGLShaderProgram & prog, GLuint attr, const char *name, bool value);


#endif
