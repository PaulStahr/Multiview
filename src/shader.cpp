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

#include "shader.h"
#include "io_util.h"
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

void read_shader(std::string const & filename, std::string & result)
{
    result = IO_UTIL::read_file(filename);
    if (result.find('\0') != std::string::npos)
    {
        result.push_back('\0');
    }
    result.push_back('\0');
}

GLuint glGetAttribLocation(
    QOpenGLShaderProgram &program,
    std::string const & name,
    const char* str)
{
    int attr = program.attributeLocation(str);
    if (attr == -1)
    {
        std::cerr << std::string("Warning attribute ") + std::string(str) + std::string(" in ") + name + std::string(" not found");
    }
    return attr;
}

shader_t::shader_t(const std::string & name) : _name(name){}

rendering_shader_t::rendering_shader_t(const std::string & name) : shader_t(name){}

remapping_shader_t::remapping_shader_t(const std::string & name) : shader_t(name){}

remapping_spherical_shader_t::remapping_spherical_shader_t() : remapping_shader_t("remapping spherical"){}

remapping_identity_shader_t::remapping_identity_shader_t() : remapping_shader_t("remapping identity"){}

spherical_approximation_shader_t::spherical_approximation_shader_t() : rendering_shader_t("spherical approximation"){}

perspective_shader_t::perspective_shader_t() : rendering_shader_t("perspective"){}

cubemap_shader_t::cubemap_shader_t() : rendering_shader_t("cubemap"){}

void rendering_shader_t::init(QObject & )
{
    _posAttr            = glGetAttribLocation(*_program, _name, "posAttr");
    _normalAttr         = glGetAttribLocation(*_program, _name, "normalAttr");
    _corAttr            = glGetAttribLocation(*_program, _name, "corAttr");
    _matrixUniform      = _program->uniformLocation("matrix");
    _objMatrixUniform   = _program->uniformLocation("objMatrix");
    _curMatrixUniform   = _program->uniformLocation("curMatrix");
    _flowMatrixUniform  = _program->uniformLocation("flowMatrix");
    _texKd              = _program->uniformLocation("mapKd");
    _objidUniform       = _program->uniformLocation("objid");
    _colAmbientUniform  = _program->uniformLocation("colAmbient");
    _colDiffuseUniform  = _program->uniformLocation("colDiffuse");
}

void spherical_approximation_shader_t::init(QObject & context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str;
    read_shader(IO_UTIL::get_programpath() + "/shader/spherical_approximation_vertex_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    read_shader(IO_UTIL::get_programpath() + "/shader/spherical_approximation_fragment_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    rendering_shader_t::init(context);
    _fovUniform         = _program->uniformLocation("fovUnif");
    _fovCapUniform      = _program->uniformLocation("fovCapUnif");
    _cropUniform        = _program->uniformLocation("cropUnif");
}

void perspective_shader_t::init(QObject & context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str;
    read_shader(IO_UTIL::get_programpath() + "/shader/perspective_vertex_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    read_shader(IO_UTIL::get_programpath() + "/shader/perspective_fragment_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    rendering_shader_t::init(context);    
}

void cubemap_shader_t::init(QObject & context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str;
    read_shader(IO_UTIL::get_programpath() + "/shader/cubemap_geometry_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Geometry, str.c_str());
    read_shader(IO_UTIL::get_programpath() + "/shader/cubemap_vertex_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    read_shader(IO_UTIL::get_programpath() + "/shader/cubemap_fragment_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    rendering_shader_t::init(context);
    _cbMatrixUniform = _program->uniformLocation("cbMatrix");
}

void remapping_shader_t::init(QObject &)
{
    _posAttr = _program->attributeLocation("posAttr");
    _corAttr = _program->attributeLocation("corAttr");
    _cropUniform =   _program->uniformLocation("cropUnif");
    _fovUniform = _program->uniformLocation("fovUnif");
    _viewtypeUniform = _program->uniformLocation("viewtype");
    _transformUniform = _program->uniformLocation("transform");
    _transformColorUniform = _program->uniformLocation("transformColor");
    _transformCam[0] = _program->uniformLocation("tCam0");
    _transformCam[1] = _program->uniformLocation("tCam1");
    _transformCam[2] = _program->uniformLocation("tCam2");
    _positionMaps[0] = _program->uniformLocation("positionMap0");
    _positionMaps[1] = _program->uniformLocation("positionMap1");
    _positionMaps[2] = _program->uniformLocation("positionMap2");
    _numOverlays = _program->uniformLocation("numOverlays");
    _positionMap = _program->uniformLocation("positionMap");
    _texAttr = _program->uniformLocation("map");
}

void remapping_spherical_shader_t::init(QObject & context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str;
    read_shader(IO_UTIL::get_programpath() + "/shader/remapping_cubemap_spherical_vertex_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    
    read_shader(IO_UTIL::get_programpath() + "/shader/remapping_cubemap_spherical_fragment_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    remapping_shader_t::init(context);
}

void remapping_identity_shader_t::init(QObject& context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str;
    read_shader(IO_UTIL::get_programpath() + "/shader/remapping_spherical_spherical_vertex_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    
    read_shader(IO_UTIL::get_programpath() + "/shader/remapping_spherical_spherical_fragment_shader", str);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    remapping_shader_t::init(context);
}

void shader_t::destroy()
{
    if (_program)
    {
        delete _program;
        _program = nullptr;
    }
}
