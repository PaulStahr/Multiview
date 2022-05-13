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
#include "image_io.h"
#include "io_util.h"

//void *glMapBuffer(GLenum target,GLenum access);

//LIBS += -lImath -lHalf -lIex -lIexMath -lIlmThread -lIlmImf -ldl -lboost_system -lboost_filesystem -lQt5Widgets

//vec3 vertex_vector = vertex.xyz - center; //vertex = gl_Vertex, center = the patch's center = the camera positiondepth = length(vertex_vector)/100.0; //depth is a varyingvertex_vector = normalize(vertex_vector);//http://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projectionfloat z_term = pow( 2.0/(1.0-vertex_vector.z), 0.5 );vec2 coord = z_term*vertex_vector.xy; // in range (-2,2)coord = ((coord+vec2(2.0))/4.0);//*vec2(512.0,512.0); //in range (0,1)vertex.xy = coord;vertex.z = 0.0;gl_Position = vertex;

//color.rgb = indices; //default values of patch, just so we can see what's going ongl_FragDepth = depth;

//OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER


//OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT

struct input_reader {
public:
    exec_env &env;
    session_t &_session;
    input_reader(exec_env & env_, session_t & session_) : env(env_), _session(session_){}
    void operator()() const {
        std::string line;
        while (std::getline(std::cin, line))
        {
            try
            {
                exec(line, std::vector<std::string>(), const_cast<input_reader*>(this)->env, std::cout, _session, const_cast<input_reader*>(this)->env.emitPendingTask(line));
            }catch(std::exception const & ex)
            {
                std::cout << ex.what() << std::endl;
            }
        }
    }
};

struct command_executer_t{
    exec_env env;
    session_t &_session;
    command_executer_t(session_t & session_) : env(IO_UTIL::get_programpath()), _session(session_){}

    command_executer_t(const command_executer_t&) = delete;

    command_executer_t(command_executer_t&& other) = default;

    void operator()(std::string str, std::ostream & out)//TODO why no reference?
    {
        exec(str, std::vector<std::string>(), env, out, _session, env.emitPendingTask(str));
    }

    ~command_executer_t()
    {
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    QSurfaceFormat format;
    format.setSamples(16);
    format.setSwapInterval(0);
    /*
    QSurfaceFormat format, set format.setSwapInterval(0), then QSurfaceFormat::setDefaultFormat(format)
    */
    std::shared_ptr<destroy_functor> exit_handler = std::make_shared<destroy_functor>([](){std::cout << "quit" << std::endl; QApplication::quit();});

    RenderingWindow *window = new RenderingWindow(exit_handler);
    WorkerThread wt([window](){
        window->rendering_loop();
    });
    window->set_worker(wt);

    session_t & session = window->session;
    Ui::ControlWindow cw;
    ControlWindow *widget = new ControlWindow(session, cw, exit_handler);
    exit_handler = nullptr;
    widget->updateUiSignal(UPDATE_SESSION);
    widget->show();

    std::setlocale(LC_ALL, "C");
    CommandServer server;
    //command_executer_t executer(session);
    exec_env server_env(IO_UTIL::get_programpath());
    server.setCommandExecutor([&server_env, &session](std::string str, std::ostream & out){exec(str, std::vector<std::string>(), server_env, out, session, server_env.emitPendingTask(str));});

    exec_env command_env(IO_UTIL::get_programpath());
    input_reader reader(command_env, session);
    std::thread input_reader_thread(reader);
    std::thread command_argument_thread([argc, &argv,&session]{      
        exec_env argument_env(IO_UTIL::get_programpath());
        for (int i = 1; i < argc; ++i)
        {
            if (std::strcmp(argv[i],"-s")==0)
            {
                if (argc < i + 1){throw std::runtime_error("Argument required");}
                std::string tmp = std::string(argv[i + 1]);
                tmp = "run " + tmp;
                exec(tmp, std::vector<std::string>(), std::ref(argument_env), std::ref(std::cout), std::ref(session), std::ref(argument_env.emitPendingTask(tmp)));
                i += 1;
            }
            else if (std::strcmp(argv[i],"-c")==0)
            {
                if (argc < i + 1){throw std::runtime_error("Argument required");}
                std::string tmp = std::string(argv[i + 1]);
                std::replace(tmp.begin(), tmp.end(), ';','\n');
                exec(tmp, std::vector<std::string>(), std::ref(argument_env), std::ref(std::cout), std::ref(session), std::ref(argument_env.emitPendingTask(tmp)));
                i += 1;
            }
            else
            {
                throw std::runtime_error(std::string("Unknown argument: ") + argv[i]);
            }
        }
    });

    window->setFormat(format);
    window->resize(1500, 480);
    window->show();

    window->setAnimating(true);

    app.exec();

    //while (!window->destroyed){}
    input_reader_thread.detach();
    image_io_destroy();
    command_argument_thread.join();
    return 0;
}
