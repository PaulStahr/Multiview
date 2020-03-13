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

#include <QtGui/QOpenGLShaderProgram>
#include <QObject>

struct shader_t
{
    void destroy();
    QOpenGLShaderProgram *_program = nullptr;
};

struct spherical_approximation_shader_t : shader_t
{
    void init(QObject & context);
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
    //GLuint _texAttr[6];
    GLuint _texAttr;
    GLuint _posAttr;
    GLuint _corAttr;

    GLuint _colAttr;
    GLuint _fovUniform;
    GLuint _viewtypeUniform;
    GLuint _transformUniform;
    GLuint _transformCam[3];
    GLuint _numOverlays;
    GLuint _positionMap;
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
