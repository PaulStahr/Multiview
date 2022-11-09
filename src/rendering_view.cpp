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

#include "rendering_view.h"
#include "qt_gl_util.h"
#include "gl_util.h"
#include "shader.h"
#include "statistics.h"
#include "transformation.h"
#include "image_io.h"
#include "mesh.h"
#include <qt5/QtGui/QImage>
#include <iostream>
#include <cstdint>
#include <qt5/QtGui/QPainter>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

/*
//Start of egltest
#include <EGL/egl.h>

  static const EGLint configAttribs[] = {
          EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
          EGL_BLUE_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_RED_SIZE, 8,
          EGL_DEPTH_SIZE, 8,
          EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
          EGL_NONE
  };    

  static const int pbufferWidth = 9;
  static const int pbufferHeight = 9;

  static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
  };

int egltest()
{
  // 1. Initialize EGL
  EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  EGLint major, minor;

  eglInitialize(eglDpy, &major, &minor);

  // 2. Select an appropriate configuration
  EGLint numConfigs;
  EGLConfig eglCfg;

  eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

  // 3. Create a surface
  EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, 
                                               pbufferAttribs);

  // 4. Bind the API
  eglBindAPI(EGL_OPENGL_API);

  // 5. Create a context and make it current
  EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, 
                                       NULL);

  eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

  // from now on use your OpenGL context

  // 6. Terminate EGL when finished
  eglTerminate(eglDpy);
  return 0;
}
//end of egltest
*/

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

void RenderingWindow::load_meshes(mesh_object_t & mesh)
{
    if (mesh._vbo.empty() && !mesh._meshes.empty())
    {
        std::cout << "load meshes " << mesh._name << std::endl;
        mesh._vbo.resize(mesh._meshes.size());
        gen_buffers_direct(mesh._vbo.size(), mesh._vbo.begin());
        mesh._vbi.resize(mesh._meshes.size());
        gen_buffers_direct(mesh._vbi.size(), mesh._vbi.begin());
        for (size_t i = 0; i < mesh._vbo.size(); ++i)
        {
            objl::Mesh const & curMesh = mesh._meshes[i];
            glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo[i]);
            glBufferData(GL_ARRAY_BUFFER, curMesh._vertices->_sizeofa * curMesh._vertices->size(), curMesh._vertices->data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._vbi[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_t) * curMesh.Indices.size(), curMesh.Indices.data(), GL_STATIC_DRAW);
        }
    }
}

void load_textures(mesh_object_t & mesh)
{
    for (size_t i = 0; i < mesh._meshes.size(); ++i)
    {
        std::shared_ptr<objl::Material> material = mesh._meshes[i]._material;
        if (material)
        {
            std::string const & map_Ka = material->map_Ka;
            if (map_Ka != "" && mesh._textures.find(map_Ka) == mesh._textures.end())
            {
                QImage img;
                if (!img.load(map_Ka.c_str()))
                {
                    std::cout << "error, can't load image " << map_Ka.c_str() << std::endl;
                }
                std::cout << img.width() << ' ' << img.height() << std::endl;
                mesh._textures[map_Ka] = new QOpenGLTexture(img.mirrored());
            }
            std::string const & map_Kd = material->map_Kd;
            if (map_Kd != "" && mesh._textures.find(map_Kd) == mesh._textures.end())
            {
                QImage img;
                if (!img.load(map_Kd.c_str()))
                {
                    std::cout << "error, can't load image " << map_Kd.c_str() << std::endl;
                }
                std::cout << img.width() << ' ' << img.height() << std::endl;
                mesh._textures[map_Kd] = new QOpenGLTexture(img.mirrored());
            }
        }
    }
}

void destroy(mesh_object_t & mesh)
{
    mesh._vbo.clear();
    mesh._vbi.clear();
    for (auto iter = mesh._textures.begin(); iter != mesh._textures.end(); ++iter)
    {
        iter->second -> destroy();
        delete iter->second;
        iter->second = nullptr;
    }
}

void debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
                  const GLchar *message, const void* /* userParam*/)
{
    char const* severity_str = "";
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:        severity_str = "high";          break;
        case GL_DEBUG_SEVERITY_MEDIUM:      severity_str = "medium";        break;
        case GL_DEBUG_SEVERITY_LOW:         severity_str = "low";           break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:severity_str = "notification";  break;
    }
    std::cerr << "GlDebug: " << severity_str << ' ' << source << ' ' << type << ' ' << id << ' ' << message << std::endl;
}

std::ostream & print_gl_errors(std::ostream & out, std::string const & message, bool endl)
{
    GLenum error = glGetError();
    if (error == 0){return out;}
    out << message << error;// << gluErrorString(error);
    while (true)
    {
        error = glGetError();
        if (!error)
        {
            return endl ? (out << std::endl) : out;
        }
        out << ' '<< error;// << gluErrorString(error);
    }
    return out;
}

std::string getGlErrorString()
{
    std::string result;
    while (true)
    {
        GLenum error = glGetError();
        if (error == 0)
        {
            return result;
        }
        result += ' ';
        result += error;
    }
}

void setupTexture(GLenum target, gl_texture_id &texture, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type)
{
    glBindTexture(target, texture);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (target == GL_TEXTURE_CUBE_MAP)
    {
        for (uint8_t f = 0; f < 6; ++f)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0,internalFormat, width, height, 0,format, type, 0);
        }
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0,internalFormat, width, height, 0,format, type, 0);
    }
}

void render_map(std::shared_ptr<gl_texture_id> cubemap, remapping_shader_t & remapping_shader, bool flipped)
{
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(dynamic_cast<remapping_spherical_shader_t*>(&remapping_shader)  || dynamic_cast<remapping_equirectangular_shader_t*>(&remapping_shader)?  GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, *cubemap);
    glUniform1i(remapping_shader._texAttr, 0);
    
    glVertexAttribPointer(remapping_shader._posAttr, 2, GL_FLOAT, GL_FALSE, 0, g_quad_vertex_buffer_data);
    glVertexAttribPointer(remapping_shader._corAttr, 2, GL_FLOAT, GL_FALSE, 0, flipped ? g_quad_texture_coords_flipped : g_quad_texture_coords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void activate_render_settings(remapping_shader_t & remapping_shader, render_setting_t const & render_setting)
{
    remapping_shader._program->setUniformValue(remapping_shader._transformUniform, render_setting._transform);
    glUniform(remapping_shader._transformColorUniform, render_setting._color_transformation);
    glUniform(remapping_shader._viewtypeUniform, static_cast<GLint>(render_setting._viewtype));
}

void render_view(remapping_shader_t & remapping_shader, render_setting_t const & render_setting)
{
    activate_render_settings(remapping_shader, render_setting);
    glActiveTexture(GL_TEXTURE1);
    GLenum target = dynamic_cast<remapping_spherical_shader_t*>(&remapping_shader) || dynamic_cast<remapping_equirectangular_shader_t*>(&remapping_shader)? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    glBindTexture(target, *render_setting._position_texture);
    glUniform1i(remapping_shader._positionMap, 1);
    for (size_t i = 0; i < render_setting._other_views.size(); ++i)
    {
        other_view_information_t const & other = render_setting._other_views[i];
        glUniform(remapping_shader._transformCam[i], get_affine(other._world_to_camera));
        glActiveTexture(GL_TEXTURE2 + i);
        glBindTexture(target, *other._position_texture);
        glUniform1i(remapping_shader._positionMaps[i], 2 + i);
    }
    glUniform(remapping_shader._numOverlays, static_cast<GLint>(render_setting._other_views.size()));
    render_map(render_setting._rendered_texture, remapping_shader, render_setting._flipped);
}

struct texture_format_t
{
    size_t _channels;
    GLuint _internal_format;
    GLuint _format;
    GLuint _type;
};

const static texture_format_t texture_formats[] = {
    {1, GL_R32F,    GL_RED,     GL_FLOAT},
    {2, GL_R32F,    GL_RG,      GL_FLOAT},
    {3, GL_RGBA32F, GL_RGBA,    GL_FLOAT},
    {4, GL_RGBA32F, GL_RGBA,    GL_FLOAT},
    {3, GL_RGBA,    GL_RGBA,    GL_UNSIGNED_BYTE},
    {4, GL_RGBA,    GL_RGBA,    GL_UNSIGNED_BYTE},
    {3, GL_RGBA,    GL_RGBA16,  GL_UNSIGNED_SHORT},
    {4, GL_RGBA,    GL_RGBA,    GL_UNSIGNED_BYTE},
    {0, GL_INVALID_ENUM, GL_INVALID_ENUM, GL_INVALID_ENUM}};

const texture_format_t & get_texture_format(size_t channels, size_t type)
{
    return *std::find_if(texture_formats, texture_formats + 6, [channels, type](auto elem){return elem._channels == channels && elem._type == type;});
}

struct viewtype_tuple_t
{
    viewtype_t _viewtype;
    size_t _channels;
    GLuint _internal_format;
    GLuint _format;
    GLuint _type;
};

const std::array<viewtype_tuple_t, 7> viewtypes = {{
        {VIEWTYPE_RENDERED, 3, GL_RGBA,     GL_RGBA, GL_UNSIGNED_BYTE},
        {VIEWTYPE_POSITION, 3, GL_RGBA32F,  GL_RGBA, GL_FLOAT},
        {VIEWTYPE_DEPTH,    1, GL_R32F,     GL_RED,  GL_FLOAT},
        {VIEWTYPE_FLOW,     3, GL_RGBA32F,  GL_RGBA, GL_FLOAT},
        {VIEWTYPE_INDEX,    1, GL_R32F,     GL_RED,  GL_FLOAT},
        {VIEWTYPE_VISIBILITY,3,GL_RGBA,     GL_RGBA, GL_UNSIGNED_BYTE},
        {VIEWTYPE_END,      0, 0, 0, 0}}};

const viewtype_tuple_t & get_viewtype_tuple_t(viewtype_t viewtype){return viewtypes[viewtype];}

const static GLenum channel_colors[] ={GL_NONE, GL_RED, GL_RG, GL_RGB, GL_RGBA};

GLenum get_format(size_t channels)
{
    if (channels > 4){throw std::runtime_error("Unsupported number of channels " + std::to_string(channels));}
    return channel_colors[channels];
}

void RenderingWindow::dmaTextureCopy(screenshot_handle_t & current, bool debug)
{
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    assert(current._state == screenshot_state_rendered_texture);
    size_t textureType = GL_TEXTURE_2D;
    if (current._prerendering != std::numeric_limits<size_t>::max())
    {
        textureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X + current._prerendering;
        glBindTexture(GL_TEXTURE_CUBE_MAP, *current._textureId);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *current._textureId);
    }
    if (current._channels == 0) {current._channels = get_viewtype_tuple_t(current._type)._channels;}
    std::shared_ptr<gl_buffer_id> pbo_userImage;
    gen_buffers_shared(1, &pbo_userImage);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, *pbo_userImage);
    glBufferData(GL_PIXEL_PACK_BUFFER, current.size(), 0, GL_STREAM_READ);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}

    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    glGetTexImage(textureType, 0, get_format(current._channels), current.get_datatype(), 0);
    current._bufferAddress = std::move(pbo_userImage);
    current.set_state(screenshot_state_rendered_buffer);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
}

void RenderingWindow::delete_texture(GLuint tex){
    if (std::this_thread::get_id() == _context_id){
        glDeleteTextures(1, &tex);
    }else{
        std::lock_guard<std::mutex> lockGuard(_delete_mtx);
        _to_remove_textures.emplace_back(tex);
    }
}

void RenderingWindow::delete_buffer(GLuint buf){
    if (std::this_thread::get_id() == _context_id){
        glDeleteBuffers(1, &buf);
    }else{
        std::lock_guard<std::mutex> lockGuard(_delete_mtx);
        _to_remove_buffers.emplace_back(buf);
    }
}

void RenderingWindow::delete_framebuffer(GLuint buf){
    if (std::this_thread::get_id() == _context_id){
        glDeleteFramebuffers(1, &buf);
    }else{
        std::lock_guard<std::mutex> lockGuard(_delete_mtx);
        _to_remove_framebuffers.emplace_back(buf);
    }
}

void RenderingWindow::delete_renderbuffer(GLuint buf){
    if (std::this_thread::get_id() == _context_id){
        glDeleteRenderbuffers(1, &buf);
    }else{
        std::lock_guard<std::mutex> lockGuard(_delete_mtx);
        _to_remove_renderbuffers.emplace_back(buf);
    }
}

std::shared_ptr<gl_texture_id> RenderingWindow::create_texture(size_t swidth, size_t sheight, viewtype_t vtype)
{
    std::shared_ptr<gl_texture_id> screenshotTexture;
    gen_textures_shared(1, &screenshotTexture);
    viewtype_tuple_t viewtype = get_viewtype_tuple_t(vtype);
    setupTexture(GL_TEXTURE_2D, *screenshotTexture.get(), viewtype._internal_format, swidth, sheight, viewtype._format, viewtype._type);
    return screenshotTexture;
}

std::shared_ptr<gl_texture_id> RenderingWindow::create_texture(size_t swidth, size_t sheight, size_t channels, GLuint type)
{
    std::shared_ptr<gl_texture_id> screenshotTexture;
    gen_textures_shared(1, &screenshotTexture);
    texture_format_t tf = get_texture_format(channels, type);
    if (tf._type == GL_INVALID_ENUM){throw std::runtime_error("Couldn't find matching texture-format " + std::to_string(channels) + " " + std::to_string(type));}
    setupTexture(GL_TEXTURE_2D, *screenshotTexture.get(), tf._internal_format, swidth, sheight, tf._format, type);
    return screenshotTexture;
}

void RenderingWindow::render_to_texture(
    screenshot_handle_t & current,
    render_setting_t const & render_setting,
    bool blend,
    size_t loglevel, bool debug, remapping_shader_t & remapping_shader)
{
    if (current._task != TAKE_SCREENSHOT && current._task != RENDER_TO_TEXTURE)
    {
        throw std::runtime_error("Invalid task " + std::to_string(current._task));
    }
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    assert(current._state == screenshot_state_queued);
    if (loglevel > 2){std::cout << "take screenshot " << current._camera << std::endl;}
    activate_render_settings(remapping_shader, render_setting);
    gl_framebuffer_id screenshotFramebuffer;
    gen_framebuffers_direct(1, &screenshotFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screenshotFramebuffer);
    if (current._task == TAKE_SCREENSHOT){current._textureId = create_texture(current._width, current._height, current._type);}
    gl_renderbuffer_id depthrenderbuffer;
    gen_renderbuffers_direct(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, current._width, current._height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *current._textureId, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    if (debug){
        print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
        GLuint framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Error, no framebuffer(" + std::to_string(__LINE__) + "):" + std::to_string(framebufferStatus));
    }
    glViewport(0,0,current._width, current._height);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);
    if (blend)
    {
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}    
    render_view(remapping_shader, render_setting);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    if (blend)
    {
        glDisable(GL_BLEND);
    }
    current.set_state(screenshot_state_rendered_texture);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    return;
}

bool rendering_view_update_handler_t::operator()(SessionUpdateType sut){
    if (_rw){
        _rw->session_update(sut);
    }
    return _rw;
}

void RenderingWindow::session_update(SessionUpdateType sut){
    setAnimating(session._animating == REDRAW_ALWAYS || (session._animating == REDRAW_AUTOMATIC && session._play != 0));
    if (_updating){return;}
    if (sut == UPDATE_SCENE)
    {
        _scene_updated = true;
    }
    switch(session._animating)
    {
        case REDRAW_ALWAYS:     break;
        case REDRAW_AUTOMATIC:  if (sut & (UPDATE_REDRAW | UPDATE_SESSION | UPDATE_FRAME | UPDATE_SCENE))   {renderLater();}break;
        case REDRAW_MANUAL:     if (sut &  UPDATE_REDRAW)                                                   {renderLater();}break;
        default: break;
    }
    return;
}

RenderingWindow::RenderingWindow(std::shared_ptr<destroy_functor> exit_handler_) : _exit_handler(exit_handler_)
{
    QObject::connect(this, SIGNAL(renderLaterSignal()), this, SLOT(renderLater()));
    //QObject::connect(this, SIGNAL(renderNowSignal()), this, SLOT(renderNow()));
    session._m_frame = 100000;
    _updating = false;
    _scene_updated = true;
    _update_handler = std::shared_ptr<session_updater_t>(new rendering_view_update_handler_t(this));
    session.add_update_listener(_update_handler);
    _texture_deleter     = [this](GLuint id){this->delete_texture(id);};
    _buffer_deleter      = [this](GLuint id){this->delete_buffer(id);};
    _renderbuffer_deleter= [this](GLuint id){this->delete_renderbuffer(id);};
    _framebuffer_deleter = [this](GLuint id){this->delete_framebuffer(id);};
}

RenderingWindow::~RenderingWindow()
{
    if (!destroyed){std::cerr << "Trying to delete rendering view, without clearing gl-resources" << std::endl;}
    session._exit_program = true;
    static_cast<rendering_view_update_handler_t*>(_update_handler.get())->_rw = nullptr;
}

void RenderingWindow::initialize()
{
    _context_id = std::this_thread::get_id();
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessage, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    perspective_shader.init(*this);
    cubemap_shader.init(*this);
    remapping_spherical_shader.init(*this);
    approximation_shader.init(*this);
    remapping_identity_shader.init(*this);
    remapping_equirectangular_shader.init(*this);
    _premaps.clear();
    GLint maxColorAttachememts = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachememts);
    std::cout << "max attachments:" << maxColorAttachememts << std::endl;
    //GLubyte const* msg = glGetString(GL_EXTENSIONS);
    //std::cout << "extensions:" << msg << std::endl;//TODO
    image_io_init();
    QImage img(1,1,QImage::Format_RGB32);
    img.setPixelColor(0, 0, QColor(255,255,255));
    _texture_white = std::make_unique<QOpenGLTexture>(img);
    QMatrix4x4 tmp;
    tmp.setToIdentity();
    tmp.perspective(90.0f, 1.0f/1.0f, 0.1f, 1000.0f);
    std::fill(&cubemap_camera_to_view[0], &cubemap_camera_to_view[6], tmp);

    /*
     * 0     1    2  3    4    5
     * right left up down back front
     * +x    -x   +y -y   +z   -z
     */
    cubemap_camera_to_view[0].rotate(-90, 0, 1, 0);
    cubemap_camera_to_view[1].rotate(90, 0, 1, 0);
    cubemap_camera_to_view[2].rotate(180, 0, 0, 1);
    cubemap_camera_to_view[2].rotate(90, 1, 0, 0);
    cubemap_camera_to_view[3].rotate(180, 0, 0, 1);
    cubemap_camera_to_view[3].rotate(-90, 1, 0, 0);
    cubemap_camera_to_view[4].rotate(180, 0, 1, 0);
    for (auto & mat : cubemap_camera_to_view)
    {
        mat.rotate(180,0,0,1);
    }
}

std::ostream & operator << (std::ostream & out, arrow_t const & arrow)
{
    return out << '('<< arrow._x0 << ' ' << arrow._y0 << ' ' << arrow._x1 << ' ' << arrow._y1 << ')';
}

void copy_pixel_buffer_to_screenshot(screenshot_handle_t & current, bool debug)
{
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    assert(current._state == screenshot_state_rendered_buffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, *current._bufferAddress);
    void *ptr = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    if (!ptr){throw std::runtime_error("map buffer returned null" + getGlErrorString());}
    GLint datatype = current.get_datatype();
    size_t num_elements = current.num_elements();
    switch(datatype)
    {
        case gl_type<uint8_t>: current.set_data(static_cast<uint8_t*> (ptr), num_elements);break;
        case gl_type<int8_t>:  current.set_data(static_cast<int8_t*>  (ptr), num_elements);break;
        case gl_type<uint16_t>:current.set_data(static_cast<uint16_t*>(ptr), num_elements);break;
        case gl_type<int16_t>: current.set_data(static_cast<int16_t*> (ptr), num_elements);break;
        case gl_type<uint32_t>:current.set_data(static_cast<uint32_t*>(ptr), num_elements);break;
        case gl_type<int32_t>: current.set_data(static_cast<int32_t*> (ptr), num_elements);break;
        case gl_type<float>:   current.set_data(static_cast<float*>   (ptr), num_elements);break;
        default: throw std::runtime_error("Unsupported image-type " + std::to_string(datatype));
    }
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    current._bufferAddress = nullptr;
    current.set_state(screenshot_state_copied);
}

size_t draw_elements_cubemap_singlepass(
    objl::octree_t const & octree,
    QMatrix4x4 const &,
    size_t,
    std::function<void(size_t, size_t)> draw)
{
    draw(octree._begin, octree._end);
    return octree._end - octree._begin;
}

size_t draw_elements_spherical_approximation(
    objl::octree_t const & octree,
    QMatrix4x4 const & object_to_view_cur,
    size_t min_batch_size,
    std::function<void(size_t, size_t)> draw)
{
    for (size_t i = 0; i < 8; ++i)
    {
        QVector4D vertex;
        for (size_t j = 0; j < 3; ++j)
        {
            vertex[j] = i & (1u << j) ? octree._min[j] : octree._max[j];
        }
        vertex[3] = 1;
        vertex = object_to_view_cur * vertex;
        if (vertex[2] <= 0)
        {
            if (octree._end - octree._begin < min_batch_size)
            {
                draw(octree._begin, octree._end);
                return octree._end - octree._begin;
            }
            size_t rendered = octree._cut_end - octree._cut_begin;
            if (octree._lhs)
            {
                rendered += draw_elements_spherical_approximation(*octree._lhs, object_to_view_cur, min_batch_size, draw);
            }
            draw(octree._cut_begin, octree._cut_end);
            if (octree._rhs)
            {
                rendered += draw_elements_spherical_approximation(*octree._rhs, object_to_view_cur, min_batch_size, draw);
            }
            return rendered;
        }
    }
    return 0;
}

size_t draw_elements_cubemap_multipass(
    objl::octree_t const & octree,
    QMatrix4x4 const & object_to_view_cur,
    size_t min_batch_size,
    std::function<void(size_t, size_t)> draw)
{
    std::array<bool, 5> side;
    std::fill(side.begin(), side.end(), false);
    for (size_t i = 0; i < 8; ++i)
    {
        QVector4D vertex;
        for (size_t j = 0; j < 3; ++j)
        {
            vertex[j] = i & (1 << j) ? octree._min[j] : octree._max[j];
        }
        vertex[3] = 1;
        vertex = object_to_view_cur * vertex;
        float dist = vertex[3];
        side[0] |= vertex[0] <= dist;
        side[1] |= vertex[0] >= -dist;
        side[2] |= vertex[1] <= dist;
        side[3] |= vertex[1] >= -dist;
        side[4] |= dist >= 0;
        if (side[0] && side[1] && side[2] && side[3] && side[4])
        {
            if (octree._end - octree._begin < min_batch_size)
            {
                draw(octree._begin, octree._end);
                return octree._end - octree._begin;
            }
            size_t rendered = octree._cut_end - octree._cut_begin;
            if (octree._lhs)
            {
                rendered += draw_elements_cubemap_multipass(*octree._lhs, object_to_view_cur, min_batch_size, draw);
            }
            draw(octree._cut_begin, octree._cut_end);
            if (octree._rhs)
            {
                rendered += draw_elements_cubemap_multipass(*octree._rhs, object_to_view_cur, min_batch_size, draw);
            }
            return rendered;
        }
    }
    return 0;
}

void RenderingWindow::render_objects(
    std::set<mesh_object_t*> const & meshes,
    objl::Material & null_material,
    rendering_shader_t & shader,
    frameindex_t m_frame,
    frameindex_t framedenominator,
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
    bool debug)
{
    size_t numAttributes = 3;
    QMatrix4x4 const *current_world_to_camera_pre  = &world_to_camera_pre;
    QMatrix4x4 const *current_world_to_camera_post = &world_to_camera_post;
    if (difffallback)
    {
        if (contains_nan(*current_world_to_camera_pre))//TODO go as far as possible
        {
            diffbackward = 0;
            current_world_to_camera_pre = &world_to_camera_cur;
        }
        if (contains_nan(*current_world_to_camera_post))
        {
            diffforward = 0;
            current_world_to_camera_post = &world_to_camera_cur;
        }
    }
    for (size_t j = 0; j < numAttributes;++j){glEnableVertexAttribArray(j);}
    for (mesh_object_t * m : meshes)
    {

        mesh_object_t & mesh = *m;
        if (!mesh._visible){continue;}
        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        load_meshes(mesh);
        std::array<QMatrix4x4, 3> object_to_world;
        bool difftranscurrent = diffobj && mesh._difftrans;
        bool diffrotcurrent   = diffobj && mesh._diffrot;
        std::array<frameindex_t, 3> rotframes   = {m_frame + (diffrotcurrent   ? diffbackward : 0), m_frame, m_frame + (diffrotcurrent   ? diffforward  : 0)};
        std::array<frameindex_t, 3> transframes = {m_frame + (difftranscurrent ? diffbackward : 0), m_frame, m_frame + (difftranscurrent ? diffforward  : 0)};
        for (auto iter = mesh._transform_pipeline.rbegin(); iter != mesh._transform_pipeline.rend(); ++iter)
        {
            transform_matrices<3>(
                iter->first.get(),
                iter->second,
                object_to_world,
                rotframes,
                transframes,
                framedenominator,
                smoothing,
                smoothing);
        }
        for (QMatrix4x4 & m: object_to_world)
        {
            m *= mesh._transformation;
        }
        if (contains_nan(object_to_world[1])){continue;}
        glUniform(shader._objidUniform, static_cast<GLint>(mesh._id));
        QMatrix4x4 flowMatrix = *current_world_to_camera_pre * object_to_world[0] - *current_world_to_camera_post * object_to_world[2];
        if (diffnormalize){flowMatrix *= 1. / (diffforward - diffbackward);}
        QMatrix4x4 object_to_view_cur = world_to_view * object_to_world[1];
        QMatrix4x4 object_to_camera = world_to_camera_cur * object_to_world[1];

        auto & meshes = mesh._meshes;
        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            objl::Mesh const & curMesh = meshes[i];
            QMatrix4x4 mesh_transform;
            mesh_transform.setToIdentity();
            QT_UTIL::translate(mesh_transform, curMesh._offset);
            QT_UTIL::scale(mesh_transform, curMesh._scale);
            glUniform(shader._flowMatrixUniform, get_affine(flowMatrix * mesh_transform));
            glUniform(shader._curMatrixUniform, get_affine(object_to_camera * mesh_transform));
            shader._program->setUniformValue(shader._matrixUniform, object_to_view_cur * mesh_transform);
            shader._program->setUniformValue(shader._objMatrixUniform, object_to_world[1] * mesh_transform);

            objl::Material & material = curMesh._material ? *curMesh._material : null_material;
            glUniform3f(shader._colAmbientUniform, material.Ka[0],material.Ka[1],material.Ka[2]);
            glUniform3f(shader._colDiffuseUniform, material.Kd[0],material.Kd[1],material.Kd[2]);
            glUniform3f(shader._colSpecularUniform, material.Ks[0],material.Ks[1],material.Ks[2]);
            load_textures(mesh);
            QOpenGLTexture *tex = mesh._textures[material.map_Kd];
            glActiveTexture(GL_TEXTURE0);
            (tex ? tex : _texture_white.get()) -> bind();
            glUniform1i(shader._texKd, 0);                
            objl::VertexArrayCommon const & vertices = *curMesh._vertices;
            if ((curMesh.Indices.empty() && mesh._dt != DRAWTYPE::line && mesh._dt != DRAWTYPE::frameline) || vertices.empty()){continue;}
            glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo[i]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._vbi[i]);
            size_t vertex_size = vertices._sizeofa;
            glVertexAttribPointer(shader._posAttr,      3, get_gl_type(vertices._typeofp), GL_TRUE, vertex_size, BUFFER_OFFSET(vertices._offsetp));
            glVertexAttribPointer(shader._corAttr,      2, get_gl_type(vertices._typeoft), GL_TRUE, vertex_size, BUFFER_OFFSET(vertices._offsett));
            glVertexAttribPointer(shader._normalAttr,   3, get_gl_type(vertices._typeofn), GL_TRUE, vertex_size, BUFFER_OFFSET(vertices._offsetn));
            if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
            std::pair<size_t,size_t> current_range(0,0);
            if (mesh._dt == DRAWTYPE::line || mesh._dt == DRAWTYPE::frameline)
            {
                glLineWidth(2);
                if (mesh._dt == DRAWTYPE::frameline)
                {
                    glDrawArrays(GL_LINE_STRIP, std::max((frameindex_t)0, m_frame - 100), 200);
                }
                else
                {
                    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
                }
            }
            else
            {
                if (session._octree_batch_size)
                {
                    auto draw_func = [&current_range, this](size_t begin, size_t end){
                        if (current_range.second == begin)
                        {
                            current_range.second = end;
                        }
                        else
                        {
                            if (current_range.first != current_range.second)
                            {
                                glDrawElements( GL_TRIANGLES, (current_range.second - current_range.first) * 3, gl_type<triangle_t::value_type>, (GLvoid*)(current_range.first * sizeof(triangle_t)));
                            }
                            current_range.first = begin;
                            current_range.second = end;
                        }
                    };
                    switch (coordinate_system)
                    {
                        case COORDINATE_EQUIRECTANGULAR:
                        case COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS:    frame_stats._rendered_faces += draw_elements_cubemap_multipass      (curMesh.octree, object_to_view_cur, session._octree_batch_size, draw_func);break;
                        case COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS:   frame_stats._rendered_faces += draw_elements_cubemap_singlepass     (curMesh.octree, object_to_view_cur, session._octree_batch_size, draw_func);break;
                        case COORDINATE_SPHERICAL_APPROXIMATED:         frame_stats._rendered_faces += draw_elements_spherical_approximation(curMesh.octree, object_to_view_cur, session._octree_batch_size, draw_func);break;
                        case COORDINATE_END: throw std::runtime_error("Invalid coordinate system");

                    }
                    draw_func(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max());
                }
                else
                {
                    glDrawElements( GL_TRIANGLES, curMesh.Indices.size() * 3, gl_type<triangle_t::value_type>, nullptr);
                    frame_stats._rendered_faces += curMesh.Indices.size();
                }
            }
            frame_stats._active_faces += curMesh.Indices.size();
            if (tex!= nullptr){glBindTexture(GL_TEXTURE_2D, 0);}
            if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        }
        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    for (size_t j = numAttributes; j --> 0;){glDisableVertexAttribArray(j);}
}

static std::array<GLuint,3> gl_depthbuffer_enum = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32};

GLint depth_component(depthbuffer_size_t depthbuffer_size){return gl_depthbuffer_enum[depthbuffer_size];}

void setup_framebuffer(GLuint target, size_t resolution, session_t const & session, rendered_framebuffer_t const & framebuffer, gl_texture_id & depth)
{
    GLuint textures[] = {*framebuffer._rendered, *framebuffer._flow, *framebuffer._position, *framebuffer._index};
    GLenum drawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    if (target != GL_TEXTURE_CUBE_MAP)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[i], target, textures[i], 0);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  target, depth, 0 );
    }
    else
    {
        for (size_t i = 0; i < 4; ++i)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, drawBuffers[i], textures[i], 0);
        }
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  depth, 0 );        
    }
    glDrawBuffers(4, drawBuffers);
    if (session._debug){
        GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Error, no framebuffer(" + std::to_string(__LINE__) + "):" + std::to_string(frameBufferStatus));
    }
    glViewport(0,0,resolution,resolution);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    set_activated(GL_DEPTH_TEST, session._depth_testing);
}

void RenderingWindow::clean()
{
    std::lock_guard<std::mutex> lockGuard(_delete_mtx);
    glDeleteFramebuffers(_to_remove_framebuffers.size(), _to_remove_framebuffers.data());
    _to_remove_framebuffers.clear();
    glDeleteRenderbuffers(_to_remove_renderbuffers.size(), _to_remove_renderbuffers.data());
    _to_remove_renderbuffers.clear();
    glDeleteTextures(_to_remove_textures.size(), _to_remove_textures.data());
    _to_remove_textures.clear();
    glDeleteBuffers(_to_remove_buffers.size(), _to_remove_buffers.data());
    _to_remove_buffers.clear();
}

struct camera_name_comparator_t
{
    std::string const & _name;
    camera_name_comparator_t(std::string const & name_) : _name(name_){}

    bool operator ()(object_t const & cam){return cam._name == _name;}

    bool operator ()(active_camera_t const & cam){return cam._cam->_name == _name;}
};

template <typename T>
struct pointer_comparator_t
{
    T const & _obj;
    pointer_comparator_t(T & obj_) : _obj(obj_){}

    bool operator ()(std::shared_ptr<T> const & elem){return *elem == _obj;}

    bool operator ()(std::unique_ptr<T> const & elem){return *elem == _obj;}

    bool operator ()(T const * elem){return *elem == _obj;}
};

std::shared_ptr<premap_t> RenderingWindow::render_premap(
    premap_t & premap,
    std::set<mesh_object_t*> const & objects,
    objl::Material & null_material,
    frame_stats_t & frame_stats)
{
    {
        std::thread::id id = std::this_thread::get_id();
        if (id != _context_id)
        {
            std::cerr << "Info: rendering id changed " << _context_id << " --> " << id << std::endl;
            _context_id = id;
        }
    }
    {
        auto result = std::find_if(_premaps.begin(), _premaps.end(), pointer_comparator_t(premap));
        if (result != _premaps.end()){return *result;}
    }
    gen_textures_shared(4, premap._framebuffer.begin());
    camera_t const &cam = *premap._cam;
    QMatrix4x4 const & world_to_camera_pre = premap._world_to_camera_pre;
    QMatrix4x4 const & world_to_camera_cur = premap._world_to_camera_cur;
    QMatrix4x4 const & world_to_camera_post= premap._world_to_camera_post;

    if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    GLuint target = premap._coordinate_system == COORDINATE_SPHERICAL_APPROXIMATED ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;
    rendered_framebuffer_t & framebuffer = premap._framebuffer;
    setupTexture(target, *framebuffer._rendered.get(),GL_RGBA,    premap._resolution, premap._resolution, GL_BGRA,        GL_UNSIGNED_BYTE);
    setupTexture(target, *framebuffer._flow.get(),    GL_RGB16F,  premap._resolution, premap._resolution, GL_BGR,         GL_FLOAT);
    setupTexture(target, *framebuffer._position.get(),GL_R32F,    premap._resolution, premap._resolution, GL_RED,         GL_FLOAT);
    setupTexture(target, *framebuffer._index.get(),   GL_R32UI,   premap._resolution, premap._resolution, GL_RED_INTEGER, GL_UNSIGNED_INT);
    gl_texture_id depth;
    gen_textures_direct(1, &depth);
    if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    setupTexture(target, depth, depth_component(session._depthbuffer_size), premap._resolution, premap._resolution, GL_DEPTH_COMPONENT, GL_FLOAT);
    if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    glPolygonMode( GL_FRONT_AND_BACK, cam._dt == DRAWTYPE::wireframe || cam._dt == DRAWTYPE::line || cam._dt == DRAWTYPE::frameline ? GL_LINE : GL_FILL);
    if (contains_nan(world_to_camera_cur)){return std::make_shared<premap_t>(premap);}
    switch (premap._coordinate_system)
    {
        case COORDINATE_SPHERICAL_APPROXIMATED:
        {
            float fova = premap._fov * (M_PI / 180);
            approximation_shader._program->bind();
            setup_framebuffer(GL_TEXTURE_2D, premap._resolution, session, framebuffer, depth);
            glUniform(approximation_shader._fovUniform,     static_cast<GLfloat>(fova));
            glUniform(approximation_shader._fovCapUniform,  static_cast<GLfloat>(1/tan(fova)));
            glUniform(approximation_shader._cropUniform,    static_cast<GLboolean>(session._crop));
            if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
            render_objects(objects,
                        null_material,
                        approximation_shader,
                        premap._frame,
                        premap._framedenominator,
                        premap._diffobj,
                        premap._diffbackward,
                        premap._diffforward,
                        premap._diffnormalize,
                        premap._difffallback,
                        premap._smoothing,
                        premap._coordinate_system,
                        world_to_camera_cur,
                        world_to_camera_pre,
                        world_to_camera_cur,
                        world_to_camera_post,
                        frame_stats,
                        session._debug);
            approximation_shader._program->release();
            break;
        }
        case COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS:
        {
            cubemap_shader._program->bind();
            setup_framebuffer(GL_TEXTURE_CUBE_MAP, premap._resolution, session, framebuffer, depth);
            cubemap_shader._program->setUniformValueArray(cubemap_shader._cbMatrixUniform ,&cubemap_camera_to_view[0],6);
            render_objects(
                objects,
                null_material,
                cubemap_shader,
                premap._frame,
                premap._framedenominator,
                premap._diffobj,
                premap._diffbackward,
                premap._diffbackward,
                premap._diffnormalize,
                premap._difffallback,
                premap._smoothing,
                premap._coordinate_system,
                world_to_camera_cur,
                world_to_camera_pre,
                world_to_camera_cur,
                world_to_camera_post,
                frame_stats,
                session._debug);
            cubemap_shader._program->release();
            break;
        }
        case COORDINATE_EQUIRECTANGULAR:
        case COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS:
        {
            perspective_shader._program->bind();
            for (size_t f = 0; f < 6; ++f)
            {
                if (premap._coordinate_system == COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS && ((f == 4 && premap._fov < 120) || (f != 5 && premap._fov <= 45))){continue;}
                QMatrix4x4 world_to_view = cubemap_camera_to_view[f] * world_to_camera_cur;
                setup_framebuffer(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, premap._resolution, session, framebuffer, depth);
                render_objects(
                    objects,
                    null_material,
                    perspective_shader,
                    premap._frame,
                    premap._framedenominator,
                    premap._diffobj,
                    premap._diffbackward,
                    premap._diffforward,
                    premap._diffnormalize,
                    premap._difffallback,
                    premap._smoothing,
                    premap._coordinate_system,
                    world_to_view,
                    world_to_camera_pre,
                    world_to_camera_cur,
                    world_to_camera_post,
                    frame_stats,
                    session._debug);
            }
            perspective_shader._program->release();
            break;
        }
        case COORDINATE_END:
            throw std::runtime_error("Invalid coordinate system");
    }
    if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    auto result = std::make_shared<premap_t>(premap);
    _premaps.push_back(result);
    return result;
}

bool premap_t::operator==(premap_t const & premap) const
{
    return (_world_to_camera_cur == premap._world_to_camera_cur || (contains_nan(_world_to_camera_cur) && contains_nan(premap._world_to_camera_cur)))
           && _cam              == premap._cam
           && _smoothing        == premap._smoothing
           && _frame            == premap._frame
           && _diffnormalize    == premap._diffnormalize
           && _difffallback     == premap._difffallback
           && _difftrans        == premap._difftrans
           && _diffrot          == premap._diffrot
           && _diffobj          == premap._diffobj
           && _diffbackward     == premap._diffbackward
           && _diffforward      == premap._diffforward
           && _fov              == premap._fov
           && _resolution       == premap._resolution
           && _coordinate_system == premap._coordinate_system;
}

//static const std::vector<vec2f_t> depth_of_field_translations({{0,0},{1,0},{-1,0},{0,1},{0,-1}});
static const std::vector<vec2f_t> depth_of_field_translations({{0,0},{2./3.,0},{1./3.,sqrt(1./3.)},{-1./3.,sqrt(1./3.)},{-2./3.,0},{-1./3.,-sqrt(1./3.)},{1./3.,-sqrt(1./3.)}});

void init_premap(session_t const & session, premap_t & premap)
{
    premap._coordinate_system= session._coordinate_system;
    premap._smoothing       = session._smoothing;
    premap._frame           = session._m_frame;
    premap._framedenominator= session._framedenominator;
    premap._resolution      = session._preresolution;
    premap._diffnormalize   = session._diffnormalize;
    premap._difffallback    = session._difffallback;
    premap._difftrans       = session._difftrans;
    premap._diffrot         = session._diffrot;
    premap._diffobj         = session._diffobjects;
    premap._diffbackward    = session._diffbackward;
    premap._diffforward     = session._diffforward;
    premap._fov             = session._fov;
}

void init_matrices(active_camera_t const & cam, size_t idx, premap_t & premap)
{
    premap._world_to_camera_pre  = cam._world_to_cam_pre [idx];
    premap._world_to_camera_cur  = cam._world_to_cam_cur [idx];
    premap._world_to_camera_post = cam._world_to_cam_post[idx];
}

void RenderingWindow::render()
{
    if (destroyed){return;}
    uint8_t overlay = 1;
    bool show_arrows = session._show_arrows;
    bool show_curser = session._show_curser;
    bool show_flow = session._show_flow;
    size_t loglevel = session._loglevel;
    std::string const & show_only = session._show_only;
    QPoint curser_pos = mapFromGlobal(QCursor::pos());
    const high_res_clock current_time = std::chrono::high_resolution_clock::now();
    bool debug = session._debug;
    frame_stats_t frame_stats;
    set_activated(GL_DEBUG_OUTPUT, debug);
    set_activated(GL_DEBUG_OUTPUT_SYNCHRONOUS, debug);
    if (session._loglevel > 5){std::cout << "start render" << std::endl;}
    if (session._reload_shader)
    {
        initialize();
        session._reload_shader = false;
    }
    scene_t & scene = session._scene;
    for (texture_t & tex : scene._textures)
    {
        if (!tex._tex)
        {
            tex._tex = create_texture(tex._width, tex._height, tex._channels, tex._datatype);
            tex._defined = false;
        }
    }
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale , height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        std::lock_guard<std::mutex> lockGuard(scene._mtx);
        if (show_only != "")
        {
            for (size_t i = 0; i < scene._framelists.size(); ++i)
            {
                if (scene._framelists[i]._name == show_only)
                {
                    if (scene._framelists[i]._frames.empty()){continue;}
                    if (_lastframe < session._m_frame)
                    {
                        auto iter = std::lower_bound(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), session._m_frame);
                        if (iter == scene._framelists[i]._frames.end()){--iter;}
                        session._m_frame = *iter;
                    }
                    else
                    {
                        auto iter = std::upper_bound(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), session._m_frame);
                        if (iter != scene._framelists[i]._frames.begin()){--iter;}
                        if (iter != scene._framelists[i]._frames.end())
                        {
                            session._m_frame = *iter;
                        }
                    }
                }
            }
        }
        _lastframe = session._m_frame;
        if (_scene_updated)
        {
            _premaps.clear();
            _scene_updated = false;
        }
        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        if (session._loglevel > 5){std::cout << "locked scene" << std::endl;}
        premap_t premap;
        init_premap(session, premap);

        size_t motion_blur = session._motion_blur;
        for (camera_t const & cam : scene._cameras)
        {
            if (cam._visible)
            {
                _active_cameras.push_back(active_camera_t(&cam));
                active_camera_t & acam = _active_cameras.back();
                size_t blur_framecount = std::max<frameindex_t>(premap._framedenominator * motion_blur, 1);
                acam._world_to_cam_pre .resize(blur_framecount);
                acam._world_to_cam_cur .resize(blur_framecount);
                acam._world_to_cam_post.resize(blur_framecount);
                for (frameindex_t b = 0; b < static_cast<frameindex_t>(blur_framecount); ++b)
                {
                    std::array<QMatrix4x4, 3> matrices = {acam._world_to_cam_pre[b], acam._world_to_cam_cur[b], acam._world_to_cam_post[b]};
                    bool difftranscurrent = true && cam._difftrans;
                    bool diffrotcurrent   = true && cam._diffrot;
                    frameindex_t frame = premap._frame - b;
                    std::array<frameindex_t, 3> rotframes   = {frame + (diffrotcurrent   ? premap._diffbackward : 0), frame, frame + (diffrotcurrent   ? premap._diffforward  : 0)};
                    std::array<frameindex_t, 3> transframes = {frame + (difftranscurrent ? premap._diffbackward : 0), frame, frame + (difftranscurrent ? premap._diffforward  : 0)};
                    for (auto iter = cam._transform_pipeline.rbegin(); iter != cam._transform_pipeline.rend(); ++iter)
                    {
                        transform_matrices(
                            iter->first.get(),
                            iter->second,
                            matrices,
                            rotframes,
                            transframes,
                            premap._framedenominator,
                            premap._smoothing,
                            premap._smoothing);
                    }
                    for (QMatrix4x4 & m : matrices)
                    {
                        m *= cam._transformation;
                    }
                    acam._world_to_cam_pre[b] = matrices[0].inverted();
                    acam._world_to_cam_cur[b] = matrices[0].inverted();
                    acam._world_to_cam_post[b] = matrices[0].inverted();
                }
            }
        }
        size_t num_cams = _active_cameras.size();
        size_t num_views = static_cast<size_t>(session._show_raytraced) + static_cast<size_t>(session._show_position) + static_cast<size_t>(session._show_index) + static_cast<size_t>(session._show_flow) + static_cast<size_t>(session._show_depth)  + static_cast<size_t>(session._show_visibility);
        views.clear();

        marker.clear();
        QPointF curserViewPos;
        if (_active_cameras.size() != 0 && num_views != 0)
        {
            curserViewPos.setX((curser_pos.x() % (width() / (_active_cameras.size())))/static_cast<float>(width() / (_active_cameras.size())));
            curserViewPos.setY((curser_pos.y() % (height() / num_views))/static_cast<float>(height() / num_views));
            if (loglevel > 5){std::cout << curserViewPos.x() << ' ' << curserViewPos.y() << '\t';}
        }

        switch(session._culling)
        {
            case 0: break;
            case 1: glCullFace(GL_FRONT);           break;
            case 2: glCullFace(GL_BACK);            break;
            case 3: glCullFace(GL_FRONT_AND_BACK);  break;
            default:    throw std::runtime_error("Illegal face-culling value");
        }
        set_activated(GL_CULL_FACE, session._culling!= 0);

        gl_framebuffer_id FramebufferName;
        gen_framebuffers_direct(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        float fova = premap._fov * (M_PI / 180);
        for (size_t c = 0; c < _active_cameras.size(); ++c)
        {
            if (!contains_nan(_active_cameras[c]._world_to_cam_cur[0]))
            {
                camera_t const & cam = *_active_cameras[c]._cam;
                std::vector<std::shared_ptr<premap_t> > rendered_premaps;
                vec2f_t aperture = cam._aperture;
                if (!cam._key_aperture.empty())
                {
                    aperture *= smoothed(cam._key_aperture, premap._framedenominator, premap._frame - premap._smoothing, premap._frame + premap._smoothing);
                }
                for (size_t tr = 0; tr < (cam._aperture.dot() == 0 || aperture.dot() == 0 ? 1 : depth_of_field_translations.size()); ++tr)
                {
                    vec2f_t translate = depth_of_field_translations[tr] * aperture;
                    for (size_t i = 0; i < _active_cameras[c]._world_to_cam_cur.size(); ++i)
                    {
                        premap_t current_premap = premap;
                        current_premap._frame -= i;
                        init_matrices(_active_cameras[c], i, current_premap);
                        if (tr)
                        {
                            current_premap._world_to_camera_pre .translate(translate[0], translate[1], 0);
                            current_premap._world_to_camera_cur .translate(translate[0], translate[1], 0);
                            current_premap._world_to_camera_post.translate(translate[0], translate[1], 0);
                        }
                        current_premap._cam = &cam;
                        rendered_premaps.push_back(render_premap(current_premap, cam._meshes, scene._null_material, frame_stats));
                    }
                }
                if (num_views != 0)
                {
                    size_t x = c * width() / num_cams;
                    size_t w = width() / num_cams;
                    size_t h = height()/num_views;
                    size_t y = 0;
                    if (session._show_raytraced) {views.push_back(view_t({cam._name, x, (y++) * h, w, h, VIEWTYPE_RENDERED,  rendered_premaps}));}
                    if (session._show_position)  {views.push_back(view_t({cam._name, x, (y++) * h, w, h, VIEWTYPE_POSITION,  rendered_premaps}));}
                    if (session._show_index)     {views.push_back(view_t({cam._name, x, (y++) * h, w, h, VIEWTYPE_INDEX,     rendered_premaps}));}
                    if (session._show_flow)      {views.push_back(view_t({cam._name, x, (y++) * h, w, h, VIEWTYPE_FLOW,      rendered_premaps}));}
                    if (session._show_depth)     {views.push_back(view_t({cam._name, x, (y++) * h, w, h, VIEWTYPE_DEPTH,     rendered_premaps}));}
                    if (session._show_visibility){views.push_back(view_t({cam._name, x, (y++) * h, w, h, VIEWTYPE_VISIBILITY,rendered_premaps}));}
                }
            }
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        FramebufferName.destroy();
        glDisable(GL_CULL_FACE);
        remapping_shader_t *remapping_shader = nullptr;
        switch(premap._coordinate_system)
        {
            case COORDINATE_SPHERICAL_APPROXIMATED:          remapping_shader = static_cast<remapping_shader_t*>(&remapping_identity_shader);break;
            case COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS:
            case COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS:    remapping_shader = static_cast<remapping_shader_t*>(&remapping_spherical_shader);break;
            case COORDINATE_EQUIRECTANGULAR:                 remapping_shader = static_cast<remapping_shader_t*>(&remapping_equirectangular_shader);break;
            case COORDINATE_END:                             throw std::runtime_error("Invalid coordinate system");
        }
        remapping_shader->_program->bind();
        glUniform(remapping_shader->_fovUniform, fova);
        glUniform(remapping_shader->_cropUniform, static_cast<GLboolean>(session._crop));
        if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        QVector4D curser_3d;

        size_t arrow_lines = 16;
        if (show_arrows)
        {
            _arrow_handles.reserve(num_cams);
            if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
            for (active_camera_t const & active_cam : _active_cameras)
            {
                if (contains_nan(active_cam._world_to_cam_cur[0])){continue;}
                std::shared_ptr<screenshot_handle_t> current = std::make_shared<screenshot_handle_t>();
                current->_task = RENDER_TO_TEXTURE;
                current->_width = arrow_lines;
                current->_height = arrow_lines;
                current->_channels = 2;
                current->_type = VIEWTYPE_FLOW;
                current->_ignore_nan = true;
                current->set_datatype(gl_type<float>);
                current->_state = screenshot_state_queued;
                current->_camera = active_cam._cam->_name;
                current->_prerendering = std::numeric_limits<size_t>::max();
                current->_task = TAKE_SCREENSHOT;

                premap_t current_premap = premap;
                current_premap._world_to_camera_cur = active_cam._world_to_cam_cur[0];
                current_premap._cam = active_cam._cam;
                auto result = std::find_if(_premaps.begin(), _premaps.end(), pointer_comparator_t(current_premap));
                assert(result != _premaps.end());
                render_setting_t render_setting;
                render_setting._viewtype         = current->_type;
                render_setting._transform        = current_premap._world_to_camera_cur.inverted();
                render_setting._position_texture = (*result)->_framebuffer._position;
                render_setting._rendered_texture = (*result)->_framebuffer._flow;
                render_setting._color_transformation.scale(1, 1, 1);
                render_setting._flipped = false;
                render_to_texture(*current, render_setting, false, loglevel, session._debug, *remapping_shader);
                dmaTextureCopy(*current, session._debug);
                _arrow_handles.emplace_back(current);
                clean();

                if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
            }
        }

        scene._screenshot_handles.erase(std::remove_if(scene._screenshot_handles.begin(), scene._screenshot_handles.end(),
            [&scene, this, &premap, loglevel, remapping_shader](screenshot_handle_t *current)
            {
                if (current->_task == SAVE_TEXTURE)
                {
                    return false;
                }
                texture_t *tex = nullptr;
                if (current->_task == RENDER_TO_TEXTURE)
                {
                    tex = scene.get_texture(current->_texture);
                    if (!tex)
                    {
                        std::cerr << "error, texture " << current->_texture << " doesn't exist" << std::endl;
                        current->set_state(screenshot_state_error);
                        return true;
                    }
                    else
                    {
                        current->_textureId = tex->_tex;
                        current->_width     = tex->_width;
                        current->_height    = tex->_height;
                    }
                }
                auto active_cam = std::find_if(_active_cameras.begin(), _active_cameras.end(), camera_name_comparator_t(current->_camera));
                if (active_cam == _active_cameras.end())
                {
                    std::cerr << "Camera is not active or does not exist " << current->_camera << std::endl;
                    current->set_state(screenshot_state_error);
                    return true;
                }
                if (contains_nan(active_cam->_world_to_cam_cur[0]))
                {
                    std::cout << "camera-transformation invalid " << current->_id << std::endl;
                    if (current->_ignore_nan)
                    {
                        current->set_state(screenshot_state_copied);
                    }
                    else
                    {
                        current->set_state(screenshot_state_error);
                    }
                    return true;
                }
                std::cout << "rendering_screenshot " << current->_id << std::endl;
                if (current->_prerendering == std::numeric_limits<size_t>::max())
                {
                    render_setting_t render_setting;
                    render_setting._viewtype = current->_type;
                    premap_t current_premap = premap;
                    current_premap._world_to_camera_cur = active_cam->_world_to_cam_cur[0];
                    current_premap._cam = active_cam->_cam;
                    if (_active_cameras.size() == 2 && current->_type == VIEWTYPE_POSITION)
                    {
                        if (current->_camera == _active_cameras[0]._cam->_name) {render_setting._transform = _active_cameras[1]._world_to_cam_cur[0] * _active_cameras[0]._world_to_cam_cur[0].inverted();}
                        else                                                    {render_setting._transform = _active_cameras[0]._world_to_cam_cur[0].inverted() * _active_cameras[1]._world_to_cam_cur[0];} 
                    }
                    else
                    {
                        render_setting._transform = current_premap._world_to_camera_cur.inverted();
                    }
                    auto result = std::find_if(_premaps.begin(), _premaps.end(), pointer_comparator_t(current_premap));
                    assert (result != _premaps.end());
                    rendered_framebuffer_t &frb = (*result)->_framebuffer;
                    render_setting._position_texture = frb._position;
                    render_setting._flipped = current->_flip;
                    if (current->_type == VIEWTYPE_FLOW){render_setting._color_transformation.scale(-1,1,1);}
                    current->_flip = false;
                    render_setting._rendered_texture = frb.get(current->_type);
                    for (size_t i = 0; i < current->_vcam.size(); ++i)
                    {
                        auto active_vcam = std::find_if(_active_cameras.begin(), _active_cameras.end(), camera_name_comparator_t(current->_vcam[i]));
                        if (active_vcam == _active_cameras.end())
                        {
                            std::cerr << "Camera is not active or does not exist " << current->_vcam[i] << std::endl;
                            continue;
                        }
                        QMatrix4x4 & other_world_to_cam_cur = active_vcam->_world_to_cam_cur[0];
                        if (contains_nan(other_world_to_cam_cur))continue;
                        premap_t other_premap = premap;
                        other_premap._world_to_camera_cur = other_world_to_cam_cur;
                        other_premap._cam = active_vcam->_cam;
                        auto other_result = std::find_if(_premaps.begin(), _premaps.end(), pointer_comparator_t(other_premap));
                        render_setting._other_views.emplace_back(other_world_to_cam_cur, (*other_result)->_framebuffer._position);
                    }
                    render_to_texture(*current, render_setting, tex && tex->_defined, loglevel, session._debug, *remapping_shader);
                    if (current->_task == TAKE_SCREENSHOT)
                    {
                        dmaTextureCopy(*current, session._debug);
                        clean();
                    }
                    else if (current->_task == RENDER_TO_TEXTURE)
                    {
                        tex->_defined =true;
                        current->set_state(screenshot_state_saved);
                        return true;
                    }
                }
                else
                {
                    if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
                    if (current->_task == TAKE_SCREENSHOT)
                    {
                        premap_t current_premap = premap;
                        current_premap._world_to_camera_cur = active_cam->_world_to_cam_cur[0];
                        current_premap._cam = active_cam->_cam;
                        auto result = std::find_if(_premaps.begin(), _premaps.end(), pointer_comparator_t(current_premap));
                        rendered_framebuffer_t &frb = (*result)->_framebuffer;

                        current->_width = premap._resolution;
                        current->_height = premap._resolution;
                        current->_textureId = frb.get(current->_type);
                        current->_state =  screenshot_state_rendered_texture;
                        dmaTextureCopy(*current, session._debug);
                    }
                }
                std::cout << "rendered_screenshot" << current->_id << std::endl;
                return false;
            }),scene._screenshot_handles.end());
        
        scene._screenshot_handles.erase(std::remove_if(scene._screenshot_handles.begin(), scene._screenshot_handles.end(),
            [&scene, this](screenshot_handle_t *current)
            {
                std::cout << current->_id << "task: " << current->_task << std::endl;
                if (current->_task != SAVE_TEXTURE){return false;}
                texture_t const * texture = scene.get_texture(current->_texture);
                if (!texture){ 
                    std::cout << "don't rendering_screenshot " << current->_id << std::endl;
                    current->set_state(screenshot_state_error);
                    return true;}
                current->_textureId = texture->_tex;
                current->_width = texture->_width;
                current->_height = texture->_height;
                current->set_state(screenshot_state_rendered_texture);
                current->_channels = texture->_channels;
                current->set_datatype(texture->_datatype);
                current->_type = VIEWTYPE_END;
                current->_prerendering = std::numeric_limits<size_t>::max();
                dmaTextureCopy(*current,this->session._debug);
                return false;
        }),scene._screenshot_handles.end());

        screenshot_handle_t curser_handle;
        if (show_curser && num_cams != 0)
        {
            size_t icam = clamp(curser_pos.x() * num_cams / width(), size_t(0), num_cams-1);
            curser_handle._task = RENDER_TO_TEXTURE;
            curser_handle._width = 1024;
            curser_handle._height = 1024;
            curser_handle._channels = 3;
            curser_handle._type = VIEWTYPE_POSITION;
            curser_handle._ignore_nan = true;
            curser_handle.set_datatype(GL_FLOAT);
            curser_handle._state = screenshot_state_inited;
            curser_handle._camera = scene._cameras[icam]._name;
            
            premap_t current_premap = premap;
            current_premap._world_to_camera_cur = _active_cameras[icam]._world_to_cam_cur[0];
            current_premap._cam = _active_cameras[icam]._cam;
            auto result = std::find_if(_premaps.begin(), _premaps.end(), pointer_comparator_t(current_premap));
            rendered_framebuffer_t &frb = (*result)->_framebuffer;

            render_setting_t render_setting;
            render_setting._viewtype = curser_handle._type;
            render_setting._transform = _active_cameras[icam]._world_to_cam_cur[0];
            render_setting._position_texture = frb._position;
            render_setting._rendered_texture = frb.get(curser_handle._type);
            render_setting._flipped = false;
            render_to_texture(curser_handle, render_setting, false, loglevel, session._debug, *remapping_shader);
            dmaTextureCopy(curser_handle, session._debug);
            clean();
        }
        gl_texture_id virtualScreenTexture;
        gl_framebuffer_id virtualScreenFramebuffer;
        gl_renderbuffer_id virtualScreenDepth;
        if (session._indirect_rendering)
        {
            gen_textures_direct(1, &virtualScreenTexture);
            setupTexture(GL_TEXTURE_2D, virtualScreenTexture, GL_RGBA16, width(), height(), GL_RGBA, GL_UNSIGNED_SHORT);
            gen_framebuffers_direct(1, &virtualScreenFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, virtualScreenFramebuffer);
            gen_renderbuffers_direct(1, &virtualScreenDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, virtualScreenDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width(), height());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, virtualScreenDepth);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, virtualScreenTexture, 0);
            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, DrawBuffers);
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);  

        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
        float motion_blur_normalization = 1;
        size_t motion_blur_curve_range = 1;
        if (!session._motion_blur_custom_curve.empty() && session._motion_blur_curve == MOTION_BLUR_CUSTOM)
        {
            motion_blur_normalization = smoothed(session._motion_blur_custom_curve,1, session._motion_blur_custom_curve.begin()->second,session._motion_blur_custom_curve.rbegin()->second);
            motion_blur_curve_range = session._motion_blur_custom_curve.rbegin()->first - session._motion_blur_custom_curve.begin()->first;
        }
        size_t max_dist = std::max(1lu, motion_blur * premap._framedenominator);
        for (view_t & view : views)
        {
            double scale = 1./view._premaps.size();
            if (!session._motion_blur_custom_curve.empty())
            {
                scale *= motion_blur_normalization;
            }
            glViewport(view._x, view._y, view._width, view._height);
            render_setting_t render_setting;
            render_setting._viewtype = view._viewtype;
            render_setting._flipped = false;
            for (size_t dof = 0; dof < view._premaps.size(); ++dof)
            {
                std::shared_ptr<premap_t> cur_premap = view._premaps[dof];
                if (_active_cameras.size() == 2 && view._viewtype == VIEWTYPE_POSITION)
                {
                    if (view._camera == _active_cameras[0]._cam->_name){render_setting._transform = _active_cameras[1]._world_to_cam_cur[0] * _active_cameras[0]._world_to_cam_cur[0].inverted();}
                    else{                                               render_setting._transform = _active_cameras[0]._world_to_cam_cur[0].inverted() * _active_cameras[1]._world_to_cam_cur[0];}
                }
                else
                {
                    render_setting._transform = cur_premap->_world_to_camera_cur.inverted();
                }
                rendered_framebuffer_t &frb = cur_premap->_framebuffer;                
                render_setting._position_texture = frb._position;
                render_setting._rendered_texture = cur_premap->_framebuffer.get(view._viewtype);
                if (session._show_rendered_visibility || view._viewtype == VIEWTYPE_VISIBILITY)
                {
                    for (size_t i = 0; i < _active_cameras.size() && i < 3; ++i)
                    {
                        active_camera_t other_active = _active_cameras[i];
                        if (contains_nan(other_active._world_to_cam_cur[dof]))continue;
                        premap_t other_premap = *cur_premap;
                        other_premap._world_to_camera_cur = other_active._world_to_cam_cur[dof];
                        other_premap._cam = other_active._cam;
                        auto other_result = std::find_if(_premaps.begin(), _premaps.end(), pointer_comparator_t(other_premap));
                        rendered_framebuffer_t &other_frb = (*other_result)->_framebuffer;
                        render_setting._other_views.emplace_back(other_premap._world_to_camera_cur, other_frb._position);
                    }
                }
                if (view._viewtype == VIEWTYPE_INDEX)
                {
                // render_setting._color_transformation.scale(1./255,1./255,1./255);
                }
                else if (view._viewtype == VIEWTYPE_DEPTH)
                {
                    float depthscale = session._depth_scale;
                    render_setting._color_transformation.scale(depthscale, depthscale, depthscale);                
                }
                else if (view._viewtype == VIEWTYPE_FLOW)
                {
                    render_setting._color_transformation.scale(-157, 157, 157);//TODO add flowscale
                }
                frameindex_t dist = session._m_frame - cur_premap->_frame;
                float cur_scale = scale;
                if (max_dist > 1)
                {
                    switch (session._motion_blur_curve)
                    {
                        case MOTION_BLUR_CUBIC:     cur_scale *= (float)(4 * (max_dist - dist)) / (float)max_dist;__attribute__ ((fallthrough));
                        case MOTION_BLUR_QUADRATIC: cur_scale *= (float)(3 * (max_dist - dist)) / (float)max_dist;__attribute__ ((fallthrough));
                        case MOTION_BLUR_LINEAR:    cur_scale *= (float)(2 * (max_dist - dist)) / (float)(max_dist + 1);break;
                        case MOTION_BLUR_CONSTANT:  break;
                        case MOTION_BLUR_END:       break;
                        case MOTION_BLUR_CUSTOM:
                        {
                            if (!session._motion_blur_custom_curve.empty())
                            {
                                frameindex_t pos = dist * motion_blur_curve_range / cur_premap->_framedenominator * motion_blur;
                                cur_scale *= smoothed(session._motion_blur_custom_curve, 1, pos, pos);
                            }
                            break;
                        }
                    }
                }
                render_setting._color_transformation.scale(cur_scale, cur_scale, cur_scale);
                render_view(*remapping_shader, render_setting);
                render_setting._color_transformation.setToIdentity();
                render_setting._other_views.clear();
            }
        }
        if (session._indirect_rendering)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, virtualScreenFramebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0,0,width(), height(), 0, 0,width(), height(),  GL_COLOR_BUFFER_BIT, GL_NEAREST);
            virtualScreenFramebuffer.destroy();
            virtualScreenTexture.destroy();
            virtualScreenDepth.destroy();
        }
        glDisable(GL_BLEND);
        if (show_arrows)
        {
            for (size_t i = 0; i < _arrow_handles.size(); ++i)
            {
                screenshot_handle_t & current = *_arrow_handles[i];
                copy_pixel_buffer_to_screenshot(current, session._debug);                
                float *data = current.get_data<float>();
                
                size_t sy = current._height - static_cast<size_t>(curserViewPos.y() * current._height);
                size_t sx = static_cast<size_t>(curserViewPos.x() * current._width);
                if (sx < current._width && sy < current._height)
                {
                    size_t index = 2 * (sy * current._width + sx);
                    _curser_flow.emplace_back(data[index],data[index + 1]);
                }
                for (size_t y = 0; y < current._height; ++y)
                {
                    for (size_t x = 0; x < current._width; ++x)
                    {
                        float xf = (static_cast<float>(x) + 0.5) / current._width;
                        float yf = (static_cast<float>(y) + 0.5) / current._height;
                        
                        size_t index = 2 * (y * current._width + x);
                        float xdiff = -data[index];
                        float ydiff = data[index + 1];
                        if (!std::isnan(xdiff) && !std::isnan(ydiff))
                        {
                            for (view_t & view : views)
                            {
                                if (view._camera == current._camera && view._viewtype == (show_flow ? VIEWTYPE_FLOW : VIEWTYPE_RENDERED))
                                {
                                    arrows.emplace_back(arrow_t({xf * view._width + view._x, height() - yf * view._height - view._y, xdiff * view._width, ydiff * view._height}));
                                    //std::cout << arrows.back() << std::endl;
                                }
                            }
                        }
                    }
                }
                current.delete_data();
            }
        }

        if (show_curser && num_cams != 0)
        {
            copy_pixel_buffer_to_screenshot(curser_handle, session._debug);
            size_t sy = 1024 - static_cast<size_t>(curserViewPos.y() * 1024);
            size_t sx = static_cast<size_t>(curserViewPos.x() * 1024);
            if (sx < 1024 && sy < 1024)
            {
                size_t index = 3 * (sy * 1024 + sx);
                if (loglevel > 5)
                {
                    std::cout << index << '\t';
                }
                float *data = curser_handle.get_data<float>();
                curser_3d = QVector4D(data[index], data[index + 1], data[index + 2], 1);
                if (loglevel > 5)
                {
                    std::cout << curser_3d.x() << ' ' << curser_3d.y() << ' ' << curser_3d.z() << '\t';
                }
                curser_handle.delete_data();
                if (curser_3d.x() != 0 || curser_3d.y() != 0 || curser_3d.z() != 0)
                {
                    for (size_t icam = 0; icam < num_cams; ++icam)
                    {
                        QVector4D test = _active_cameras[icam]._world_to_cam_cur[0] * curser_3d;
                        if (loglevel > 5)
                        {
                            std::cout << "test " << test.x() << ' ' << test.y() << '\t';
                        }
                        std::array<float, 2> tmp = kart_to_equidistant(std::array<float, 3>({-test.x(), -test.y(), -test.z()}));
                        tmp[0] = -tmp[0];
                        
                        //std::array<float, 2> tmp = kart_to_equidistant(std::array<float, 3>({test.x(), test.y(), test.z()}));
                        for (size_t icam = 0; icam < 2; ++icam)
                        {
                            tmp[icam] = tmp[icam] * premap._fov / 45.;
                        }
                        if (tmp[0] * tmp[0] + tmp[1] * tmp[1] > 0.25)
                        {
                            continue;
                        }
                        tmp[0] += 0.5;
                        tmp[1] += 0.5;
                        for (view_t & view : views)
                        {
                            if (view._camera == _active_cameras[icam]._cam->_name)
                            {
                                marker.push_back(QPointF(tmp[0] * view._width + view._x, tmp[1] * view._height + view._y));
                            }
                        }
                        //marker.push_back(QPointF(test.x(), test.y()));
                        if (loglevel > 5)
                        {
                            std::cout << "marker " << tmp[0] << ' ' << tmp[1] << std::endl;
                        }
                    }
                }
            }  
        }
        for (screenshot_handle_t * current : scene._screenshot_handles)
        {
            if (current->_task == TAKE_SCREENSHOT || current->_task == SAVE_TEXTURE)
            {
                copy_pixel_buffer_to_screenshot(*current, session._debug);
                last_screenshottimes.emplace_back(std::chrono::high_resolution_clock::now());
            }
        }
        scene._screenshot_handles.clear();
        remapping_shader->_program->release();

        glViewport(0,0,width(), height());
        glDisable(GL_DEPTH_TEST);
        if (qogpd == nullptr)
        {
            qogpd = std::make_unique<QOpenGLPaintDevice>();
        }
        int ratio = devicePixelRatio();
        qogpd->setSize(QSize(width() * ratio, height() * ratio));
        qogpd->setDevicePixelRatio(ratio);
        QPainter painter(qogpd.get());
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        painter.setFont(QFont("Times", 20));

        //painter.setPen(QColor(clamp(static_cast<int>(curser_3d.x() * 255), 0, 0xFF), clamp(static_cast<int>(curser_3d.y() * 255), 0, 0xFF), clamp(static_cast<int>(curser_3d.z() * 255), 0, 0xFF), 255));
        //painter.setPen(QColor(255, 255, 255, 255));
        //painter.drawEllipse(QPointF(100,100), 10, 10);

        last_rendertimes.push_back(current_time);
        //std::cout << last_rendertimes.front() << '+' << CLOCKS_PER_SEC <<'<' << current_time << std::endl;
        while (std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_rendertimes.front()).count() > 1000000)
        {
            last_rendertimes.pop_front();
        }
        while (last_screenshottimes.size() > 0 && std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_screenshottimes.front()).count() > 1000000)
        {
            last_screenshottimes.pop_front();
        }
        double duration = (static_cast<std::chrono::duration<double> >( current_time - last_rendertime )).count();

        if (session._show_debug_info)
        {
            painter.setPen(QColor(255,255,255,255));
            if (overlay != 0)
            {
                for(view_t view : views)
                {
                    double x0 = view._x, y0 = view._y;
                    double x1 = x0 + view._width, y1 = y0 + view._height;
                    double cx = x0 + 0.5 * view._width, cy = y0 + 0.5 * view._height;
                    if(session._coordinate_system != COORDINATE_EQUIRECTANGULAR)
                    {
                        painter.drawEllipse(QPointF(view._width * 0.5 + view._x,0.5 * view._height + view._y), view._width/2, view._height/2);
                    }
                    painter.drawLine(cx, y0, cx, y1);
                    painter.drawLine(x0, cy, x1, cy);
                }
            }
            for (QPointF const & m : marker)
            {
                painter.drawEllipse(QPointF(m.x(),m.y()), 10, 10);
            }
            painter.drawText(30,30, QString::number(premap._frame));
            painter.drawText(30,60, "fps " + QString::number(last_rendertimes.size()) + ' ' + QString::number(1 / duration));
            painter.drawText(30,90, "scr " + QString::number(last_screenshottimes.size()));
            painter.drawText(30,120, "faces " + QString::number(frame_stats._rendered_faces) + ' ' + QString::number((frame_stats._rendered_faces * 1000 / std::max<long>(frame_stats._active_faces,1)) / 10.) + '%');
            size_t row = 0;
            std::string tmp;
            tmp.reserve(128);
            for (vec2f_t const & cf : _curser_flow)
            {
                tmp = '('+std::to_string(cf.x()) + ' ' + std::to_string(cf.y()) + ") " + std::to_string(sqrt(cf.dot()));
                painter.drawText(400,30 + row * 30,QString(tmp.c_str()));
                ++row;
            }
        }
        for (arrow_t const & arrow : arrows)
        {
            float headx = arrow._x0+ arrow._x1, heady = arrow._y0 + arrow._y1;
            float centerx = arrow._x0+ arrow._x1 * 0.5, centery = arrow._y0 + arrow._y1 * 0.5;
            painter.drawLine(arrow._x0, arrow._y0, headx, heady);
            painter.drawLine(centerx + arrow._y1 * 0.5, centery - arrow._x1 * 0.5, headx, heady);
            painter.drawLine(centerx - arrow._y1 * 0.5, centery + arrow._x1 * 0.5, headx, heady);
        }
        arrows.clear();
        last_rendertime = current_time;
        if (session._show_framelists)
        {
            for (size_t i = 0; i < scene._framelists.size(); ++i)
            {
                auto & fr_list = scene._framelists[i];
                bool found = std::binary_search(fr_list._frames.begin(), fr_list._frames.end(), premap._frame);
                painter.setPen(QColor((!found) * 255,found * 255,0,255));
                painter.drawText(30, i*30 + 60, QString(fr_list._name.c_str()));

                auto iter = std::lower_bound(fr_list._frames.begin(), fr_list._frames.end(), premap._frame - 50);
                painter.setPen(QColor((!found) * 255,found * 255,0,255));
                painter.drawText(30, i*30 + 60, QString(fr_list._name.c_str()));
                for (; iter != fr_list._frames.end(); ++iter)
                {
                    frameindex_t fr = *iter;
                    if (fr > premap._frame + 50){break;}
                    painter.drawEllipse(600 + (fr - premap._frame) * 10, i * 30 + 45, 5, 5);
                }
            }
        }
        painter.end();
        if (session._screenshot != "")
        {
            //awx_ScreenShot(session._screenshot);
            session._screenshot = "";
        }
        if (session._play != 0)
        {
            session._m_frame += session._play * (session._realtime ? session._frames_per_second * duration : session._frames_per_step);
            if (premap._frame != session._m_frame)
            {
                _updating = true;
                session.scene_update(UPDATE_FRAME);
                _updating = false;
            }
        }
        ++session._rendered_frames;
        std::vector<wait_for_rendered_frame_t*> & wait_for_rendered_frame_handles = session._wait_for_rendered_frame_handles;
        wait_for_rendered_frame_handles.erase(std::remove_if(wait_for_rendered_frame_handles.begin(), wait_for_rendered_frame_handles.end(),[this, loglevel](wait_for_rendered_frame_t *wait){
            if (wait->_frame < this->session._rendered_frames)
            {
                wait->_value = true;
                wait->_cv.notify_all();
                return true;
            }
            return false;
        }), wait_for_rendered_frame_handles.end());
        _curser_flow.clear();
        _arrow_handles.clear();
        _active_cameras.clear();
        clean();
        size_t max_premaps = session._max_premaps >= 0 ? session._max_premaps : num_cams * premap._framedenominator * motion_blur;
        if (_premaps.size() > max_premaps)
        {
            _premaps.erase(_premaps.begin(), _premaps.end() - max_premaps);
        }
        if (session._exit_program)
        {
            std::cout << "exit rendering view" << std::endl;
            perspective_shader.destroy();
            remapping_spherical_shader.destroy();
            remapping_identity_shader.destroy();
            approximation_shader.destroy();
            perspective_shader.destroy();
            _texture_white->destroy();
            _texture_white = nullptr;
            scene._textures.clear();
            scene._screenshot_handles.clear();
            scene._objects.clear();
            _premaps.clear();
            views.clear();
            clean();
            deleteLater();
            qogpd = nullptr;
            destroy();
            _exit = true;
            destroyed = true;
        }
    }//End of lock
    if (loglevel > 5)
    {
        std::cout << "end render" << std::endl;
    }
}

void RenderingWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        ++session._perm;
    }
}

