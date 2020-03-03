/*
Copyright (c) 2020 Paul Stahr

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

#include "openglwindow.h"

#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
//#include <libxml2/libxml/parser.h>
#include <QtCore/qmath.h>
#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <iostream>
#include <string>
#include <fstream>
#include <future>
#include <thread>
#include <sstream>
#include <map>
#include <mutex>
#include <cstdint>
#include "serialize.h"
#include "io_util.h"
#include "geometry.h"
#include "transformation.h"
#include "OBJ_Loader.h"
#include "image_io.h"
#include "shader.h"
#include "qt_util.h"
#include "main.h"

//vec3 vertex_vector = vertex.xyz - center; //vertex = gl_Vertex, center = the patch's center = the camera positiondepth = length(vertex_vector)/100.0; //depth is a varyingvertex_vector = normalize(vertex_vector);//http://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projectionfloat z_term = pow( 2.0/(1.0-vertex_vector.z), 0.5 );vec2 coord = z_term*vertex_vector.xy; // in range (-2,2)coord = ((coord+vec2(2.0))/4.0);//*vec2(512.0,512.0); //in range (0,1)vertex.xy = coord;vertex.z = 0.0;gl_Position = vertex;

//color.rgb = indices; //default values of patch, just so we can see what's going ongl_FragDepth = depth;

/*class obj_model
{
    std::vector<float> _vertices;
    std::vector<float> _texture_coordinates;
    std::vector<size_t> _faces;
}

void load_obj(std::string const & filename, obj_model & model)
{
    std::ifstream stream(filename);
    obj_model._vertices.clear();
    obj_model._texture_coordinates.clear();
    obj_model._faces.clear();
    std::string line;
    while (std::read_line(stream, line))
    {
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        if (type == "v")
        {
            model._vertices.add(
        }
    }
}*/

//OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER


//OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT

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

#define BUFFER_OFFSET(i) ((void*)(i))



enum viewmode_t
{
    EQUIDISTANT, EQUIDISTANT_APPROX, PERSPECTIVE
};

enum viewtype_t
{
    VIEWTYPE_RENDERED, VIEWTYPE_POSITION, VIEWTYPE_DEPTH, VIEWTYPE_FLOW, VIEWTYPE_INDEX
};

struct screenshot_handle_t
{
    std::string _camera;
    viewtype_t _type;
    size_t _width;
    size_t _height;
    size_t _channels;
    size_t _datatype;
    bool _ignore_nan;
    void *_data = nullptr;
    size_t _error_code;
    std::mutex _mtx;
    std::condition_variable _cv;

    bool operator()()
    {
        return _data != nullptr || _error_code != 0;
    }
};

struct wait_for_rendered_frame
{
    size_t _frame;
    bool _value = false;
    std::condition_variable _cv;

    wait_for_rendered_frame(size_t value_) :_frame(value_) {}

    bool operator()() const
    {
        return _value;
    }
};


struct arrow_t
{
    float _x0, _y0, _x1, _y1;
};
struct view_t
{
    std::string const & _camera;
    GLuint *_cubemap_texture;
    size_t _x, _y, _width, _height;
    bool _diff;
    bool _depth;
};

class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void mouseMoveEvent(QMouseEvent *e) override;
    void initialize() override;
    void render() override;
    int m_frame;
    bool exit_program = false;
    bool destroyed = false;
    int play = 1;
    viewmode_t viewmode = EQUIDISTANT;
    size_t loglevel = 1;
    int32_t diffforward = 1;
    int32_t diffbackward = -1;
    int perm = 0;
    std::string show_only;
    size_t smoothing = 0;
    float fov = 90;
    size_t preresolution = 512;
    bool reload_shader = false;
    bool diffrot = true;
    bool difftrans = true;
    bool show_raytraced = true;
    bool show_flow = true;
    bool show_index = false;
    bool show_position = false;
    bool show_depth = true;
    bool show_curser = false;
    bool show_arrows = true;
    std::vector<wait_for_rendered_frame*> wait_for_rendered_frame_handles;
    std::vector<screenshot_handle_t *> _screenshot_handles;
    std::vector<arrow_t> arrows;
    std::string screenshot;
    size_t _rendered_frames;

    spherical_approximation_shader_t approximation_shader;
    perspective_shader_t perspective_shader;
    remapping_spherical_shader_t remapping_shader;
private:
    std::vector<view_t> views;
    std::vector<QPointF> marker;
    std::vector<QMatrix4x4> camera_transformations;
  
};


TriangleWindow::TriangleWindow()
    :m_frame(100000)
{
}
//objl::Loader Loader;



QQuaternion to_qquat(rotation_t const & rot)
{
    return QQuaternion(rot[0], rot[1], rot[2], rot[3]);
}

/*template <class T, size_t N>
struct valcarray  : std::array<T,N>
{
};*/

/*template<typename T, typename std::enable_if<std::is_base_of<MyClass, T>::value>::type* = nullptr>
T Foo(T bar)
{
    return T();
}*/

struct framelist_t
{
    std::string _name;
    std::vector<size_t> _frames;

    framelist_t(std::string const & name_, std::vector<size_t> const & framelist_) :_name(name_), _frames(framelist_)
    {
    }
};
struct object_t
{
    std::string _name;
    size_t _id;
    std::map<size_t, vec3f_t> _key_pos;
    std::map<size_t, rotation_t> _key_rot;
    QMatrix4x4 _transformation;
    bool _visible;

    object_t(std::string const & name_):_name(name_), _id(0),  _transformation({1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}), _visible(true) {}
};

struct mesh_object_t: object_t
{
    QOpenGLTexture *tex_Ka = nullptr;
    // Diffuse Texture Map
    QOpenGLTexture *tex_Kd = nullptr;
    // Specular Texture Map
    QOpenGLTexture *tex_Ks;
    // Specular Hightlight Map
    QOpenGLTexture *tex_Ns;
    // Alpha Texture Map
    QOpenGLTexture *tex_d;
    // Bump Map
    QOpenGLTexture *tex_bump;
    objl::Loader _loader;
    std::vector<GLuint> _vbo;
    std::vector<GLuint> _vbi;

    mesh_object_t(std::string const & name_, std::string const & objfile) : object_t(name_), _vbo(0)
    {
        clock_t current_time = clock();
        _loader.LoadFile(objfile.c_str());
        std::cout << "mesh loading time: " << float( clock() - current_time ) / CLOCKS_PER_SEC << std::endl;
    }

    void load_meshes()
    {
        if (_vbo.size() == 0)
        {
            std::cout << "load meshes" << std::endl;
            _vbo.resize(_loader.LoadedMeshes.size());
            glGenBuffers(_vbo.size(), _vbo.data());
            _vbi.resize(_loader.LoadedMeshes.size());
            glGenBuffers(_vbi.size(), _vbi.data());
            for (size_t i = 0; i < _vbo.size(); ++i)
            {
                std::cout << "load mesh " << i << std::endl;
                objl::Mesh const & curMesh = _loader.LoadedMeshes[i];
                glBindBuffer(GL_ARRAY_BUFFER, _vbo[i]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(objl::Vertex) * curMesh.Vertices.size(), curMesh.Vertices.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbi[i]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * curMesh.Indices.size(), curMesh.Indices.data(), GL_STATIC_DRAW);
            }
        }
    }

    void load_textures()
    {
        for (size_t i = 0; i < _loader.LoadedMeshes.size(); ++i)
        {
            if (_loader.LoadedMeshes[i].MeshMaterial.map_Ka != "" && tex_Ka == nullptr)
            {
                std::cout << "ka" << std::endl;
                tex_Ka = new QOpenGLTexture(QImage(QString(_loader.LoadedMeshes[i].MeshMaterial.map_Ka.c_str())));
            }
            if (_loader.LoadedMeshes[i].MeshMaterial.map_Kd != "" && tex_Kd == nullptr)
            {
                std::cout << "kd" << std::endl;
                QImage img;
                if (!img.load(_loader.LoadedMeshes[i].MeshMaterial.map_Kd.c_str()))
                {
                    std::cout << "error, can't load image " << _loader.LoadedMeshes[i].MeshMaterial.map_Kd.c_str() << std::endl;
                }
                std::cout << img.width() << ' ' << img.height() << std::endl;
                tex_Kd   = new QOpenGLTexture(img.mirrored());
            }
        }
        //print_models(_loader, std::cout);
    }
    
    ~mesh_object_t()
    {
        glDeleteBuffers(_vbo.size(), _vbo.data());
        _vbo.clear();
        glDeleteBuffers(_vbi.size(), _vbi.data());
        _vbi.clear();
        if (tex_Kd != nullptr)
        {
            tex_Kd -> destroy();
            delete tex_Kd;
            tex_Kd = nullptr;
        }
    }
};

struct camera_t : object_t
{
    viewmode_t _viewmode;
    camera_t(std::string const & name_) : object_t(name_), _viewmode(PERSPECTIVE) {}
};

int awx_ScreenShot(std::string const & filename) {
    unsigned char *pixels;
    FILE *image;
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    pixels = new unsigned char[viewport[2]*viewport[3]*3];
    glReadPixels(0, 0, viewport[2], viewport[3], GL_BGR, GL_UNSIGNED_BYTE, pixels);
    char tempstring[50];
    sprintf(tempstring,filename.c_str(),1);
    if((image=fopen(tempstring, "wb"))==NULL)
    {
        return 1;
    }
    uint8_t TGAheader[12]= {0,0,2,0,0,0,0,0,0,0,0,0};
    uint8_t header[6]= {((uint8_t)(viewport[2]%256)), ((uint8_t)(viewport[2]/256)), ((uint8_t)(viewport[3]%256)), ((uint8_t)(viewport[3]/256)),24,0}; // TGA header schreiben
    fwrite(TGAheader, sizeof(unsigned char), 12, image);  // Header schreiben
    fwrite(header, sizeof(unsigned char), 6, image);
    fwrite(pixels, sizeof(unsigned char), viewport[2]*viewport[3]*3, image);
    fclose(image);
    delete [] pixels;
    return 0;
}

struct scene_t
{
    std::vector<camera_t> _cameras;
    std::vector<mesh_object_t> _objects;
    std::vector<framelist_t> _framelists;
    std::mutex _mtx;
    
    camera_t * get_camera(std::string const & name)
    {
        for (camera_t & obj : _cameras)
        {
            if (obj._name == name)
            {
                return &obj;
            }
        }
        return nullptr;
    }

    object_t * get_object(std::string const & name)
    {
        for (object_t & obj : _objects)
        {
            if (obj._name == name)
            {
                return &obj;
            }
        }

        return get_camera(name);
    }

    object_t & get_object(size_t index)
    {
        if(index < _cameras.size())
        {
            return _cameras[index];
        }
        else
        {
            return _objects[index - _cameras.size()];
        }
    }

    size_t num_objects() const
    {
        return _cameras.size() + _objects.size();
    }
};


static scene_t scene;
TriangleWindow *window;



template <typename StringIter>
void read_transformations(QMatrix4x4 & matrix, StringIter begin, StringIter end)
{
    while(begin != end)
    {
        std::string const & type = *begin;
        ++begin;
        if (type == "pos")
        {
            vec3f_t pos;
            for (float & elem : pos)
            {
                elem = std::stof(*begin);
                ++begin;
            }
            matrix.translate(pos[0],pos[1],pos[2]);
        }
        else if (type == "scale")
        {
            scale_t scale;
            for (float & elem : scale)
            {
                elem = std::stof(*begin);
                ++begin;
            }
            matrix.scale(scale[0],scale[1],scale[2]);
        }
        else if (type == "rot")
        {
            rotation_t rot;
            for (float & elem : rot)
            {
                elem = std::stof(*begin);
                ++begin;
            }
            matrix.rotate(to_qquat(rot));
        }
        else if (type == "erot")
        {
            float angle = std::stof(*(begin++));
            float x = std::stof(*(begin++));
            float y = std::stof(*(begin++));
            float z = std::stof(*(begin++));
            matrix.rotate(angle, x, y, z);
        }
    }
}

/*void read_transformations(object_t & obj, std::istringstream & iss)
{
    std::string type;
    while (getline(iss, type,' '))
    {
        if (type == "pos")
        {
            for (float & elem : pos)
            {
                iss >> elem;
            }
        }
        else if (type == "scale")
        {
            iss >> obj._scale.x();
            iss >> obj._scale.y();
            iss >> obj._scale.z();
        }
        else if (type == "rot")
        {
            iss >> obj._rot.x();
            iss >> obj._rot.y();
            iss >> obj._rot.z();
            iss >> obj._rot.w();
        }
    }
}*/

struct exec_env
{
    std::mutex _mtx;
    std::vector<std::future<void> > _pending_futures;

    exec_env() {}
};

std::vector<std::thread *> executing_threads;

void exec(std::string const & input, exec_env & env, std::ostream & out);

template <typename T>
void flip(T *pixels, size_t width, size_t height, size_t channels)
{
    for (size_t y = 0; y < height / 2; ++y)
    {
        size_t iy = height - y - 1;
        for (size_t x = 0; x < width; ++x)
        {
            for (size_t i = 0; i < channels; ++i)
            {
                std::swap(pixels[(y * width + x) * channels + i],pixels[(iy * width + x) * channels +i]);
            }
        }
    }
}

template <typename T>
void brg_to_rgb(T *pixels, size_t width, size_t height)
{
    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            size_t idx = (y * width + x) * 3;
            T b = pixels[idx], g = pixels[idx + 1], r = pixels[idx + 2];
            pixels[idx] = r;
            pixels[idx + 1] = g;
            pixels[idx + 2] = b;
        }
    }
}

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int take_lazy_screenshot(std::string filename, size_t width, size_t height, std::string camera, viewtype_t type, bool export_nan)
{
    screenshot_handle_t handle;
    handle._camera= camera;
    handle._type = type;
    handle._width = width;
    handle._height = height;
    handle._channels = ends_with(filename, ".exr") ? 0 : 3;
    handle._datatype = (type == VIEWTYPE_POSITION || type == VIEWTYPE_DEPTH || type == VIEWTYPE_FLOW) && ends_with(filename, ".exr") ?  GL_FLOAT : GL_UNSIGNED_BYTE;
    handle._ignore_nan = export_nan;
    handle._data = nullptr;
    handle._error_code = 0;
    scene._mtx.lock();
    std::cout << "add handle " << filename << std::endl;
    window->_screenshot_handles.push_back(&handle);
    scene._mtx.unlock();
    std::unique_lock<std::mutex> lck(handle._mtx);
    handle._cv.wait(lck,std::ref(handle));
    if (handle._error_code != 0)
    {
        std::cout << "no screenshot was taken " << handle._error_code << std::endl;
        return 1;
    }
    std::cout << "writing " << filename << std::endl;

    if (ends_with(filename, ".exr"))
    {
        if (handle._datatype == GL_FLOAT)
        {
            float *pixels = reinterpret_cast<float*>(handle._data);
            flip(pixels, width, height, handle._channels);
            if (handle._channels == 1)
            {
                writeGZ1 (filename,
                          pixels,
                          handle._width,
                          handle._height) ;
            }
            else if (handle._channels == 2)
            {
                float *red = new float[handle._width * handle._height];
                float *green = new float[handle._width * handle._height];
                for (size_t i = 0; i < handle._width * handle._height; ++i)
                {
                    red[i]   = pixels[i * 2];
                    green[i] = pixels[i * 2 + 1];
                }

                writeGZ1 (filename,
                          red,
                          green,
                          handle._width,
                          handle._height) ;
                delete[] red;
                delete[] green;
            }
            else if (handle._channels == 3)
            {
                float *red = new float[handle._width * handle._height];
                float *green = new float[handle._width * handle._height];
                float *blue = new float[handle._width * handle._height];
                for (size_t i = 0; i < handle._width * handle._height; ++i)
                {
                    blue[i]  = pixels[i * 3];
                    red[i]   = pixels[i * 3 + 1];
                    green[i] = pixels[i * 3 + 2];
                }

                //Libs: -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf
                writeGZ1 (filename,
                          red,
                          green,
                          blue,
                          handle._width,
                          handle._height) ;
                delete[] red;
                delete[] green;
                delete[] blue;
            }
            else
            {
                std::cout << "Error, invalid number of channels: " << handle._channels << std::endl;
            }
            std::cout << "written " << filename << std::endl;

            delete[] pixels;
        }
        else
        {
            std::cout << "Error, invalid datatype" << std::endl;
        }
    }
    else
    {
        uint8_t *pixels = reinterpret_cast<uint8_t*>(handle._data);
        flip(pixels, width, height, 3);
        //brg_to_rgb(pixels, width, height);
        QPixmap pixmap(handle._width,handle._height);

        QByteArray pixData;
        pixData.append(QString("P6 %1 %2 255 ").arg(handle._width).arg(handle._height));
        pixData.append(reinterpret_cast<char*>(pixels), handle._width * handle._height*3);
        if (pixmap.loadFromData(reinterpret_cast<uchar *>(pixData.data()), pixData.size()))

            //if(pixmap.loadFromData(pixels,handle._width * handle._height*3))
        {
            pixmap.save(filename.c_str());
            std::cout << "written " << filename << std::endl;
        }
        else
        {
            std::cout << "error" << std::endl;
        }
        delete[] pixels;
    }

    /*char tempstring[50];
    sprintf(tempstring,filename.c_str(),1);
    FILE *image;
    if((image=fopen(tempstring, "wb"))==NULL)
    {
        std::cout << "can't open file " << tempstring << std::endl;
        return 1;
    }
    uint8_t TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
    width = handle._width;
    height = handle._height;
    uint8_t header[6]={((uint8_t)(width%256)), ((uint8_t)(width/256)), ((uint8_t)(height%256)), ((uint8_t)(height/256)),24,0};  // TGA header schreiben
    fwrite(TGAheader, sizeof(unsigned char), 12, image);  // Header schreiben
    fwrite(header, sizeof(unsigned char), 6, image);
    fwrite(pixels, sizeof(unsigned char), width * height *3, image);
    fclose(image);
    delete [] pixels;*/
    return 0;
}

void exec_impl(std::string input, exec_env & env, std::ostream & out)
{
    std::vector<std::string> args;
    IO_UTIL::split_in_args(args, input);
    //std::stringstream ss(input);
    std::string command = args[0];
    if (command == "help")
    {
        out << "goto (<frame>)" << std::endl;
        out << "play (<speed>)" << std::endl;
        out << "loglevel (<ivalue>)" << std::endl;
        out << "next (<frames>)"<< std::endl;
        out << "prev (<frames>)"<< std::endl;
        out << "show_only (<framelist>)" << std::endl;
        out << "diffbackward (<num_frames>)" <<std::endl;
        out << "diffforward (<num_frames>)" << std::endl;
        out << "screenshot <filename>" << std::endl;
        out << "screenshot2 <filename> <width> <height> <camera> <type> (<export nan>)" << std::endl;
        out << "diffrot (<activated>)" << std::endl;
        out << "difftrans (<activated>)" << std::endl;
        out << "preresolution (<num_pixels>)" << std::endl;
        out << "echo <...>" << std::endl;
        out << "run <scriptfile>" << std::endl;
        out << "wait -> wait for next redraw" << std::endl;
        out << "join -> wait for all tasks in the pipeline to fininsh" << std::endl;
        out << "framelist <filename> <name>" <<std::endl;
        out << "oubject <filename> (<transformation>)" << std::endl;
        out << "id <name> (<id-value>)" << std::endl;
        out << "anim <filename> <transformations>" << std::endl;
        out << "load <session_file>" << std::endl;
        out << "save <session_file>" << std::endl;
    }
    else if (command == "load")
    {
        
    }
    else if (command == "save")
    {
        
    }
    else if (command == "viewmode")
    {
        if (args[1] == "equidistant")
        {
            window->viewmode = EQUIDISTANT;
        }
        else if (args[1] == "perspective")
        {
            window->viewmode = PERSPECTIVE;
        }
    }
    else if (command == "goto")
    {
        if (args.size() > 1)
        {
            window->m_frame = std::stoi(args[1]);
        }
        else
        {
            out << window->m_frame << std::endl;
        }
    }
    else if (command == "play")
    {
        if (args.size() > 1)
        {
            window->play = std::stoi(args[1]);
        }
        else
        {
            out << window-> play << std::endl;
        }
    }
    else if (command == "loglevel")
    {
        if (args.size() > 1)
        {
            window->loglevel = std::stoi(args[1]);
        }
        else
        {
            out << window-> loglevel << std::endl;
        }
    }
    else if (command == "next")
    {
        if (args.size() > 1)
        {
            window->m_frame += std::stoi(args[1]);
        }
        else
        {
            ++window->m_frame;
        }
    }
    else if (command == "prev")
    {
        if (args.size() > 1)
        {
            window->m_frame -= std::stoi(args[1]);
        }
        else
        {
            --window->m_frame;
        }
    }
    else if (command == "show_only")
    {
        if (args.size() > 1)
        {
            window-> show_only = args[1];
        }
        else
        {
            window -> show_only = "";
        }
    }
    else if (command == "diffbackward")
    {
        if (args.size() > 1)
        {
            window->diffbackward = std::stoi(args[1]);
        }
        else
        {
            out << window->diffbackward << std::endl;
        }
    }
    else if (command == "diffforward")
    {
        if (args.size() > 1)
        {
            window->diffforward = std::stoi(args[1]);
        }
        else
        {
            out << window->diffbackward << std::endl;
        }
    }
    else if (command == "screenshot")
    {
        window->screenshot = args[1];
    }
    else if (command == "screenshot2")
    {
        //screenshot2 test.png 512 512 left_eye rendered
        //screenshot2 test.tga 512 512 left_eye rendered
        //screenshot2 test.jpg 512 512 left_eye rendered
        bool export_nan = false;
        if (args.size() > 6)
        {
            export_nan = std::stoi(args[6]);
        }
        viewtype_t view;
        if (args[5] == "rendered")
        {
            view = VIEWTYPE_RENDERED;
        }
        else if (args[5] == "position")
        {
            view = VIEWTYPE_POSITION;
        }
        else if (args[5] == "index")
        {
            view = VIEWTYPE_INDEX;
        }
        else if (args[5] == "depth")
        {
            view = VIEWTYPE_DEPTH;
        }
        else if (args[5] == "flow")
        {
            view = VIEWTYPE_FLOW;
        }
        else
        {
            throw std::runtime_error("type not known");
        }
        take_lazy_screenshot(args[1], std::stoi(args[2]), std::stoi(args[3]), args[4], view, export_nan);
        std::cout << "success" << std::endl;
    }
    else if (command == "diffrot")
    {
        if (args.size() > 1)
        {
            window -> diffrot = static_cast<bool>(std::stoi(args[1]));
        }
        else
        {
            out << window -> diffrot << std::endl;
        }
    }
    else if (command == "difftrans")
    {
        if (args.size() > 1)
        {
            window -> difftrans = static_cast<bool>(std::stoi(args[1]));
        }
        else
        {
            out << window -> difftrans << std::endl;
        }
    }
    else if (command == "smoothing")
    {
        if (args.size() > 1)
        {
            window -> smoothing = std::stoi(args[1]);
        }
        else
        {
            out << window -> smoothing << std::endl;
        }
    }
    else if (command == "fov")
    {
        if (args.size() > 1)
        {
            window -> fov = std::stof(args[1]);
        }
        else
        {
            out << window -> fov << std::endl;
        }
    }
    else if (command == "reload")
    {
        if (args[1] == "shader")
        {
            window-> reload_shader = true;
        }
    }
    else if (command == "show")
    {
        bool *ref = nullptr;
        if (args[1] == "raytraced")
        {
            ref = &window-> show_raytraced;
        }
        else if (args[1] == "flow")
        {
            ref = &window->show_flow;
        }
        else if (args[1] == "index")
        {
            ref = &window->show_index;
        }
        else if (args[1] == "pos")
        {
            ref = &window->show_position;
        }
        else if (args[1] == "depth")
        {
            ref = &window->show_depth;
        }
        else if (args[1] == "curser")
        {
            ref = &window->show_curser;
        }
        else if (args[1] == "arrows")
        {
            ref = &window->show_arrows;
        }
        if (ref == nullptr)
        {
            out << "error, key not known" << std::endl;
            return;
        }
        if (args.size() > 2)
        {
            *ref = std::stoi(args[2]);
        }
        else
        {
            out << *ref << std::endl;
        }
    }
    else if (command == "preresolution")
    {
        if (args.size() > 1)
        {
            window -> preresolution = std::stoi(args[1]);
        }
        else
        {
            out << window->preresolution << std::endl;
        }
    }
    else if (command == "echo")
    {
        print_elements(out, args.begin() + 1, args.end(), ' ');
    }
    else if (command == "modify")
    {
        std::string name = args[1];
        for (size_t i = 0; i < scene.num_objects(); ++i)
        {
            object_t & obj = scene.get_object(i);
            if (obj._name == name)
            {
                if (args[2] == "transform")
                {
                    obj._transformation.setToIdentity();
                    read_transformations(obj._transformation, args.begin() + 3, args.end());
                }
                else if (args[2] == "visible")
                {
                    obj._visible = std::stoi(args[3]);
                }
            }
        }
    }
    else if (command == "run")
    {
        std::ifstream infile(args[1]);
        std::string line;
        std::cout << "run" <<args[1] << std::endl;
        while(std::getline(infile, line))
        {
            std::cout << line << std::endl;
            exec(line, env, out);
        }
        infile.close();
    }
    else if (command == "camera")
    {
        if (args.size() > 1)
        {
            std::string name = args[1];
            scene._cameras.push_back(camera_t(name));
            read_transformations(scene._cameras.back()._transformation, args.begin() + 2, args.end());
        }
        else
        {
            for (camera_t const & cam : scene._cameras)
            {
                out << cam._name << std::endl;
            }
        }
    }
    else if (command == "wait")
    {
        std::mutex mtx;
        std::unique_lock<std::mutex> lck(mtx);
        scene._mtx.lock();
        wait_for_rendered_frame wait_obj(window->_rendered_frames + 1);
        window->wait_for_rendered_frame_handles.push_back(&wait_obj);
        scene._mtx.unlock();
        wait_obj._cv.wait(lck,std::ref(wait_obj));
    }
    else if (command == "join")
    {
        std::cout << "joining" << std::endl;
        env._mtx.lock();
        for (size_t i = 0; i < env._pending_futures.size(); ++i)
        {
            env._pending_futures[i].wait();
        }
        env._pending_futures.clear();
        std::cout << "joined" << std::endl;
        env._mtx.unlock();
    }
    else if (command == "framelist")
    {
        if (args.size() > 1)
        {
            std::string name = args[1];
            std::string framefilename = args[2];
            std::ifstream framefile(framefilename);
            std::vector<size_t> framelist = IO_UTIL::parse_framelist(framefile);
            scene._mtx.lock();
            scene._framelists.emplace_back(name, framelist);
            scene._mtx.unlock();
            framefile.close();
        }
        else
        {
            for (framelist_t const & list : scene._framelists)
            {
                out << list._name << std::endl;
            }
        }
    }
    else if (command == "object")
    {
        if (args.size() > 1)
        {
            std::string name = args[1];
            std::string meshfile = args[2];
            mesh_object_t m = mesh_object_t(name, meshfile);
            scene._mtx.lock();
            scene._objects.push_back(m);
            scene._mtx.unlock();
            read_transformations(scene._objects.back()._transformation, args.begin() + 3, args.end());
        }
        else
        {
            for (object_t const & obj : scene._objects)
            {
                out << obj._name << std::endl;
            }
        }
            
    }
    else if (command == "id")
    {
        object_t *obj = scene.get_object(args[1]);
        if (obj != nullptr)
        {
            if (args.size() > 2)
            {
                obj->_id = std::stoi(args[2]);
            }
            else
            {
                out << obj->_id << std::endl;
            }
        }
        else
        {
            out << "object not found" << std::endl;
        }
    }
    else if (command == "exit")
    {
        window->exit_program = true;
    }
    else if (command == "anim")
    {
        std::string animfile = args[1];
        std::ifstream animss(animfile);
        std::cout << "animfile: " << animfile << std::endl;
        std::vector<std::vector<float> > anim_data = IO_UTIL::parse_csv(animss);
        animss.close();
        std::cout << "anim_data_size " << anim_data.size() << std::endl;
        size_t column = 0;
        size_t index_column = std::numeric_limits<size_t>::max();
        {
            auto iter = std::find(args.begin(), args.end(), "frame");
            if (iter != args.end())
            {
                index_column = std::stoi(*(++iter));
            }
        }
        for (auto strIter = args.begin() + 2; strIter != args.end();)
        {
            std::string field = *strIter;
            ++strIter;
            if (field == "skip")
            {
                size_t to_skip = std::stoi(*strIter);
                ++strIter;
                column += to_skip;
                std::cout << "skip " << to_skip << std::endl;
            }
            else if (field == "frame")
            {
                ++strIter;
            }
            else
            {
                scene._mtx.lock();
                bool found = false;
                for (size_t i = 0; i < scene.num_objects(); ++i)
                {
                    object_t & obj = scene.get_object(i);
                    if (obj._name == field)
                    {
                        found = true;
                        std::string type = *strIter;
                        std::cout << "type " << type << std::endl;
                        if (type == "pos")
                        {
                            for (size_t fr = 0; fr < anim_data.size(); ++fr)
                            {
                                size_t frame = fr;
                                if (index_column != std::numeric_limits<size_t>::max())
                                {
                                    frame = anim_data[fr][index_column];
                                }
                                //std::cout << index_column << ' '<< frame << std::endl;
                                obj._key_pos[frame] = {anim_data[fr][column], anim_data[fr][column + 1], anim_data[fr][column + 2]};
                                //std::cout << obj._key_pos.size() << std::endl;

                            }
                            column += 3;
                        }
                        else if (type == "rot")
                        {
                            for (size_t fr = 0; fr < anim_data.size(); ++fr)
                            {
                                size_t frame = fr;
                                if (index_column != std::numeric_limits<size_t>::max())
                                {
                                    frame = anim_data[fr][index_column];
                                }
                                obj._key_rot[frame] = {anim_data[fr][column], anim_data[fr][column + 1], anim_data[fr][column + 2], anim_data[fr][column + 3]};
                            }
                            column += 4;
                        }
                    }
                }
                if (found)
                {
                    ++strIter;
                }
                scene._mtx.unlock();
            }
        }
    }
}

void exec(std::string const & input, exec_env & env, std::ostream & out)
{
    if (input[input.size() - 1] == '&')
    {
        std::string substr = input.substr(0, input.size() - 1);
        std::cout << "start background " << substr << std::endl;
        auto f = std::async(std::launch::async, exec_impl, substr, std::ref(env), std::ref(out));

        env._mtx.lock();
        env._pending_futures.push_back(std::move(f));
        env._mtx.unlock();

        //new std::thread(std::ref(exec_impl), substr, std::ref(out));
    }
    else
    {
        exec_impl(input, env, out);
    }
}

struct input_reader {
public:
    void operator()() const {
        std::string line;
        std::cout << "thread stated " << std::endl;
        exec_env env;
        while (std::getline(std::cin, line))
        {
            exec(line, env, std::cout);
        }

    }
};

int main(int argc, char **argv)
{
    /*std::string lala("lalalalala");
    std::vector<std::string> t0;
    t0.reserve(20000000);
    for (size_t j = 0; j < 10; ++j)
    {
        {
        clock_t current_time = clock();
        for (size_t i = 0; i < 20000000; ++i)
        {
            t0.emplace_back(lala, 1, 4);
        }
        std::cout << "time 1 " << float( clock() - current_time ) / CLOCKS_PER_SEC << std::endl;
        t0.clear();
        }
        {
        clock_t current_time = clock();
        for (size_t i = 0; i < 20000000; ++i)
        {
            t0.push_back(lala.substr(1,4));
        }
        std::cout << "time 0 " << float( clock() - current_time ) / CLOCKS_PER_SEC << std::endl;
        t0.clear();
        }
    }*/

    QGuiApplication app(argc, argv);
    QSurfaceFormat format;
    format.setSamples(16);
    /*TriangleWindow window2;
    window2.setFormat(format);
    window2.resize(500, 480);
    window2.show();

    window2.setAnimating(true);
    std::string str;
    std::cin >> str;*/
    window = new TriangleWindow();


    //const clock_t begin_time = clock();
    //exec("run " + std::string(argv[1]));
    //std::cout << "time " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << std::endl;
    std::setlocale(LC_ALL, "C");
    input_reader reader;
    std::thread t1(reader);
    if (argc > 1)
    {
        std::string tmp = "run " + std::string(argv[1]);
        exec_env env;
        std::thread load_project(std::ref(exec_impl), tmp, std::ref(env), std::ref(std::cout));
        load_project.join();
    }
    window->setFormat(format);
    window->resize(1500, 480);
    window->show();

    window->setAnimating(true);

    int ret = app.exec();
    delete window;
    return ret;
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

std::ostream & operator<<(std::ostream & out, QMatrix4x4 const & mat)
{
    float const *data = mat.data();
    out << '(';
    for (size_t i = 0; i < 4; ++i)
    {
        print_elements(out, data + i * 4, data + i * 4 + 4, ' ') << ',';
    }
    return out << ')';
}


template <typename T>
T interpolated(std::map<size_t, T> const & map, size_t frame)
{
    auto up = map.lower_bound(frame);
    if (up->first == frame)
    {
        return up->second;
    }
    auto low = up;
    --low;
    //auto up = map.upper_bound(frame);
    float value = static_cast<float>(frame - low->first) / (up->first - low->first);
    //std::cout << value << '=' << '(' << frame  << '-' << low->first << ") / (" << up->first << '-'<< low->first << ')'<< std::endl;
    return interpolate(low->second, up->second, value);
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

template <typename K, typename T>
class interpolation_iterator
{
    std::map<K, T> const & _map;
    typename std::map<K, T>::const_iterator _low;
    typename std::map<K, T>::const_iterator _high;
    
public:
    interpolation_iterator(std::map<K, T> const & map_): _map(map_)
    {
    }   
};

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

static QOpenGLPaintDevice *qogpd = nullptr;

static const GLfloat g_quad_texture_coords[] = {
    1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    -1.0f, -1.0f,
};

static const GLfloat g_quad_vertex_buffer_data[] = {
    1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    -1.0f, -1.0f,
};

void render_cubemap(GLuint *renderedTexture)
{

    for (size_t i = 0; i < 6; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderedTexture[i]);
        glUniform1i(window->remapping_shader._texAttr[i], i);
    }
    glVertexAttribPointer(window->remapping_shader._posAttr, 2, GL_FLOAT, GL_FALSE, 0, g_quad_vertex_buffer_data);
    glVertexAttribPointer(window->remapping_shader._corAttr, 2, GL_FLOAT, GL_FALSE, 0, g_quad_texture_coords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

clock_t last_rendertime = 0;

void render_to_screenshot(screenshot_handle_t & current, GLuint **cubemaps, size_t loglevel)
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
        throw std::runtime_error("Error no framebuffer:" + std::to_string(framebufferStatus));

    glViewport(0,0,swidth,sheight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);

    render_cubemap(cubemaps[current._type] + 6 * std::distance(scene._cameras.data(), cam));

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


void TriangleWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        ++perm;
    }
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
    QPoint curser_pos = mapFromGlobal(QCursor::pos());
    //std::cout << p.x() << ' ' << p.y()  << std::endl;
    const clock_t current_time = clock();
    //std::cout << "fps " << CLOCKS_PER_SEC / float( current_time - last_rendertime ) << std::endl;
    last_rendertime = current_time;

    if (loglevel > 5)
    {
        std::cout << "start render" << std::endl;
    }
    if (reload_shader)
    {
        initialize();
        reload_shader = false;
    }
    size_t resolution = preresolution;
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale , height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 cubemap_transforms[6];
    for (size_t i = 0; i <6; ++i)
    {
        cubemap_transforms[i].perspective(90.0f, 1.0f/1.0f, 0.1f, 1000.0f);
    }
    //settransform left_eye rot 0.7 0.7 0 0
    cubemap_transforms[0].rotate(180, 0, 0, 1);
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
    for (size_t i = 0; i < 6; ++i)
    {
        cubemap_transforms[i].rotate(-90,1,0,0);
        cubemap_transforms[i].rotate(90,0,0,1);
    }
    size_t num_cams = scene._cameras.size();
    size_t num_views = static_cast<size_t>(show_raytraced) + static_cast<size_t>(show_position) + static_cast<size_t>(show_index) + static_cast<size_t>(show_flow) + static_cast<size_t>(show_depth);
    views.clear();

    marker.clear();
    QPointF curserViewPos;
    if (num_cams != 0)
    {
        curserViewPos.setX((curser_pos.x() % (width() / num_cams))/static_cast<float>(width() / num_cams));
        curserViewPos.setY((curser_pos.y() % (height() / num_views))/static_cast<float>(height() / num_views));
        if (loglevel > 5)
        {
            std::cout << curserViewPos.x() << ' ' << curserViewPos.y() << '\t';
        }
    }

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
        if (show_raytraced)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedTexture + c * 6, x, y, w, h, false, false}));
        }
        if (show_position)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedPositionTexture + c * 6, x, y, w, h, false, false}));
        }
        if (show_index)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedIndexTexture + c * 6, x, y, w, h, false, false}));
        }
        if (show_flow)
        {
            size_t y = (i++) * height() / num_views;
            views.push_back(view_t({cam._name, renderedFlowTexture + c * 6, x, y, w, h, true, false}));
        }
        if (show_depth)
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
    for (size_t c = 0; c < scene._cameras.size(); ++c)
    {
        camera_t & cam = scene._cameras[c];
        QMatrix4x4 &cam_transform_cur = camera_transformations[c];
        QMatrix4x4 cam_transform_pre;
        QMatrix4x4 cam_transform_post;
        transform_matrix(cam, cam_transform_pre, m_frame + (difftrans ? diffbackward : 0), smoothing, m_frame + (diffrot ? diffbackward : 0), smoothing);
        transform_matrix(cam, cam_transform_post, m_frame + (difftrans ? diffforward : 0), smoothing, m_frame + (diffrot ? diffforward: 0), smoothing);
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
            if (f == 5 && fov < 120)
            {
                continue;
            }
            if (f != 4 && fov <= 45)
            {
                continue;
            }
            glBindTexture(GL_TEXTURE_2D, renderedTexture[f + c * 6]);
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, resolution, resolution, 0,GL_BGRA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, renderedFlowTexture[f + c * 6]);
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB16F, resolution, resolution, 0,GL_BGR, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, renderedPositionTexture[f + c * 6]);
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F, resolution, resolution, 0,GL_BGRA, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, renderedIndexTexture[f + c * 6]);
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, resolution, resolution, 0,GL_BGRA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, renderedDepthTexture[f + c * 6]);
            glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
            //glTexImage2D (GL_TEXTURE_2D, 0,GL_R32F, resolution, resolution, 0,GL_RED, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // glBindRenderbuffer(GL_RENDERBUFFER, renderedDepthTexture[f + c * 6]);
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, resolution, resolution);
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderedDepthTexture[f + c * 6]);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture[f + c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderedFlowTexture[f + c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderedPositionTexture[f + c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderedIndexTexture[f + c * 6], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderedDepthTexture[f + c * 6], 0 );
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, renderedDepthTexture[f + c * 6], 0);

            // Set the list of draw buffers.
            GLenum DrawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
            glDrawBuffers(4, DrawBuffers);

            // Always check that our framebuffer is ok
            GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Error, no Framebuffer:" + std::to_string(frameBufferStatus));

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
                mesh.load_meshes();
                QMatrix4x4 object_transform_pre;
                QMatrix4x4 object_transform_cur;
                QMatrix4x4 object_transform_post;
                transform_matrix(mesh, object_transform_pre, m_frame + diffbackward, smoothing, m_frame + diffbackward, smoothing);
                transform_matrix(mesh, object_transform_cur, m_frame, smoothing, m_frame, smoothing);
                transform_matrix(mesh, object_transform_post, m_frame + diffforward, smoothing, m_frame + diffforward, smoothing);
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
                mesh.load_textures();
                //GLUint orig;
                if (mesh.tex_Kd != nullptr)
                {
                    glActiveTexture(GL_TEXTURE0);
                    mesh.tex_Kd->bind();
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
                if (mesh.tex_Kd != nullptr)
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
    
    GLuint *texturePointer[5] = {renderedTexture, renderedPositionTexture, renderedDepthTexture, renderedFlowTexture, renderedIndexTexture};
    QVector4D curser_3d;
    if (show_arrows)
    {
        setShaderBoolean(*remapping_shader._program, remapping_shader._diffUniform, "diff", true);
        setShaderBoolean(*remapping_shader._program, remapping_shader._depthUniform, "depth", false);
        for (size_t icam = 0; icam < num_cams; ++icam)
        {
            screenshot_handle_t current;
            current._width = 1024;
            current._height = 1024;
            current._channels = 2;
            current._type = VIEWTYPE_FLOW;
            current._ignore_nan = true;
            current._datatype = GL_FLOAT;
            current._data = nullptr;
            current._error_code = 0;
            current._camera = scene._cameras[icam]._name;
            render_to_screenshot(current, texturePointer, loglevel);
            
            float *data = reinterpret_cast<float*>(current._data);
            for (size_t y = 0; y < 1024; y += 64)
            {
                for (size_t x = 0; x < 1024; x += 64)
                {
                    float xf = static_cast<float>(x) / 1024;
                    float yf = static_cast<float>(y) / 1024;
                    
                    size_t index = 2 * (y * 1024 + x);
                    float xdiff = data[index];
                    float ydiff = data[index + 1];
                    for (view_t & view : views)
                    {
                        if (view._camera == scene._cameras[icam]._name && view._cubemap_texture == renderedFlowTexture + icam * 6)
                        {
                            //std::cout << xf * view._width << ' '<< yf * view._height << ' ' << xdiff << ' '<< ydiff<<std::endl;
                            arrows.emplace_back(arrow_t({xf * view._width + view._x, height() - yf * view._height - view._y, xdiff, -ydiff}));
                        }
                    }
                }
            }
            delete[] data;
        }
    }
    if (show_curser && num_cams != 0)
    {
        setShaderBoolean(*remapping_shader._program, remapping_shader._diffUniform, "diff", false);
        setShaderBoolean(*remapping_shader._program, remapping_shader._depthUniform, "depth", false);
        screenshot_handle_t current;
        current._width = 1024;
        current._height = 1024;
        current._channels = 3;
        current._type = VIEWTYPE_POSITION;
        current._ignore_nan = true;
        current._datatype = GL_FLOAT;
        current._data = nullptr;
        current._error_code = 0;
        current._camera = scene._cameras[clamp(curser_pos.x() * num_cams / width(), size_t(0), num_cams-1)]._name;
        render_to_screenshot(current, texturePointer, loglevel);
        size_t sy = 1024 - static_cast<size_t>(curserViewPos.y() * 1024);
        size_t sx = static_cast<size_t>(curserViewPos.x() * 1024);
        if (sx < 1024 && sy < 1024)
        {
            size_t index = 3 * (sy * 1024 + sx);
            if (loglevel > 5)
            {
                std::cout << index << '\t';
            }
            float *data = reinterpret_cast<float*>(current._data);
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
                        tmp[icam] = tmp[icam] * 0.5 * 4 * fov / 90.;
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
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);  
    
    for (view_t & view : views)
    {
        setShaderBoolean(*remapping_shader._program, remapping_shader._diffUniform, "diff", view._diff);
        setShaderBoolean(*remapping_shader._program, remapping_shader._depthUniform, "depth", view._depth);

        glViewport(view._x, view._y, view._width, view._height);

        render_cubemap(view._cubemap_texture);
    }

    
    {
        for (size_t read = 0; read < _screenshot_handles.size(); ++read)
        {
            screenshot_handle_t & current = *_screenshot_handles[read];

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
                    render_to_screenshot(current, texturePointer, loglevel);
                    std::cout << "rendered_screenshot" << std::endl;
                }
                else
                {
                    current._error_code = 1;
                }
            }
        }
        _screenshot_handles.clear();

    }
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
    for (size_t i = 0; i < scene._framelists.size(); ++i)
    {
        bool found = std::binary_search(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), m_frame);
        painter.setPen(QColor((!found) * 255,found * 255,0,255));
        painter.drawText(30, i*30 + 60, QString(scene._framelists[i]._name.c_str()));
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
                    m_frame = *(iter);
                }
            }
        }
    }
    painter.end();
    if (screenshot != "")
    {
        awx_ScreenShot(screenshot);
        screenshot = "";
    }
    m_frame += play;
    ++_rendered_frames;
    size_t write = 0;
    for (size_t read = 0; read < wait_for_rendered_frame_handles.size(); ++read)
    {
        if (wait_for_rendered_frame_handles[read]->_frame < _rendered_frames)
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
    if (exit_program)
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
