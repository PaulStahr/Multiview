/*
Copyright (c) 2018 Paul Stahr

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

#ifndef SHADER_H
#define SHADER_H
#include <QtGlobal>

#if QT_VERSION_MAJOR == 5
#include <QtGui/QOpenGLShaderProgram>
#elif QT_VERSION_MAJOR == 6
#include <QtOpenGL/QOpenGLShaderProgram>
#else
#error Unsupported Qt version
#endif
#include <QObject>
#include <memory>

struct shader_t
{
    void destroy();
    std::unique_ptr<QOpenGLShaderProgram> _program = nullptr;
    std::string _name;
    std::string _vertex_source_file;
    std::string _geometry_source_file;
    std::string _fragment_source_file;

    shader_t(
        std::string const & name,
        std::string const & vertex_source_file,
        std::string const & geometry_source_file,
        std::string const & fragment_source_file);
    virtual void init(QObject & context) = 0;
};

struct gl_variable_base
{
    std::string _name;
    GLint _id;

    gl_variable_base(std::string const & name);
    
    operator GLint() const;
};

enum gl_variable_type
{
    uniform, attribute
};

template<gl_variable_type VARTYPE> struct gl_variable;
template<> struct gl_variable<attribute>;

template<gl_variable_type VARTYPE>
struct gl_variable : gl_variable_base
{
    gl_variable(std::string const & name);

    bool load_location(QOpenGLShaderProgram &program, std::string const & name);
};

template<> struct gl_variable<attribute> : gl_variable_base{
    gl_variable(std::string const & name);

    bool load_location(QOpenGLShaderProgram &program, std::string const & name);
};

template<> struct gl_variable<uniform> : gl_variable_base{
    gl_variable(std::string const & name);

    bool load_location(QOpenGLShaderProgram &program, std::string const & name);
};

struct rendering_shader_t : shader_t
{
    gl_variable<uniform> _lightDirectionUniform;
    gl_variable<attribute> _posAttr;
    gl_variable<attribute> _corAttr;
    gl_variable<attribute> _normalAttr;
    gl_variable<uniform> _objToScreenUniform;
    gl_variable<uniform> _objToWorldUniform;
    gl_variable<uniform> _objToCameraUniform;
    gl_variable<uniform> _objToCameraFlowUniform;
    gl_variable<uniform> _objToWorldNormalUniform;
    gl_variable<uniform> _colAmbientUniform;
    gl_variable<uniform> _colDiffuseUniform;
    gl_variable<uniform> _colSpecularUniform;
    gl_variable<uniform> _depth_offset;
    gl_variable<uniform> _alpha;
    gl_variable<uniform> _texKd;
    gl_variable<uniform> _objidUniform;

    rendering_shader_t(
        std::string const & name,
        std::string const & vertex_source_file,
        std::string const & geometry_source_file,
        std::string const & fragment_source_file);
    virtual void init(QObject & context);
};

struct spherical_approximation_shader_t : rendering_shader_t
{
    gl_variable<uniform> _fovUniform;
    gl_variable<uniform> _fovCapUniform;
    gl_variable<uniform> _cropUniform;
    spherical_approximation_shader_t();
    void init(QObject & context);
};

struct perspective_shader_t : rendering_shader_t
{
    perspective_shader_t();
    void init(QObject & context);
};

struct cubemap_shader_t : rendering_shader_t
{
    gl_variable<uniform> _cbMatrixUniform;
    cubemap_shader_t();
    void init(QObject & context);
};

struct remapping_shader_t : shader_t
{
    gl_variable<uniform> _texUniform;
    gl_variable<attribute> _posAttr;
    gl_variable<attribute> _corAttr;
    gl_variable<attribute> _colAttr;
    gl_variable<uniform> _fovUniform;
    gl_variable<uniform> _cropUniform;
    gl_variable<uniform> _viewtypeUniform;
    gl_variable<uniform> _transformUniform;
    gl_variable<uniform> _transformColorUniform;
    std::array<gl_variable<uniform>, 3> _transformCam;
    std::array<gl_variable<uniform>, 3> _positionMaps;
    gl_variable<uniform> _numOverlays;
    gl_variable<uniform> _positionMap;
    
    remapping_shader_t(
        std::string const & name,
        std::string const & vertex_source_file,
        std::string const & geometry_source_file,
        std::string const & fragment_source_file);
    virtual void init(QObject & context);
};

struct remapping_spherical_shader_t: remapping_shader_t
{
    remapping_spherical_shader_t();
    void init(QObject & context);
};

struct remapping_custom_shader_t: remapping_shader_t
{
    gl_variable<uniform> _pixelCoordinateMap; 
    remapping_custom_shader_t();
    void init(QObject & context);
};

struct remapping_equirectangular_shader_t: remapping_shader_t
{
    remapping_equirectangular_shader_t();
    void init(QObject & context);
};

struct remapping_identity_shader_t:remapping_shader_t
{
    remapping_identity_shader_t();
    void init(QObject & context);
};

struct remapping_cubemap_cubemap_shader_t: remapping_shader_t
{
    remapping_cubemap_cubemap_shader_t();
    void init(QObject & context);
};

#endif
