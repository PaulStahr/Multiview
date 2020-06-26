/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "openglwindow.h"
#include <QtCore/QCoreApplication>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

#include <iostream>
OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent)
    , thread(nullptr)
    , m_animating(false)
    , m_context(0)
    , m_device(0)
{
    _rendering_flag = false;
    setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow(){    delete m_device;}

void OpenGLWindow::render(QPainter *painter){    Q_UNUSED(painter);}

void OpenGLWindow::initialize(){}

void OpenGLWindow::rendering_loop()
{
    while(true)
    {
        {
            std::unique_lock<std::mutex> lck(_mtx);
            _cv.wait(lck,[this](){return this->_rendering_flag.load();});
            this->_rendering_flag = false;
        }
        renderNow();
    }
}

void OpenGLWindow::render()
{
    if (!m_device)
        m_device = new QOpenGLPaintDevice;
    glClearColor(1,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_device->setSize(size());
    QPainter painter(m_device);
    render(&painter);
}

void OpenGLWindow::renderLater()
{
    std::lock_guard<std::mutex> g(_mtx);
    _rendering_flag = true;
    _cv.notify_all();
}

bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
    {
        renderLater();
        return true;
    }
    default:
        return QWindow::event(event);
    }
}

void WorkerThread::run(){
    _window->rendering_loop();
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);
    if (!m_context)
    {
        m_context = new QOpenGLContext(this);
        initializeOpenGLFunctions();
        m_context->setFormat(requestedFormat());
        m_context->create();
        m_context->makeCurrent(this);
        initialize();
        m_context->doneCurrent();
        thread = new WorkerThread(this);
        thread->start();
        moveToThread(thread);
    }
    if (isExposed())
        renderLater();
}

void OpenGLWindow::renderNow()
{
    if (!isExposed())
        return;
    m_context->makeCurrent(this);
    render();
    m_context->swapBuffers(this);
    if (m_animating)
    {
        renderLater();
    }
}

void OpenGLWindow::setAnimating(bool animating)
{
    if (m_animating == animating)
    {
        return;
    }
    m_animating = animating;
    if (animating)
        renderLater();
}
