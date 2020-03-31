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
#define GL_GLEXT_PROTOTYPES

/*
 * #ifndef GL_EXT_PROTOTYPES
#define GL_EXT_PROTOTYPES 1
#endif

and

#include <GLES2/gl2ext.h>
*/

//#include <GL/glew.h>

#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
//#include <libxml2/libxml/parser.h>
#include <QtCore/qmath.h>
//#include <OpenGLES/ES2/glext.h> 
#include <iostream>
#include <string>
#include <fstream>
#include <future>
#include <queue>
#include <thread>
#include <sstream>
#include <map>
#include <mutex>
#include <cstdint>
#include <chrono>
#include "serialize.h"
#include "main.h"
#include "image_util.h"
#include "server.h"
#include "session.h"
#include "data.h"
#include "control_window.h"
#include "control_ui.h"
#include "rendering_view.h"

//void *glMapBuffer(GLenum target,GLenum access);

//LIBS += -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf -ldl -lboost_system -lboost_filesystem -lQt5Widgets

//vec3 vertex_vector = vertex.xyz - center; //vertex = gl_Vertex, center = the patch's center = the camera positiondepth = length(vertex_vector)/100.0; //depth is a varyingvertex_vector = normalize(vertex_vector);//http://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projectionfloat z_term = pow( 2.0/(1.0-vertex_vector.z), 0.5 );vec2 coord = z_term*vertex_vector.xy; // in range (-2,2)coord = ((coord+vec2(2.0))/4.0);//*vec2(512.0,512.0); //in range (0,1)vertex.xy = coord;vertex.z = 0.0;gl_Position = vertex;

//color.rgb = indices; //default values of patch, just so we can see what's going ongl_FragDepth = depth;

//OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER


//OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT



//objl::Loader Loader;



/*template <class T, size_t N>
struct valcarray  : std::array<T,N>
{
};*/

/*template<typename T, typename std::enable_if<std::is_base_of<MyClass, T>::value>::type* = nullptr>
T Foo(T bar)
{
    return T();
}*/




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

//std::vector<std::thread *> executing_threads;

/*template <typename T>
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
}*/

struct input_reader {
public:
    exec_env &env;
    session_t *_session;
    input_reader(exec_env & env_, session_t & session_) : env(env_), _session(&session_){}
    void operator()() const {
        std::string line;
        std::cout << "thread stated " << std::endl;
        while (std::getline(std::cin, line))
        {
            exec(line, const_cast<input_reader*>(this)->env, std::cout, *_session, const_cast<input_reader*>(this)->env.emitPendingTask());
        }
    }
};

struct command_executer_t{
    exec_env *env;
    session_t *_session;
    command_executer_t(session_t & session_) : env(new exec_env(IO_UTIL::get_programpath())), _session(&session_){}
    
    command_executer_t(const command_executer_t&) = delete;
    
    
    command_executer_t(command_executer_t&& other)
    {
        env = std::move(other.env);
        _session = std::move(other._session);
    } 
    
    void operator()(std::string str, std::ostream & out)
    {
        exec(str, *env, out, *_session, env->emitPendingTask());
    }
    
    ~command_executer_t()
    {
        delete env;
        env = nullptr;
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QSurfaceFormat format;
    format.setSamples(16);
    /*TriangleWindow window2;
    window2.setFormat(format);
    window2.resize(500, 480);
    window2.show();

    window2.setAnimating(true);
    std::string str;
    std::cin >> str;*/
    TriangleWindow *window = new TriangleWindow();

    Ui::ControlWindow cw;
    ControlWindow *widget = new ControlWindow(window->session, cw);
    cw.setupUi(widget);
    widget->updateUiSignal(UPDATE_SESSION);
    widget->show();

    //const clock_t begin_time = clock();
    //exec("run " + std::string(argv[1]));
    //std::cout << "time " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << std::endl;
    std::setlocale(LC_ALL, "C");
    CommandServer server;
    //command_executer_t executer(window->session);
    exec_env server_env(IO_UTIL::get_programpath());
    server.setCommandExecutor([&server_env, window](std::string str, std::ostream & out){exec(str, server_env, out, window->session, server_env.emitPendingTask());});
    
    exec_env command_env(IO_UTIL::get_programpath());
    input_reader reader(command_env, window->session);
    std::thread input_reader_thread(reader);
    exec_env argument_env(IO_UTIL::get_programpath());
    if (argc > 1)
    {
        std::string tmp = "run " + std::string(argv[1]) + "&";
        exec(tmp, std::ref(argument_env), std::ref(std::cout), std::ref(window->session), std::ref(argument_env.emitPendingTask()));
        /*Joins because env has to be destructed*/
    }
    window->setFormat(format);
    window->resize(1500, 480);
    window->show();

    window->setAnimating(true);
    app.exec();
    //std::thread t2([&app](){app.exec();});
    
    //while (!window->destroyed){}
    //delete window;
    return 0;
}
