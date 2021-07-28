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

#define GL_GLEXT_PROTOTYPES

#include <QtGui/QMatrix4x4>
#include <GL/gl.h>
#include <GL/glext.h>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <stdexcept>

#include "qt_gl_util.h"

template <typename T>void glUniform(GLint location, T v0){throw std::runtime_error("not implemented");}

template <> void glUniform(GLint location, GLfloat   v0) {glUniform1f (location, v0);}
template <> void glUniform(GLint location, GLint     v0) {glUniform1i (location, v0);}
template <> void glUniform(GLint location, GLuint    v0) {glUniform1ui(location, v0);}
template <> void glUniform(GLint location, GLboolean v0) {glUniform1i (location, v0);}

/*void setShaderInt       (QOpenGLShaderProgram & prog, GLuint attr, const char *name, GLint value)    {setShaderValue(prog, attr, name, value);}
void setShaderFloat     (QOpenGLShaderProgram & prog, GLuint attr, const char *name, GLfloat value)  {setShaderValue(prog, attr, name, value);}
void setShaderBoolean   (QOpenGLShaderProgram & prog, GLuint attr, const char *name, GLboolean value){setShaderValue(prog, attr, name, value);}*/
