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

#ifndef QT_GL_UTIL_H
#define QT_GL_UTIL_H
#include <cassert>
#include <iostream>
/*void setShaderInt       (QOpenGLShaderProgram & prog, GLuint attr, const char *name, GLint value);
void setShaderFloat     (QOpenGLShaderProgram & prog, GLuint attr, const char *name, GLfloat value);
void setShaderBoolean   (QOpenGLShaderProgram & prog, GLuint attr, const char *name, GLboolean value);*/

template <typename T>void glUniform(GLint location, T v0);
/*
template <typename T>
void setShaderValue(QOpenGLShaderProgram & prog, GLuint attr, const char *name, T value)
{
    //prog.setUniformValue(attr, value);
    GLint dloc = prog.uniformLocation(name);
    std::cout << attr << ' ' << dloc << std::endl;
    assert (attr == dloc);
    if (dloc != -1){glUniform(dloc, value);}
}*/

#endif
