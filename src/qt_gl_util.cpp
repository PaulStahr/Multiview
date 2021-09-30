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

#include <qt5/QtGui/QMatrix4x4>
#include <qt5/QtGui/QMatrix4x3>
#include <GL/gl.h>
#include <GL/glext.h>
#include <qt5/QtGui/QOpenGLTexture>
#include <qt5/QtGui/QOpenGLShaderProgram>
#include <stdexcept>

#include "qt_gl_util.h"

template <typename T>void glUniform(GLint location, T const & v0){throw std::runtime_error("not implemented");}

template <> void glUniform(GLint location, GLfloat    const & v0) {glUniform1f          (location, v0);}
template <> void glUniform(GLint location, GLint      const & v0) {glUniform1i          (location, v0);}
template <> void glUniform(GLint location, GLuint     const & v0) {glUniform1ui         (location, v0);}
template <> void glUniform(GLint location, GLboolean  const & v0) {glUniform1i          (location, v0);}
template <> void glUniform(GLint location, QMatrix4x3 const & v0) {glUniformMatrix4x3fv (location, 1, false, v0.data());}
template <> void glUniform(GLint location, QMatrix4x4 const & v0) {glUniformMatrix4fv (location, 1, false, v0.data());}
