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
#include <QtGui/QMouseEvent>
#include <QtGui/QMatrix4x4>
#include <GL/gl.h>
#include <GL/glext.h>
#include <algorithm>
#include <QtGui/QOpenGLTexture>
#include <chrono>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QPainter>
#include <QtGui/QOpenGLPaintDevice>
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
    QMatrix4x4 _camera_transformation;
    other_view_information_t(QMatrix4x4 const & camera_transformation_, std::shared_ptr<gl_texture_id> position_texture_) : _position_texture(position_texture_), _camera_transformation(camera_transformation_){}
};

struct render_setting_t
{
    viewtype_t _viewtype;
    QMatrix4x4 _camera_transformation;
    QMatrix4x4 _position_transformation;
    QMatrix4x4 _color_transformation;
    std::shared_ptr<gl_texture_id> _selfPositionTexture;
    std::shared_ptr<gl_texture_id> _rendered_texture;
    bool _flipped;
    std::vector<other_view_information_t> _other_views;
};

typedef std::chrono::time_point<std::chrono::high_resolution_clock> high_res_clock;

class TriangleWindow : public OpenGLWindow
{
private:
    std::vector<GLuint> _to_remove_textures;
public:
    TriangleWindow();

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
    remapping_spherical_shader_t remapping_spherical_shader;
    remapping_identity_shader_t remapping_identity_shader;
    QOpenGLPaintDevice *qogpd = nullptr;
    ~TriangleWindow();
private:
    std::vector<view_t> views;
    std::vector<QPointF> marker;
    std::vector<QMatrix4x4> world_to_camera;
    QMatrix4x4 cubemap_camera_to_view[6];
    std::vector<vec2f_t> _curser_flow;
    std::vector<screenshot_handle_t*> _arrow_handles;
    std::vector<camera_t const *> _active_cameras;
    bool _updating;
    std::function<void(SessionUpdateType)> _update_handler;
    void render_to_texture(screenshot_handle_t & current, render_setting_t const & render_setting, size_t loglevel, bool debug, remapping_shader_t & remapping_shader);
    std::shared_ptr<gl_texture_id> create_texture(size_t swidth, size_t sheight, viewtype_t vtype);
    void delete_texture(GLuint);
    void clean();
    template <typename T>
    void gen_textures(size_t count, T output_iter)
    {
        while (count > 0)
        {
            std::array<GLuint, 32> tmp_id;
            size_t blk = std::min(count, tmp_id.size());
            glGenTextures(blk, reinterpret_cast<GLuint*>(&tmp_id));
            for (size_t i = 0; i < blk; ++i)
            {
                *output_iter = std::make_shared<gl_texture_id>(tmp_id[i], std::bind(&TriangleWindow::delete_texture, this, std::placeholders::_1));
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

//TriangleWindow *window;

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
