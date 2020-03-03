#include "shader.h"
#include "io_util.h"

void perspective_shader_t::init(QObject & context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str = IO_UTIL::read_file("shader/perspective_vertex_shader");
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    str = IO_UTIL::read_file("shader/perspective_fragment_shader");
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    
    _posAttr = _program->attributeLocation("posAttr");
    _corAttr = _program->attributeLocation("corAttr");
    _colAttr = _program->attributeLocation("colAttr");
    _matrixUniform = _program->uniformLocation("matrix");
    _objMatrixUniform = _program->uniformLocation("objMatrix");
    _preMatrixUniform = _program->uniformLocation("preMatrix");
    _curMatrixUniform = _program->uniformLocation("curMatrix");
    _postMatrixUniform = _program->uniformLocation("postMatrix");
    _texKd = _program->uniformLocation("mapKd");
    _objidUniform = _program->uniformLocation("objid");
}

void remapping_spherical_shader_t::init(QObject & context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str = IO_UTIL::read_file("shader/remapping_spherical_vertex_shader");
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    
    str = IO_UTIL::read_file("shader/remapping_spherical_fragment_shader");
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    
    _posAttr = _program->attributeLocation("posAttr");
    _corAttr = _program->attributeLocation("corAttr");
    _fovUniform = _program->attributeLocation("fovUnif");
    _diffUniform = _program->attributeLocation("diff");
    for (size_t i = 0; i < 6; ++i)
    {
        std::string str = std::string("map") + std::to_string(i);
        _texAttr[i] = _program->uniformLocation(str.c_str());
    }
}

void remapping_identity_shader_t::init(QObject& context)
{
    destroy();
    _program = new QOpenGLShaderProgram(&context);
    std::string str = IO_UTIL::read_file("shader/remapping_identity_vertex_shader");
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
    
    str = IO_UTIL::read_file("shader/remaping_identity_fragment_shader");
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
    _program->link();
    
    _posAttr = _program->attributeLocation("posAttr");
    _corAttr = _program->attributeLocation("corAttr");
    _diffUniform = _program->attributeLocation("diff");
    _texAttr = _program->uniformLocation("map");
    _matrixUniform = _program->uniformLocation("matrix");
}

void shader_t::destroy()
{
    if (_program)
    {
        delete _program;
        _program = nullptr;
    }
}
