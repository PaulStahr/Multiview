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
#include <iostream>

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
        std::cerr << "Warning attribute " <<str<< " in " << name << " not found" << std::endl;
    }
    return attr;
}


GLuint glGetUniformLocation(
    QOpenGLShaderProgram &program,
    std::string const & name,
    const char* str)
{
    int attr = program.uniformLocation(str);
    if (attr == -1)
    {
        std::cerr << "Warning attribute " <<str<< " in " << name << " not found" << std::endl;
    }
    return attr;
}

shader_t::shader_t(
    const std::string & name,
    const std::string & vertex_source_file,
    const std::string & geometry_source_file,
    const std::string & fragment_source_file) : _name(name), _vertex_source_file(vertex_source_file), _geometry_source_file(geometry_source_file), _fragment_source_file(fragment_source_file){}

rendering_shader_t::rendering_shader_t(
    const std::string & name,
    const std::string & vertex_source_file,
    const std::string & geometry_source_file,
    const std::string & fragment_source_file) : shader_t(name, vertex_source_file, geometry_source_file, fragment_source_file),
    _posAttr                ("posAttr"),
    _corAttr                ("corAttr"),
    _normalAttr             ("normalAttr"),
    _objToScreenUniform     ("objToScreen"), 
    _objToWorldUniform      ("objToWorld"),
    _objToCameraUniform     ("objToCamera"),
    _objToCameraFlowUniform ("objToCameraFlow"),
    _objToWorldNormalUniform("objToWorldNormal"),
    _colAmbientUniform      ("colAmbient"),
    _colDiffuseUniform      ("colDiffuse"),
    _colSpecularUniform     ("colSpecular"),
    _alpha                  ("alpha"),
    _texKd                  ("mapKd"),
    _objidUniform           ("objid")
    {}

remapping_shader_t::remapping_shader_t(
    const std::string & name,
    const std::string & vertex_source_file,
    const std::string & geometry_source_file,
    const std::string & fragment_source_file) : shader_t(name, vertex_source_file, geometry_source_file, fragment_source_file){}

remapping_spherical_shader_t::remapping_spherical_shader_t()             : remapping_shader_t("remapping spherical",        "/shader/remapping_cubemap_spherical_vertex_shader",    "", "/shader/remapping_cubemap_spherical_fragment_shader"){}
remapping_equirectangular_shader_t::remapping_equirectangular_shader_t() : remapping_shader_t("remapping equirectangular",  "/shader/remapping_cubemap_equirectangular_vertex_shader","", "/shader/remapping_cubemap_equirectangular_fragment_shader"){}
remapping_identity_shader_t::remapping_identity_shader_t()               : remapping_shader_t("remapping identity",         "/shader/remapping_spherical_spherical_vertex_shader",  "", "/shader/remapping_spherical_spherical_fragment_shader"){}
remapping_cubemap_cubemap_shader_t::remapping_cubemap_cubemap_shader_t() : remapping_shader_t("remapping cubemap cubemap",  "/shader/remapping_spherical_spherical_vertex_shader",  "", "/shader/remapping_cubemap_cubemap_fragment_shader"){}
spherical_approximation_shader_t::spherical_approximation_shader_t()     : rendering_shader_t("spherical approximation",    "/shader/spherical_approximation_vertex_shader",        "", "/shader/spherical_approximation_fragment_shader"){}
perspective_shader_t::perspective_shader_t()                             : rendering_shader_t("perspective",                "/shader/perspective_vertex_shader",                    "", "/shader/perspective_fragment_shader"){}
cubemap_shader_t::cubemap_shader_t()                                     : rendering_shader_t("cubemap",                    "/shader/cubemap_vertex_shader", "/shader/cubemap_geometry_shader","/shader/cubemap_fragment_shader"){}

void shader_t::init(QObject & context)
{
    destroy();
    _program = std::make_unique<QOpenGLShaderProgram>(&context);
    std::string str;
    
    read_shader(IO_UTIL::get_programpath() + _vertex_source_file, str);
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    if (!_geometry_source_file.empty())
    {
        read_shader(IO_UTIL::get_programpath() + _geometry_source_file, str);
        _program->addShaderFromSourceCode(QOpenGLShader::Geometry, str.c_str());
    }
    read_shader(IO_UTIL::get_programpath() + _fragment_source_file, str);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
}

gl_variable_base::gl_variable_base(const std::string& name) : _name(name){}

gl_variable_base::operator GLuint() const {return _id;}

gl_variable<attribute>::gl_variable(const std::string& name) : gl_variable_base(name){}

gl_variable<uniform>::gl_variable(const std::string& name) : gl_variable_base(name){}

bool gl_variable<attribute>::load_location(QOpenGLShaderProgram &program, const std::string& name)
{
    _id = glGetAttribLocation(program, name, _name.c_str());
    return true;
}

bool gl_variable<uniform>::load_location(QOpenGLShaderProgram &program, const std::string& name)
{
    _id = glGetUniformLocation(program, name, _name.c_str());
    return true;
}

void rendering_shader_t::init(QObject & context)
{
    shader_t::init(context);
    _posAttr                    .load_location(*_program, _name);
    _normalAttr                 .load_location(*_program, _name);
    _corAttr                    .load_location(*_program, _name);
    _objToScreenUniform         .load_location(*_program, _name);
    _objToWorldUniform          .load_location(*_program, _name);
    _objToCameraUniform         .load_location(*_program, _name);
    _objToCameraFlowUniform     .load_location(*_program, _name);
    _objToWorldNormalUniform    .load_location(*_program, _name);
    _texKd                      .load_location(*_program, _name);
    _objidUniform               .load_location(*_program, _name);
    _colAmbientUniform          .load_location(*_program, _name);
    _colDiffuseUniform          .load_location(*_program, _name);
    _colSpecularUniform         .load_location(*_program, _name);
}

void spherical_approximation_shader_t::init(QObject & context)
{
    rendering_shader_t::init(context);
    _fovUniform              = _program->uniformLocation("fovUnif");
    _fovCapUniform           = _program->uniformLocation("fovCapUnif");
    _cropUniform             = _program->uniformLocation("cropUnif");
}

void perspective_shader_t::init(QObject & context)      {rendering_shader_t::init(context);}

void cubemap_shader_t::init(QObject & context)
{
    rendering_shader_t::init(context);
    _cbMatrixUniform = _program->uniformLocation("cbMatrix");
}

void remapping_shader_t::init(QObject & context)
{
    shader_t::init(context);
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

void remapping_spherical_shader_t::init(QObject & context)  {remapping_shader_t::init(context);}
void remapping_equirectangular_shader_t::init(QObject & context)  {remapping_shader_t::init(context);}
void remapping_identity_shader_t::init(QObject& context)    {remapping_shader_t::init(context);}
void remapping_cubemap_cubemap_shader_t::init(QObject& context)    {remapping_shader_t::init(context);}

void shader_t::destroy()
{
    if (_program)
    {
        _program = nullptr;
    }
}
