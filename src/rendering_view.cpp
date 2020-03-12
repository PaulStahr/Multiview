#include "rendering_view.h"

void print_models(objl::Loader & Loader, std::ostream & file)
{
    for (size_t i = 0; i < Loader.LoadedMeshes.size(); i++)
    {
        // Copy one of the loaded meshes to be our current mesh
        objl::Mesh curMesh = Loader.LoadedMeshes[i];

        // Print Mesh Name
        file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

        /*// Print Vertices
        file << "Vertices:\n";

        // Go through each vertex and print its number,
        //  position, normal, and texture coordinate
        for (size_t j = 0; j < curMesh.Vertices.size(); j++)
        {
            file << "V" << j << ": " <<
                "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
                "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
                "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
        }

        // Print Indices
        file << "Indices:\n";

        // Go through every 3rd index and print the
        //	triangle that these indices represent
        for (size_t j = 0; j < curMesh.Indices.size(); j += 3)
        {
            file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
        }*/

        // Print Material
        file << "Material: " << curMesh.MeshMaterial.name << "\n";
        file << "Ambient Color: "  << curMesh.MeshMaterial.Ka.x() << ", " << curMesh.MeshMaterial.Ka.y() << ", " << curMesh.MeshMaterial.Ka.z() << "\n";
        file << "Diffuse Color: "  << curMesh.MeshMaterial.Kd.x() << ", " << curMesh.MeshMaterial.Kd.y() << ", " << curMesh.MeshMaterial.Kd.z() << "\n";
        file << "Specular Color: " << curMesh.MeshMaterial.Ks.x() << ", " << curMesh.MeshMaterial.Ks.y() << ", " << curMesh.MeshMaterial.Ks.z() << "\n";
        file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
        file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
        file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
        file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
        file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
        file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
        file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
        file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
        file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

        // Leave a space to separate from the next mesh
        file << "\n";
    }
}

void load_meshes(mesh_object_t & mesh)
{
    if (mesh._vbo.size() == 0)
    {
        std::cout << "load meshes" << std::endl;
        mesh._vbo.resize(mesh._loader.LoadedMeshes.size());
        glGenBuffers(mesh._vbo.size(), mesh._vbo.data());
        mesh._vbi.resize(mesh._loader.LoadedMeshes.size());
        glGenBuffers(mesh._vbi.size(), mesh._vbi.data());
        for (size_t i = 0; i < mesh._vbo.size(); ++i)
        {
            std::cout << "load mesh " << i << std::endl;
            objl::Mesh const & curMesh = mesh._loader.LoadedMeshes[i];
            glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(objl::Vertex) * curMesh.Vertices.size(), curMesh.Vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._vbi[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * curMesh.Indices.size(), curMesh.Indices.data(), GL_STATIC_DRAW);
        }
    }
}

void load_textures(mesh_object_t & mesh)
{
    for (size_t i = 0; i < mesh._loader.LoadedMeshes.size(); ++i)
    {
        if (mesh._loader.LoadedMeshes[i].MeshMaterial.map_Ka != "" && mesh._tex_Ka == nullptr)
        {
            std::cout << "ka" << std::endl;
            mesh._tex_Ka = new QOpenGLTexture(QImage(QString(mesh._loader.LoadedMeshes[i].MeshMaterial.map_Ka.c_str())));
        }
        if (mesh._loader.LoadedMeshes[i].MeshMaterial.map_Kd != "" && mesh._tex_Kd == nullptr)
        {
            std::cout << "kd" << std::endl;
            QImage img;
            if (!img.load(mesh._loader.LoadedMeshes[i].MeshMaterial.map_Kd.c_str()))
            {
                std::cout << "error, can't load image " << mesh._loader.LoadedMeshes[i].MeshMaterial.map_Kd.c_str() << std::endl;
            }
            std::cout << img.width() << ' ' << img.height() << std::endl;
            mesh._tex_Kd   = new QOpenGLTexture(img.mirrored());
        }
    }
    //print_models(_loader, std::cout);
}

void destroy(mesh_object_t & mesh)
{
    glDeleteBuffers(mesh._vbo.size(), mesh._vbo.data());
    mesh._vbo.clear();
    glDeleteBuffers(mesh._vbi.size(), mesh._vbi.data());
    mesh._vbi.clear();
    if (mesh._tex_Kd != nullptr)
    {
        mesh._tex_Kd -> destroy();
        delete mesh._tex_Kd;
        mesh._tex_Kd = nullptr;
    }
}

std::ostream & print_gl_errors(std::ostream & out, std::string const & message, bool endl)
{
    GLenum error = glGetError();
    if (error == 0)
    {
        return out;
    }
    out << message << error;
    while (true)
    {
        error = glGetError();
        if (error == 0)
        {
            return endl ? (out << std::endl) : out;
        }
        out << ' '<< error;
    }
    return out;
}

rotation_t smoothed(std::map<size_t, rotation_t> const & map, size_t frame, size_t smoothing)
{
    rotation_t result(0,0,0,0);
    for (size_t i = 0; i < smoothing * 2 + 1; ++i)
    {
        rotation_t const & tmp = interpolated(map, i + frame - smoothing);
        if (dot(result, tmp) < 0)
        {
            result -= tmp;
        }
        else
        {
            result += tmp;
        }
    }
    //std::cout << result << std::endl;
    return result.normalized();
}

std::string getGlErrorString()
{
    std::string res;
    while (true)
    {
        GLenum error = glGetError();
        if (error == 0)
        {
            return res;
        }
        res += " ";
        res += std::to_string(error);
    }
}

void render_to_screenshot(screenshot_handle_t & current, GLuint **cubemaps, size_t loglevel, scene_t & scene, remapping_spherical_shader_t & remapping_shader)
{
    if (loglevel > 2)
    {
        std::cout << "take screenshot " << current._camera << std::endl;
    }
    camera_t * cam = scene.get_camera(current._camera);
    
    GLuint screenshotFramebuffer = 0;
    glGenFramebuffers(1, &screenshotFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screenshotFramebuffer);
    size_t swidth = current._width;
    size_t sheight = current._height;

    GLuint screenshotTexture;
    glGenTextures(1, &screenshotTexture);
    glBindTexture(GL_TEXTURE_2D, screenshotTexture);
    switch(current._type)
    {
    case 0:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, swidth, sheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);break;
    case 1:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, swidth, sheight, 0,GL_RGBA, GL_FLOAT, 0);break;
    case 2:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, swidth, sheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);break;
    case 3:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, swidth, sheight, 0,GL_RGBA, GL_FLOAT, 0);break;
    case 4:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, swidth, sheight, 0,GL_RGBA, GL_FLOAT, 0);break;//TODO
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, swidth, sheight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenshotTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    GLuint framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Error, no framebuffer(" + std::to_string(__LINE__) + "):" + std::to_string(framebufferStatus));

    glViewport(0,0,swidth,sheight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);

    render_cubemap(cubemaps[current._type] + 6 * std::distance(scene._cameras.data(), cam), remapping_shader);

    if (current._channels == 0)
    {
        switch(current._type)
        {
        case 0:current._channels = 3;break;
        case 1:current._channels = 3;break;
        case 2:current._channels = 1;break;
        case 3:current._channels = 2;break;
        case 4:current._channels = 2;break;
        }
    }
    if (current._datatype == GL_FLOAT)
    {
        float *pixels = new float[swidth*sheight*current._channels];
        glBindTexture(GL_TEXTURE_2D, screenshotTexture);
        switch(current._channels)
        {
        case 1:glGetTexImage(GL_TEXTURE_2D, 0, GL_R, GL_FLOAT, pixels);break;
        case 2:glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, pixels);break;
        case 3:glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);break;
        }
        current._data = pixels;
    }
    else
    {
        uint8_t *pixels = new uint8_t[swidth*sheight*current._channels];
        glBindTexture(GL_TEXTURE_2D, screenshotTexture);
        switch(current._channels)
        {
        case 1:glGetTexImage(GL_TEXTURE_2D, 0, GL_R, GL_UNSIGNED_BYTE, pixels);break;
        case 2:glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);break;
        case 3:glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);break;
        }
        current._data = pixels;
    }
    current._cv.notify_one();
    glDeleteTextures(1, &screenshotTexture);
    glDeleteRenderbuffers(1, &depthrenderbuffer);
    glDeleteFramebuffers(1, &screenshotFramebuffer);
}

vec3f_t smoothed(std::map<size_t, vec3f_t> const & map, size_t frame, size_t smoothing)
{
    vec3f_t result(0,0,0);
    for (size_t i = 0; i < smoothing * 2 + 1; ++i)
    {
        result += interpolated(map, i + frame - smoothing);
    }
    return result /= smoothing * 2 + 1;
}

void transform_matrix(object_t const & obj, QMatrix4x4 & matrix, size_t mt_frame, size_t t_smooth, size_t mr_frame, size_t r_smooth)
{
    if (obj._key_pos.size() != 0)
    {
        vec3f_t pos = smoothed(obj._key_pos, mt_frame, t_smooth);
        matrix.translate(pos.x(), pos.y(), pos.z());
        //std::cout << pos ;
    }
    if (obj._key_rot.size() != 0)
    {
        //std::cout << "found" << std::endl;
        matrix.rotate(to_qquat(smoothed(obj._key_rot, mr_frame, r_smooth)));
        //std::cout << obj._key_rot.lower_bound(m_frame)->second << ' '<< std::endl;
    }
    matrix *= obj._transformation;
}

void render_cubemap(GLuint *renderedTexture, remapping_spherical_shader_t & remapping_shader)
{

    /*for (size_t i = 0; i < 6; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderedTexture[i]);
        glUniform1i(window->remapping_shader._texAttr[i], i);
    }*/
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *renderedTexture);
    glUniform1i(remapping_shader._texAttr, 0);
    
    glVertexAttribPointer(remapping_shader._posAttr, 2, GL_FLOAT, GL_FALSE, 0, g_quad_vertex_buffer_data);
    glVertexAttribPointer(remapping_shader._corAttr, 2, GL_FLOAT, GL_FALSE, 0, g_quad_texture_coords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

GLuint render_to_pixel_buffer(screenshot_handle_t & current, GLuint **cubemaps, size_t loglevel, scene_t & scene, bool debug, remapping_spherical_shader_t & remapping_shader)
{
    if (debug)
    {
        print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
    }
    if (loglevel > 2)
    {
        std::cout << "take screenshot " << current._camera << std::endl;
    }
    camera_t * cam = scene.get_camera(current._camera);
    
    GLuint screenshotFramebuffer = 0;
    glGenFramebuffers(1, &screenshotFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screenshotFramebuffer);
    size_t swidth = current._width;
    size_t sheight = current._height;

    GLuint screenshotTexture;
    glGenTextures(1, &screenshotTexture);
    glBindTexture(GL_TEXTURE_2D, screenshotTexture);
    switch(current._type)
    {
        case 0:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, swidth, sheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);break;
        case 1:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, swidth, sheight, 0,GL_RGBA, GL_FLOAT, 0);break;
        case 2:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, swidth, sheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);break;
        case 3:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, swidth, sheight, 0,GL_RGBA, GL_FLOAT, 0);break;
        case 4:glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, swidth, sheight, 0,GL_RGBA, GL_FLOAT, 0);break;//TODO
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, swidth, sheight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenshotTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    GLuint framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Error, no framebuffer(" + std::to_string(__LINE__) + "):" + std::to_string(framebufferStatus));

    glViewport(0,0,swidth,sheight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);
    render_cubemap(cubemaps[current._type] + 6 * std::distance(scene._cameras.data(), cam), remapping_shader);

    if (current._channels == 0)
    {
        switch(current._type)
        {
            case 0:current._channels = 3;break;
            case 1:current._channels = 3;break;
            case 2:current._channels = 1;break;
            case 3:current._channels = 2;break;
            case 4:current._channels = 2;break;
        }
    }
    
    GLuint pbo_userImage;
    glGenBuffers(1, &pbo_userImage);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_userImage);
    glBufferData(GL_PIXEL_PACK_BUFFER, current.size(), 0, GL_STREAM_READ);
    glBindTexture(GL_TEXTURE_2D, screenshotTexture);
    if (debug)
    {
        print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
    }
    //
    if (current._datatype == GL_FLOAT)
    {
        switch(current._channels)
        {
            case 1:glGetTexImage(GL_TEXTURE_2D, 0, GL_R,   GL_FLOAT, 0);break;
            case 2:glGetTexImage(GL_TEXTURE_2D, 0, GL_RG,  GL_FLOAT, 0);break;
            case 3:glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, 0);break;
            /*case 1:glReadPixels(0, 0, swidth, sheight, GL_R,   GL_FLOAT, 0);break;
            case 2:glReadPixels(0, 0, swidth, sheight, GL_RG,  GL_FLOAT, 0);break;
            case 3:glReadPixels(0, 0, swidth, sheight, GL_RGB, GL_FLOAT, 0);break;*/
        }
    }
    else
    {
        switch(current._channels)
        {
            case 1:glGetTexImage(GL_TEXTURE_2D, 0, GL_R,   GL_UNSIGNED_BYTE, 0);break;
            case 2:glGetTexImage(GL_TEXTURE_2D, 0, GL_RG,  GL_UNSIGNED_BYTE, 0);break;
            case 3:glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);break;
        }
    }
    glDeleteTextures(1, &screenshotTexture);
    glDeleteRenderbuffers(1, &depthrenderbuffer);
    glDeleteFramebuffers(1, &screenshotFramebuffer);
    current._bufferAddress = pbo_userImage;
    if (debug)
    {
        print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
    }
    return pbo_userImage;
}

TriangleWindow::TriangleWindow()
{
    QObject::connect(this, SIGNAL(renderLaterSignal()), this, SLOT(renderLater()));
    QObject::connect(this, SIGNAL(renderNowSignal()), this, SLOT(renderNow()));
    session._m_frame = 100000;
    
    session._updateListener.emplace_back([this](SessionUpdateType sut){
        if (sut == UPDATE_ANIMATING)
        {
            this->setAnimating(this->session._animating == REDRAW_ALWAYS);
        }
        else if (sut == UPDATE_REDRAW)
        {
            this->renderNowSignal();
        }
        else if (sut == UPDATE_SESSION)
        {
            if (this->session._animating == REDRAW_AUTOMATIC)
            {
                this->renderNowSignal();
            }
        }
    });
}

void TriangleWindow::initialize()
{
    perspective_shader.init(*this);
    remapping_shader.init(*this);
    approximation_shader.init(*this);
    GLint maxColorAttachememts = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachememts);
    std::cout << "max attachments:" << maxColorAttachememts << std::endl;
    GLubyte const* msg = glGetString(GL_EXTENSIONS);
    std::cout << "extensions:" << msg << std::endl;
    image_io_init();
}

void copy_pixel_buffer_to_screenshot(screenshot_handle_t & current, bool debug)
{
    if (debug)
    {
        print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, current._bufferAddress);
    if (current._datatype == GL_FLOAT)
    {
        float *pixels = new float[current.num_elements()];
        float* ptr = static_cast<float*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY)); //One of these: -lGLEW -lglut -lGLU -lGLX -lSDL
        if (ptr == nullptr)
        {
            throw std::runtime_error("map buffer returned null " + getGlErrorString());
        }
        std::copy(ptr, ptr + current.num_elements(), pixels);
        current._data = pixels;
    }
    else
    {
        uint8_t *pixels = new uint8_t[current.num_elements()];
        uint8_t* ptr = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
        if (ptr == nullptr)
        {
            throw std::runtime_error("map buffer returned null " + getGlErrorString());
        }
        std::copy(ptr, ptr + current.num_elements(), pixels);
        current._data = pixels;
    }
    current._cv.notify_one();
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glDeleteBuffers(1, &current._bufferAddress);
    current._bufferAddress = 0;
}


void setShaderBoolean(QOpenGLShaderProgram & prog, GLuint attr, const char *name, bool value)
{
    prog.setUniformValue(attr, static_cast<GLboolean>(value));
    {
        GLint dloc = prog.uniformLocation(name);
        if (dloc != -1)
        {
            glUniform1i(dloc, static_cast<GLboolean>(value));
        }
    }
}

void TriangleWindow::render()
{
    if (destroyed)
    {
        return;
    }
    bool show_arrows = session._show_arrows;
    bool show_curser = session._show_curser;
    float fov = session._fov;
    size_t loglevel = session._loglevel;
    size_t smoothing = session._smoothing;
    int32_t m_frame = session._m_frame;
    std::string const & show_only = session._show_only;
    QPoint curser_pos = mapFromGlobal(QCursor::pos());
    //std::cout << p.x() << ' ' << p.y()  << std::endl;
    const high_res_clock current_time = std::chrono::high_resolution_clock::now();
    //std::cout << "fps " << CLOCKS_PER_SEC / float( current_time - last_rendertime ) << std::endl;

    if (session._loglevel > 5)
    {
        std::cout << "start render" << std::endl;
    }
    if (session._reload_shader)
    {
        initialize();
        session._reload_shader = false;
    }
    scene_t & scene = session._scene;
    size_t resolution = session._preresolution;
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale , height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 cubemap_transforms[6];
    for (size_t i = 0; i <6; ++i)
    {
        cubemap_transforms[i].perspective(90.0f, 1.0f/1.0f, 0.1f, 1000.0f);
    }
    //settransform left_eye rot 0.7 0.7 0 0
    cubemap_transforms[0].rotate(90, 0, 0, 1);
    cubemap_transforms[0].scale(1,-1,1);
    cubemap_transforms[1].rotate(180, 1, 0, 0);
    cubemap_transforms[1].rotate(270, 0, 0, 1);
    cubemap_transforms[1].scale(1,-1,1);
    cubemap_transforms[2].rotate(270, 0, 1, 0);
    cubemap_transforms[2].scale(1,-1,1);
    cubemap_transforms[3].rotate(90, 0, 1, 0);
    cubemap_transforms[3].scale(1, 1, -1);
    cubemap_transforms[4].rotate(270, 0, 0, 1);
    cubemap_transforms[4].rotate(90, 1, 0, 0);
    cubemap_transforms[4].scale(-1, 1, 1);
    cubemap_transforms[5].rotate(270, 0, 0, 1);
    cubemap_transforms[5].rotate(270, 1, 0, 0);
    cubemap_transforms[5].scale(-1,1,1);
    
    /*if (session._m_frame < 6)
    {
        cubemap_transforms[session._m_frame].scale(0,0,0);
    }*/
    /*cubemap_transforms[0].rotate(180, 0, 0, 1);
    cubemap_transforms[1].rotate(180, 1, 0, 0);
    cubemap_transforms[1].scale(1,-1,1);
    cubemap_transforms[2].rotate(270, 0, 1, 0);
    cubemap_transforms[2].scale(1,-1,1);
    cubemap_transforms[3].rotate(90, 0, 1, 0);
    cubemap_transforms[4].rotate(270, 0, 0, 1);
    cubemap_transforms[4].rotate(90, 1, 0, 0);
    cubemap_transforms[5].rotate(270, 0, 0, 1);
    cubemap_transforms[5].rotate(270, 1, 0, 0);
    cubemap_transforms[5].scale(-1,1,1);
    */
    
    for (size_t i = 0; i < 6; ++i)
    {
        cubemap_transforms[i].rotate(-90,1,0,0);
        cubemap_transforms[i].rotate(90,0,0,1);
    }
    size_t num_cams = session._scene._cameras.size();
    size_t num_views = static_cast<size_t>(session._show_raytraced) + static_cast<size_t>(session._show_position) + static_cast<size_t>(session._show_index) + static_cast<size_t>(session._show_flow) + static_cast<size_t>(session._show_depth);
    views.clear();

    marker.clear();
    QPointF curserViewPos;
    if (num_cams != 0 && num_views != 0)
    {
        curserViewPos.setX((curser_pos.x() % (width() / num_cams))/static_cast<float>(width() / num_cams));
        curserViewPos.setY((curser_pos.y() % (height() / num_views))/static_cast<float>(height() / num_views));
        if (loglevel > 5)
        {
            std::cout << curserViewPos.x() << ' ' << curserViewPos.y() << '\t';
        }
    }
    std::vector<wait_for_rendered_frame*> & wait_for_rendered_frame_handles = session._wait_for_rendered_frame_handles;

    scene._mtx.lock();

    size_t num_textures = 6 * num_cams;
    GLuint renderedTexture[num_textures];
    glGenTextures(num_textures, renderedTexture);
    GLuint renderedFlowTexture[num_textures];
    glGenTextures(num_textures, renderedFlowTexture);
    GLuint renderedPositionTexture[num_textures];
    glGenTextures(num_textures, renderedPositionTexture);
    GLuint renderedIndexTexture[num_textures];
    glGenTextures(num_textures, renderedIndexTexture);
    GLuint renderedDepthTexture[num_textures];
    //glGenRenderbuffers(num_textures, renderedDepthTexture);
    glGenTextures(num_textures, renderedDepthTexture);
    num_textures = 6 * num_cams;
    for (size_t c = 0; c < scene._cameras.size(); ++c)
    {
        camera_t & cam = scene._cameras[c];
        size_t x = c * width() / num_cams;
        size_t w = width() / num_cams;
        size_t h = height()/num_views;
        size_t i = 0;
        if (session._show_raytraced)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedTexture + c * 6, x, y, w, h, false, false}));
        }
        if (session._show_position)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedPositionTexture + c * 6, x, y, w, h, false, false}));
        }
        if (session._show_index)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedIndexTexture + c * 6, x, y, w, h, false, false}));
        }
        if (session._show_flow)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedFlowTexture + c * 6, x, y, w, h, true, false}));
        }
        if (session._show_depth)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedDepthTexture + c * 6, x, y, w, h, false, true}));
        }
    }

    camera_transformations.clear();
    for (size_t c = 0; c < scene._cameras.size(); ++c)
    {
        camera_t & cam = scene._cameras[c];
        camera_transformations.emplace_back();
        QMatrix4x4 &cam_transform_cur = camera_transformations.back();
        transform_matrix(cam, cam_transform_cur, m_frame, smoothing, m_frame, smoothing);
        cam_transform_cur = cam_transform_cur.inverted();
    }
    bool difftrans = session._difftrans;
    bool diffrot = session._diffrot;
    bool diffobj = session._diffobjects;
    int32_t diffbackward = session._diffbackward;
    int32_t diffforward = session._diffforward;
    
    if (session._debug)
    {
        print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
    }
    for (size_t c = 0; c < scene._cameras.size(); ++c)
    {
        camera_t & cam = scene._cameras[c];
        QMatrix4x4 &cam_transform_cur = camera_transformations[c];
        QMatrix4x4 cam_transform_pre;
        QMatrix4x4 cam_transform_post;
        transform_matrix(cam, cam_transform_pre,  m_frame + (difftrans ? diffbackward : 0), smoothing, m_frame + (diffrot ? diffbackward : 0), smoothing);
        transform_matrix(cam, cam_transform_post, m_frame + (difftrans ? diffforward  : 0), smoothing, m_frame + (diffrot ? diffforward  : 0), smoothing);
        cam_transform_pre = cam_transform_pre.inverted();
        cam_transform_post = cam_transform_post.inverted();

        GLuint FramebufferName = 0;
        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        // The depth buffer
        //GLuint depthrenderbuffer;
        //glGenRenderbuffers(1, &depthrenderbuffer);
        // The texture we're going to render to
        for (size_t f = 0; f < 6; ++f)
        {    
            glBindTexture(GL_TEXTURE_CUBE_MAP, renderedTexture[c * 6]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0,GL_RGBA, resolution, resolution, 0,GL_BGRA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_CUBE_MAP, renderedFlowTexture[c * 6]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0,GL_RGB16F, resolution, resolution, 0,GL_BGR, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_CUBE_MAP, renderedPositionTexture[c * 6]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0,GL_RGBA16F, resolution, resolution, 0,GL_BGRA, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_CUBE_MAP, renderedIndexTexture[c * 6]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0,GL_RGBA, resolution, resolution, 0,GL_BGRA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_CUBE_MAP, renderedDepthTexture[c * 6]);
            glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0, GL_DEPTH_COMPONENT32F, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
            //glTexImage2D (GL_TEXTURE_2D, 0,GL_R32F, resolution, resolution, 0,GL_RED, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        for (size_t f = 0; f < 6; ++f)
        {
            if (f == 5 && fov < 120)
            {
                continue;
            }
            if (f != 4 && fov <= 45)
            {
                continue;
            }
            
            // glBindRenderbuffer(GL_RENDERBUFFER, renderedDepthTexture[f + c * 6]);
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, resolution, resolution);
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderedDepthTexture[f + c * 6]);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, renderedTexture[c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, renderedFlowTexture[c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, renderedPositionTexture[c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, renderedIndexTexture[c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, renderedDepthTexture[c * 6], 0 );
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, renderedDepthTexture[f + c * 6], 0);

            // Set the list of draw buffers.
            GLenum DrawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
            glDrawBuffers(4, DrawBuffers);

            //std::cout << "bind framebuffer" << std::endl;
            // Always check that our framebuffer is ok
            GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Error, no framebuffer(" + std::to_string(__LINE__) + "):" + std::to_string(frameBufferStatus));

            //glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
            glViewport(0,0,resolution,resolution);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            perspective_shader._program->bind();

            QMatrix4x4 view_matrix = cubemap_transforms[f] * cam_transform_cur;

            for (size_t i = 0; i < scene._objects.size(); ++i)
            {
                mesh_object_t & mesh = scene._objects[i];
                remapping_shader._program->setUniformValue(perspective_shader._objidUniform, static_cast<GLint>(mesh._id));
                GLint loc = perspective_shader._program->uniformLocation("objid");
                if (loc != -1)
                {
                    glUniform1f(loc, static_cast<GLint>(mesh._id));
                }
                load_meshes(mesh);
                QMatrix4x4 object_transform_pre;
                QMatrix4x4 object_transform_cur;
                QMatrix4x4 object_transform_post;
                transform_matrix(mesh, object_transform_pre, m_frame + (diffobj ? diffbackward : 0), smoothing, m_frame + (diffobj ? diffbackward : 0), smoothing);
                transform_matrix(mesh, object_transform_cur, m_frame, smoothing, m_frame, smoothing);
                transform_matrix(mesh, object_transform_post, m_frame + (diffobj ? diffforward : 0), smoothing, m_frame + (diffobj ? diffforward : 0), smoothing);
                perspective_shader._program->setUniformValue(perspective_shader._preMatrixUniform, cam_transform_pre * object_transform_pre);
                perspective_shader._program->setUniformValue(perspective_shader._curMatrixUniform, cam_transform_cur * object_transform_cur);
                perspective_shader._program->setUniformValue(perspective_shader._postMatrixUniform, cam_transform_post * object_transform_post);
                perspective_shader._program->setUniformValue(perspective_shader._matrixUniform, view_matrix * object_transform_cur);
                if (camera_transformations.size() == 2)
                {
                    perspective_shader._program->setUniformValue(perspective_shader._objMatrixUniform, camera_transformations[1 - c] * object_transform_cur);
                }
                else
                {
                    perspective_shader._program->setUniformValue(perspective_shader._objMatrixUniform, object_transform_cur);
                }

                objl::Loader & Loader = mesh._loader;
                load_textures(mesh);
                //GLUint orig;
                if (mesh._tex_Kd != nullptr)
                {
                    glActiveTexture(GL_TEXTURE0);
                    mesh._tex_Kd->bind();
                    glUniform1i(perspective_shader._texKd, 0);
                }
                for (size_t i = 0; i < Loader.LoadedMeshes.size(); ++i)
                {
                    objl::Mesh const & curMesh = Loader.LoadedMeshes[i];
                    glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo[i]);

                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(perspective_shader._posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), BUFFER_OFFSET(0));
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(perspective_shader._colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), BUFFER_OFFSET(3 * sizeof(float)));
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(perspective_shader._corAttr, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), BUFFER_OFFSET(6 * sizeof(float)));
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._vbi[i]);

                    glDrawElements( GL_TRIANGLES, curMesh.Indices.size(), GL_UNSIGNED_INT, (void*)0);

                    glDisableVertexAttribArray(2);
                    glDisableVertexAttribArray(1);
                    glDisableVertexAttribArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                }
                if (mesh._tex_Kd != nullptr)
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }

            perspective_shader._program->release();
        }
        //glDeleteRenderbuffers(1, &depthrenderbuffer);
        glDeleteFramebuffers(1, &FramebufferName);
    }


    remapping_shader._program->bind();
    remapping_shader._program->setUniformValue(remapping_shader._fovUniform, static_cast<GLfloat>(fov / 90. * M_PI));
    {
        GLint loc = remapping_shader._program->uniformLocation("fovUnif");
        if (loc != -1)
        {
            glUniform1f(loc, static_cast<GLfloat>(fov * (M_PI/ 180.)));
        }
    }
    if (session._debug)
    {
        print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
    }
    
    GLuint *texturePointer[5] = {renderedTexture, renderedPositionTexture, renderedDepthTexture, renderedFlowTexture, renderedIndexTexture};
    QVector4D curser_3d;
    std::vector<vec2f_t> curser_flow;
    
    std::vector<screenshot_handle_t*> arrow_handles;
    size_t arrow_lines = 16;
    if (show_arrows)
    {
        arrow_handles.reserve(num_cams);
        setShaderBoolean(*remapping_shader._program, remapping_shader._diffUniform, "diff", true);
        setShaderBoolean(*remapping_shader._program, remapping_shader._depthUniform, "depth", false);
        for (size_t icam = 0; icam < num_cams; ++icam)
        {
            if (session._debug)
            {
                print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
            }
            screenshot_handle_t & current = *(new screenshot_handle_t);
            arrow_handles.emplace_back(&current);
            current._width = arrow_lines;
            current._height = arrow_lines;
            current._channels = 2;
            current._type = VIEWTYPE_FLOW;
            current._ignore_nan = true;
            current._datatype = GL_FLOAT;
            current._data = nullptr;
            current._error_code = 0;
            current._camera = scene._cameras[icam]._name;
            render_to_pixel_buffer(current, texturePointer, loglevel, scene, session._debug, remapping_shader);
            if (session._debug)
            {
                print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);
            }
        }
    }
    
    for (size_t read = 0; read < scene._screenshot_handles.size(); ++read)
    {
        screenshot_handle_t & current = *scene._screenshot_handles[read];

        bool flow = current._type == VIEWTYPE_FLOW;
        bool depth = current._type == VIEWTYPE_DEPTH;
        setShaderBoolean(*remapping_shader._program, remapping_shader._diffUniform, "diff", flow);
        setShaderBoolean(*remapping_shader._program, remapping_shader._depthUniform, "depth", depth);
        camera_t *cam = scene.get_camera(current._camera);
        if (cam == nullptr)
        {
            std::cout << "error, camera doesn't exist" << std::endl;
            current._error_code = 1;
        }
        else
        {
            if (current._ignore_nan || !contains_nan(camera_transformations[std::distance(scene._cameras.data(), cam)]) || !contains_nan(camera_transformations[1 - std::distance(scene._cameras.data(), cam)]))
            {
                std::cout << "rendering_screenshot " << read << std::endl;
                render_to_pixel_buffer(current, texturePointer, loglevel, scene, session._debug, remapping_shader);
                //render_to_screenshot(current, texturePointer, loglevel, scene);
                std::cout << "rendered_screenshot" << std::endl;
            }
            else
            {
                current._error_code = 1;
            }
        }
    }

    screenshot_handle_t curser_handle;
    if (show_curser && num_cams != 0)
    {
        setShaderBoolean(*remapping_shader._program, remapping_shader._diffUniform, "diff", false);
        setShaderBoolean(*remapping_shader._program, remapping_shader._depthUniform, "depth", false);
        curser_handle._width = 1024;
        curser_handle._height = 1024;
        curser_handle._channels = 3;
        curser_handle._type = VIEWTYPE_POSITION;
        curser_handle._ignore_nan = true;
        curser_handle._datatype = GL_FLOAT;
        curser_handle._data = nullptr;
        curser_handle._error_code = 0;
        curser_handle._camera = scene._cameras[clamp(curser_pos.x() * num_cams / width(), size_t(0), num_cams-1)]._name;
        render_to_pixel_buffer(curser_handle, texturePointer, loglevel, scene, session._debug, remapping_shader);
        //render_to_screenshot(curser_handle, texturePointer, loglevel, scene);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);  
    
    for (view_t & view : views)
    {
        setShaderBoolean(*remapping_shader._program, remapping_shader._diffUniform, "diff", view._diff);
        setShaderBoolean(*remapping_shader._program, remapping_shader._depthUniform, "depth", view._depth);

        glViewport(view._x, view._y, view._width, view._height);

        render_cubemap(view._cubemap_texture, remapping_shader);
    }
    
    if (show_arrows)
    {
        for (size_t icam = 0; icam < num_cams; ++icam)
        {
            screenshot_handle_t & current = *arrow_handles[icam];
            copy_pixel_buffer_to_screenshot(current, session._debug);
            //render_to_screenshot(current, texturePointer, loglevel, scene);
            
            float *data = reinterpret_cast<float*>(current._data);
            
            size_t sy = current._height - static_cast<size_t>(curserViewPos.y() * current._height);
            size_t sx = static_cast<size_t>(curserViewPos.x() * current._width);
            if (sx < current._width && sy < current._height)
            {
                size_t index = 2 * (sy * current._width + sx);
                //std::cout << 2 * (sy * 1024 + sx) << '=' << 2 << "*("<<sy <<'*'<< 1024 <<'+'<< sx<<')' << data[index] << ' ' << data[index + 1]<< std::endl;
                curser_flow.emplace_back(data[index],data[index + 1]);
            }
            
            for (size_t y = 0; y < current._height; ++y)
            {
                for (size_t x = 0; x < current._width; ++x)
                {
                    float xf = (static_cast<float>(x) + 0.5) / current._width;
                    float yf = (static_cast<float>(y) + 0.5) / current._height;
                    
                    size_t index = 2 * (y * current._width + x);
                    float xdiff = data[index];
                    float ydiff = data[index + 1];
                    if (!std::isnan(xdiff) && !std::isnan(ydiff))
                    {
                        for (view_t & view : views)
                        {
                            if (view._camera == scene._cameras[icam]._name && view._cubemap_texture == renderedFlowTexture + icam * 6)
                            {
                                //std::cout << xf * view._width << ' '<< yf * view._height << ' ' << xdiff << ' '<< ydiff<<std::endl;
                                arrows.emplace_back(arrow_t({xf * view._width + view._x, height() - yf * view._height - view._y, xdiff, ydiff}));
                            }
                        }
                    }
                }
            }
            delete[] data;
        }
        for (size_t i = 0; i < arrow_handles.size(); ++i)
        {
            delete arrow_handles[i];
        }
    }
    if (show_curser && num_cams != 0)
    {
        copy_pixel_buffer_to_screenshot(curser_handle, session._debug);
        size_t sy = 1024 - static_cast<size_t>(curserViewPos.y() * 1024);
        size_t sx = static_cast<size_t>(curserViewPos.x() * 1024);
        if (sx < 1024 && sy < 1024)
        {
            size_t index = 3 * (sy * 1024 + sx);
            if (loglevel > 5)
            {
                std::cout << index << '\t';
            }
            float *data = reinterpret_cast<float*>(curser_handle._data);
            curser_3d = QVector4D(data[index], data[index + 1], data[index + 2], 1);
            if (loglevel > 5)
            {
                std::cout << curser_3d.x() << ' ' << curser_3d.y() << ' ' << curser_3d.z() << '\t';
            }
            delete[] data;
            if (curser_3d.x() != 0 || curser_3d.y() != 0 || curser_3d.z() != 0)
            {
                for (size_t icam = 0; icam < num_cams; ++icam)
                {
                    QVector4D test = camera_transformations[icam] * curser_3d;
                    if (loglevel > 5)
                    {
                        std::cout << "test " << test.x() << ' ' << test.y() << '\t';
                    }
                    
                    std::array<float, 2> tmp = kart_to_equidistant(std::array<float, 3>({-test.x(), -test.y(), -test.z()}));
                    tmp[0] = -tmp[0];
                    
                    //std::array<float, 2> tmp = kart_to_equidistant(std::array<float, 3>({test.x(), test.y(), test.z()}));
                    for (size_t icam = 0; icam < 2; ++icam)
                    {
                        tmp[icam] = tmp[icam] * fov / 45.;
                    }
                    if (tmp[0] * tmp[0] + tmp[1] * tmp[1] > 0.25)
                    {
                        continue;
                    }
                    tmp[0] += 0.5;
                    tmp[1] += 0.5;
                    
                    for (view_t & view : views)
                    {
                        if (view._camera == scene._cameras[icam]._name)
                        {
                            marker.push_back(QPointF(tmp[0] * view._width + view._x, tmp[1] * view._height + view._y));
                        }
                    }
                    
                    //marker.push_back(QPointF(test.x(), test.y()));
                    if (loglevel > 5)
                    {
                        std::cout << "marker " << tmp[0] << ' ' << tmp[1] << std::endl;
                    }
                }
            }
        }  
    }
    for (screenshot_handle_t * current : scene._screenshot_handles)
    {
        if (current->_error_code != 1)
        {
            copy_pixel_buffer_to_screenshot(*current, session._debug);
            last_screenshottimes.emplace_back(std::chrono::high_resolution_clock::now());
        }
    }
    scene._screenshot_handles.clear();
    remapping_shader._program->release();
    //num_textures = 0;
    glDeleteTextures(num_textures, renderedTexture);
    glDeleteTextures(num_textures, renderedFlowTexture);
    glDeleteTextures(num_textures, renderedPositionTexture);
    glDeleteTextures(num_textures, renderedIndexTexture);
    glDeleteTextures(num_textures, renderedDepthTexture);
    //glDeleteRenderbuffers(num_textures, renderedDepthTexture);

    //screenshot = "movie/" + std::to_string(m_frame) + ".tga";
    glViewport(0,0,width(), height());
    glDisable(GL_DEPTH_TEST);
    //GLdouble glColor[4];
    //glGetDoublev(GL_CURRENT_COLOR, glColor);
    //QColor fontColor = QColor(glColor[0], glColor[1], glColor[2], glColor[3]);

    if (qogpd == nullptr)
    {
        qogpd = new QOpenGLPaintDevice;
    }
    int ratio = devicePixelRatio();
    qogpd->setSize(QSize(width() * ratio, height() * ratio));
    qogpd->setDevicePixelRatio(ratio);
    QPainter painter(qogpd);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    painter.setFont(QFont("Times", 24));

    std::string framestr = std::to_string(m_frame);
    //painter.setPen(QColor(clamp(static_cast<int>(curser_3d.x() * 255), 0, 0xFF), clamp(static_cast<int>(curser_3d.y() * 255), 0, 0xFF), clamp(static_cast<int>(curser_3d.z() * 255), 0, 0xFF), 255));
    //painter.setPen(QColor(255, 255, 255, 255));
    //painter.drawEllipse(QPointF(100,100), 10, 10);
    painter.setPen(QColor(255,255,255,255));
    for (QPointF const & m : marker)
    {
        painter.drawEllipse(QPointF(m.x(),m.y()), 10, 10);
    }
    for (arrow_t const & arrow : arrows)
    {
        float headx = arrow._x0+ arrow._x1, heady = arrow._y0 + arrow._y1;
        float centerx = arrow._x0+ arrow._x1 * 0.5, centery = arrow._y0 + arrow._y1 * 0.5;
        painter.drawLine(arrow._x0, arrow._y0, headx, heady);
        painter.drawLine(centerx + arrow._y1 * 0.5, centery - arrow._x1 * 0.5, headx, heady);
        painter.drawLine(centerx - arrow._y1 * 0.5, centery + arrow._x1 * 0.5, headx, heady);
    }
    arrows.clear();
    painter.drawText(30, 30, QString(framestr.c_str()));
    last_rendertimes.push_back(current_time);
    //std::cout << last_rendertimes.front() << '+' << CLOCKS_PER_SEC <<'<' << current_time << std::endl;
    while (std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_rendertimes.front()).count() > 1000000)
    {
        last_rendertimes.pop_front();
    }
    while (last_screenshottimes.size() > 0 && std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_screenshottimes.front()).count() > 1000000)
    {
        last_screenshottimes.pop_front();
    }
    //std::string tmp = "fps " + std::to_string(last_rendertimes.size());
    
    double duration = (static_cast<std::chrono::duration<double> >( current_time - last_rendertime )).count();
    std::string tmp = "fps "  + std::to_string(last_rendertimes.size()/* + static_cast<float>(current_time - last_rendertimes.front()) / CLOCKS_PER_SEC*/) + " " + std::to_string(last_screenshottimes.size()) + " " + std::to_string(1 / duration);
    painter.drawText(150,30,QString(tmp.c_str()));
    size_t row = 0;
    for (vec2f_t const & cf : curser_flow)
    {
        tmp = "("+std::to_string(cf.x()) + " " + std::to_string(cf.y()) + ") " + std::to_string(sqrt(cf.dot()));
        painter.drawText(400,30 + row * 30,QString(tmp.c_str()));
        ++row;
    }
    last_rendertime = current_time;
    for (size_t i = 0; i < scene._framelists.size(); ++i)
    {
        bool found = std::binary_search(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), m_frame);
        painter.setPen(QColor((!found) * 255,found * 255,0,255));
        painter.drawText(30, i*30 + 60, QString(scene._framelists[i]._name.c_str()));
    }
    painter.end();
    if (session._screenshot != "")
    {
        //awx_ScreenShot(session._screenshot);
        session._screenshot = "";
    }
    
    if (session._realtime)
    {
        session._m_frame += session._play * session._frames_per_second * duration;
    }
    else
    {
        session._m_frame += session._play * session._frames_per_step;
    }
    if (show_only != "")
    {
        for (size_t i = 0; i < scene._framelists.size(); ++i)
        {
            if (scene._framelists[i]._name == show_only)
            {
                auto iter = std::lower_bound(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), m_frame);
                if (iter != scene._framelists[i]._frames.end())
                {
                    session._m_frame = session._play >= 0 ? *(iter) : *(iter-1);
                }
            }
        }
    }
    if (m_frame != session._m_frame)
    {
        session.scene_update(UPDATE_FRAME);
    }
    ++session._rendered_frames;
    size_t write = 0;
    for (size_t read = 0; read < wait_for_rendered_frame_handles.size(); ++read)
    {
        if (wait_for_rendered_frame_handles[read]->_frame < session._rendered_frames)
        {
            wait_for_rendered_frame_handles[read]->_value = true;
            wait_for_rendered_frame_handles[read]->_cv.notify_one();
            if (loglevel > 5)
            {
                std::cout << "notify " << std::endl;
            }
        }
        else
        {
            wait_for_rendered_frame_handles[write++] = wait_for_rendered_frame_handles[read];
        }
    }
    wait_for_rendered_frame_handles.erase(wait_for_rendered_frame_handles.begin() + write, wait_for_rendered_frame_handles.end());
    scene._mtx.unlock();
    if (session._exit_program)
    {
        perspective_shader.destroy();
        remapping_shader.destroy();
        approximation_shader.destroy();
        std::vector<mesh_object_t>().swap(scene._objects);
        //scene._objects.clear();
        deleteLater();
        destroyed = true;
        exit(0);
    }
    if (loglevel > 5)
    {
        std::cout << "end render" << std::endl;
    }
}

void TriangleWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        ++session._perm;
    }
}

