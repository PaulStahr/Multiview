#ifndef SHADER_H
#define SHADER_H

#include <QtGui/QOpenGLShaderProgram>
#include <QObject>

struct shader_t
{
    void destroy();
    QOpenGLShaderProgram *_program = nullptr;
};

struct spherical_approximation_shader_t : shader_t
{
    void init(QObject & context)
    {
    /*    if (_program)
        {
            delete _program;
            _program = nullptr;
        }
        _program = new QOpenGLShaderProgram(&context);
        std::string str = IO_UTIL::read_file("shader/approximation_spherical_vertex_shader");
        _program->addShaderFromSourceCode(QOpenGLShader::Vertex, str.c_str());
        str = IO_UTIL::read_file("shader/approximation_spherical_fragment_shader");
        _program->addShaderFromSourceCode(QOpenGLShader::Fragment, str.c_str());
        _program->link();*/
    }
};

struct perspective_shader_t : shader_t
{
    GLuint _posAttr;
    GLuint _corAttr;
    GLuint _colAttr;
    GLuint _matrixUniform;
    GLuint _objMatrixUniform;
    GLuint _preMatrixUniform;
    GLuint _curMatrixUniform;
    GLuint _postMatrixUniform;
    GLuint _texKd;
    GLuint _objidUniform;
    
    void init(QObject & context);
};

struct remapping_spherical_shader_t: shader_t
{
    GLuint _texAttr[6];
    GLuint _posAttr;
    GLuint _corAttr;

    GLuint _colAttr;
    GLuint _fovUniform;
    GLuint _diffUniform;
    GLuint _depthUniform;
    
    
    void init(QObject & context);
};

struct remapping_identity_shader_t:shader_t
{
    GLuint _texAttr;
    GLuint _posAttr;
    GLuint _corAttr;
    GLuint _diffUniform;
    GLuint _matrixUniform;
    
    void init(QObject & context);
};

#endif
