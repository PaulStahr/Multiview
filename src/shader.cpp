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
        std::cerr << "Warning uniform " <<str<< " in " << name << " not found" << std::endl;
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
    const std::string & fragment_source_file) : shader_t(name, vertex_source_file, geometry_source_file, fragment_source_file),
        _texUniform           ("map"),
        _posAttr              ("posAttr"),
        _corAttr              ("corAttr"),
        _colAttr              ("colAttr"),
        _fovUniform           ("fovUnif"),
        _cropUniform          ("cropUnif"),
        _viewtypeUniform      ("viewtype"),
        _transformUniform     ("transform"),
        _transformColorUniform("transformColor"),
        _transformCam({gl_variable<uniform>("tCam0"),gl_variable<uniform>("tCam1"),gl_variable<uniform>("tCam2")}),
        _positionMaps({gl_variable<uniform>("positionMap0"), gl_variable<uniform>("positionMap1"), gl_variable<uniform>("positionMap2")}),
        _numOverlays          ("numOverlays"),
        _positionMap          ("positionMap")
        
    {}

remapping_spherical_shader_t::remapping_spherical_shader_t()             : remapping_shader_t("remapping spherical",        "/shader/remapping_cubemap_spherical_vertex_shader",    "", "/shader/remapping_cubemap_spherical_fragment_shader"){}
remapping_equirectangular_shader_t::remapping_equirectangular_shader_t() : remapping_shader_t("remapping equirectangular",  "/shader/remapping_cubemap_equirectangular_vertex_shader","", "/shader/remapping_cubemap_equirectangular_fragment_shader"){}
remapping_custom_shader_t::remapping_custom_shader_t()                   : remapping_shader_t("remapping custom",           "/shader/remapping_cubemap_custom_vertex_shader",        "", "/shader/remapping_cubemap_custom_fragment_shader"),
 _pixelCoordinateMap("pixelCoordinateMap"){}
remapping_identity_shader_t::remapping_identity_shader_t()               : remapping_shader_t("remapping identity",         "/shader/remapping_spherical_spherical_vertex_shader",  "", "/shader/remapping_spherical_spherical_fragment_shader"){}
remapping_cubemap_cubemap_shader_t::remapping_cubemap_cubemap_shader_t() : remapping_shader_t("remapping cubemap cubemap",  "/shader/remapping_spherical_spherical_vertex_shader",  "", "/shader/remapping_cubemap_cubemap_fragment_shader"){}
spherical_approximation_shader_t::spherical_approximation_shader_t()     : rendering_shader_t("spherical approximation",    "/shader/spherical_approximation_vertex_shader",        "", "/shader/spherical_approximation_fragment_shader"),
    _fovUniform("fovUnif"),
    _fovCapUniform("fovCapUnif"),
    _cropUniform("cropUnif"){}
perspective_shader_t::perspective_shader_t()                             : rendering_shader_t("perspective",                "/shader/perspective_vertex_shader",                    "", "/shader/perspective_fragment_shader"){}
cubemap_shader_t::cubemap_shader_t()                                     : rendering_shader_t("cubemap",                    "/shader/cubemap_vertex_shader", "/shader/cubemap_geometry_shader","/shader/cubemap_fragment_shader"),
  _cbMatrixUniform("cbMatrix"){}

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

gl_variable_base::operator GLint() const {return _id;}

gl_variable<attribute>::gl_variable(const std::string& name) : gl_variable_base(name){}

gl_variable<uniform>::gl_variable(const std::string& name) : gl_variable_base(name){}

bool gl_variable<attribute>::load_location(QOpenGLShaderProgram &program, const std::string& name)
{
    _id = glGetAttribLocation(program, name, _name.c_str());
    return _id >= 0;
}

bool gl_variable<uniform>::load_location(QOpenGLShaderProgram &program, const std::string& name)
{
    _id = glGetUniformLocation(program, name, _name.c_str());
    return _id >= 0;
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
    _alpha                      .load_location(*_program, _name);
    _objidUniform               .load_location(*_program, _name);
    _colAmbientUniform          .load_location(*_program, _name);
    _colDiffuseUniform          .load_location(*_program, _name);
    _colSpecularUniform         .load_location(*_program, _name);
}

void spherical_approximation_shader_t::init(QObject & context)
{
    rendering_shader_t::init(context);
    _fovUniform.load_location(*_program, _name); 
    _fovCapUniform.load_location(*_program, _name);
    _cropUniform.load_location(*_program, _name);
}

void perspective_shader_t::init(QObject & context)      {rendering_shader_t::init(context);}

void cubemap_shader_t::init(QObject & context)
{
    rendering_shader_t::init(context);
    _cbMatrixUniform.load_location(*_program, _name);
}

void remapping_shader_t::init(QObject & context)
{
    shader_t::init(context);
    _posAttr               .load_location(*_program, _name);
    _corAttr               .load_location(*_program, _name);
    _cropUniform           .load_location(*_program, _name);
    _fovUniform            .load_location(*_program, _name);
    _viewtypeUniform       .load_location(*_program, _name);
    _transformUniform      .load_location(*_program, _name);
    _transformColorUniform .load_location(*_program, _name);
    for (auto & tc : _transformCam){tc.load_location(*_program, _name);}
    for (auto & pm : _positionMaps){pm.load_location(*_program, _name);}
    _numOverlays           .load_location(*_program, _name);
    _positionMap           .load_location(*_program, _name);
    _texUniform            .load_location(*_program, _name);
}

void remapping_spherical_shader_t::init(QObject & context)  {remapping_shader_t::init(context);}
void remapping_equirectangular_shader_t::init(QObject & context)  {remapping_shader_t::init(context);}
void remapping_identity_shader_t::init(QObject& context)    {remapping_shader_t::init(context);}
void remapping_cubemap_cubemap_shader_t::init(QObject& context)    {remapping_shader_t::init(context);}
void remapping_custom_shader_t::init(QObject& context)
{
    remapping_shader_t::init(context);
    _pixelCoordinateMap.load_location(*_program, _name);
}


void shader_t::destroy()
{
    if (_program)
    {
        _program = nullptr;
    }
}
