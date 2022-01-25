#include "session.h"

#include <future>
//#include <filesystem>
#include <experimental/filesystem>
#include <boost/algorithm/string/case_conv.hpp>

#include "lang.h"
#include "python_binding.h"


//namespace fs = std::filesystem;
namespace fs = std::experimental::filesystem;

void session_t::scene_update(SessionUpdateType sup)
{
    _scene_updates.emplace_back(sup);
    for (auto & f : _updateListener)
    {
        (*f)(sup);
    }
}

void session_t::wait_for_frame(wait_for_rendered_frame_t & wait_obj)
{
    std::lock_guard<std::mutex> lockGuard(_scene._mtx);
    _wait_for_rendered_frame_handles.push_back(&wait_obj);
}

viewtype_t get_viewtype(std::string const & str)
{
    if      (str == "rendered") {return VIEWTYPE_RENDERED;}
    else if (str == "position") {return VIEWTYPE_POSITION;}
    else if (str == "index")    {return VIEWTYPE_INDEX;}
    else if (str == "depth")    {return VIEWTYPE_DEPTH;}
    else if (str == "flow")     {return VIEWTYPE_FLOW;}
    else                        {throw std::runtime_error("type not known");}
}

void assert_argument_count(size_t n, size_t m)
{
    if (n > m){throw std::runtime_error("At least " + std::to_string(n) + " arguments required, but only " + std::to_string(m) + " were given");}
}

/*template <typename T>
bool parse_or_print(T & value, std::ostream & out)
{
    if (args.size() > 1)
    {
        size_t tmp = std::stoi(args[1]);
        if (tmp != *ref_size_t)
        {
            *ref_size_t = tmp;
            return true;
        }
    }
    else
    {
        out << *ref_size_t << std::endl;
        return false;
    }
}*/

void exec_impl(std::string input, exec_env & env, std::ostream & out, session_t & session, pending_task_t &pending_task)
{
    try
    {
        while (input.back() == 10){input.pop_back();}
        std::vector<std::string> args;
        scene_t & scene = session._scene;
        IO_UTIL::split_in_args(args, input);
        if (args.size() == 0){
            pending_task.assign(PENDING_NONE);
            return;
        }
        size_t *ref_size_t = nullptr;
        //uint32_t *ref_uint32_t = nullptr;
        int32_t *ref_int32_t = nullptr;
        float *ref_float_t = nullptr;
        bool *ref_bool = nullptr;
        SessionUpdateType session_var = UPDATE_NONE;
        SessionUpdateType session_update = UPDATE_NONE;
        counting_semaphore_guard(env.num_threads_);
        std::string const & command = args[0];
        if (command == "help")
        {
            out << "status" << std::endl;
            out << "frame (<frame>)" << std::endl;
            out << "play (<speed>)" << std::endl;
            out << "loglevel (<ivalue>)" << std::endl;
            out << "next (<frames>)"<< std::endl;
            out << "prev (<frames>)"<< std::endl;
            out << "show_only (<framelist>)" << std::endl;
            out << "smoothing (<frames>)" << std::endl;
            out << "diffbackward (<num_frames>)" <<std::endl;
            out << "diffforward (<num_frames>)" << std::endl;
            out << "diffnormalize (<activated>)"<< std::endl;
            out << "difffallback (<activated>)" << std::endl;
            out << "screenshot <filename>" << std::endl;
            out << "screenshot2 <filename> <width> <height> <camera> <type> (<export nan>)" << std::endl;
            out << "camera <name>" << std::endl;
            out << "diffrot (<activated>)" << std::endl;
            out << "difftrans (<activated>)" << std::endl;
            out << "preresolution (<num_pixels>)" << std::endl;
            out << "coordinate_system (<spherical_approximated|spherical_singlepass|spherical_multipass>)" << std::endl;
            out << "modify <object> <transform|visibility|difftrans|diffrot|trajectory|wireframe> (<...>)" << std::endl;
            out << "echo <...>" << std::endl;
            out << "run <scriptfile>" << std::endl;
            out << "exec <command>" << std::endl;
            out << "animating <always|automatic|manual>" << std::endl;
            out << "python <scriptfile.py>" << std::endl;
            out << "autouiupdate <activated>" << std::endl;
            out << "wait -> wait for next redraw" << std::endl;
            out << "join (<thread sread swrite fread fwrite all>)-> wait for all tasks in the pipeline to fininsh" << std::endl;
            out << "framelist <filename> <name>" <<std::endl;
            out << "object <name> <filename> (<transformation>)" << std::endl;
            out << "id <name> (<id-value>)" << std::endl;
            out << "anim <filename> <transformations>" << std::endl;
            out << "depthbuffersize (<16|24|32>)" << std::endl;
            out << "load <session_file>" << std::endl;
            out << "save <session_file>" << std::endl;
            out << "max_threads" << std::endl;
        }
        else if (command == "load")
        {
                
        }
        else if (command == "save")
        {
            
        }
        else if (command == "max_threads")
        {
            if (args.size() < 1)
            {
                out << env.num_threads_.get_max() << std::endl;
            }
            else
            {
                env.num_threads_.set_max(std::stoi(args[1]));
            }
            
        }
        else if (command == "show_only")
        {
            session._show_only = args.size() > 1 ? args[1] : "";
        }
        else if (command == "frame" || command == "goto")   {ref_int32_t = &session._m_frame;   session_var |= UPDATE_FRAME;}
        else if (command == "play")                         {ref_int32_t = &session._play;}
        else if (command == "coordinate_system")            {
            if (args.size() > 1)
            {
                coordinate_system_t coordinate_system;
                if      (args[1] == "spherical_approximated"){coordinate_system = COORDINATE_SPHERICAL_APPROXIMATED;}
                else if (args[1] == "spherical_singlepass")  {coordinate_system = COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS;}
                else if (args[1] == "spherical_multipass")   {coordinate_system = COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS;}
                else{throw std::runtime_error("Option " + args[1] + " not known");}
                if (coordinate_system != session._coordinate_system)
                {
                    session._coordinate_system = coordinate_system;
                    session.scene_update(UPDATE_SESSION);
                }
            }
            else
            {
                switch(session._coordinate_system)
                {
                    case COORDINATE_SPHERICAL_APPROXIMATED:         out << "spherical_approximated"    << std::endl; break;
                    case COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS:   out << "spherical_singlepass"      << std::endl; break;
                    case COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS:    out << "spherical_multipass"       << std::endl; break;
                    default:                throw std::runtime_error("Unknown redraw type");
                }
            }
        }
        else if (command == "animating")
        {
            if (args.size() > 1)
            {
                RedrawScedule animating = lang::gt_animating_value(args[1].c_str());
                if (animating == REDRAW_INVALID){throw std::runtime_error("Option " + args[1] + " not known");}
                if (animating != session._animating)
                {
                    session._animating = animating;
                    session.scene_update(UPDATE_ANIMATING);
                }
            }
            else
            {
                const char* res = lang::get_animating_string(session._animating);
                if (!res){throw std::runtime_error("Unknown redraw type");}
                out << boost::algorithm::to_lower_copy(std::string(res))<< std::endl;
            }
        }
        else if (command == "status")
        {
            out << "textures" << std::endl;
            for (texture_t const & t: scene._textures)
            {
                out << t._id << ' ' << t._width << ' ' << t._height << ' ' << t._channels << ' ' << t._type << ' ' << t._name << std::endl;
            }
            out << "scene" << std::endl;
            out << "frame " << session._m_frame << std::endl;
            out << "texture_ids " << gl_texture_id::count << std::endl;
            out << "framelocks" << std::endl;
            for (wait_for_rendered_frame_t *wait_obj : session._wait_for_rendered_frame_handles)
            {
                out << *wait_obj << std::endl;
            }
            out << "screenshothandles" << std::endl;
            for (screenshot_handle_t *handle :scene._screenshot_handles)
            {
                out << *handle << std::endl;
            }
            out << "pending tasks" << std::endl;
            for (auto t : env._pending_tasks)
            {
                out << t << ' ' << t->_flags << ':' << t->_description << std::endl;
            }
        }
        else if (command == "notify")
        {
            for (wait_for_rendered_frame_t *wait_obj : session._wait_for_rendered_frame_handles){wait_obj->_cv.notify_all();}
            for (screenshot_handle_t *handle :scene._screenshot_handles)        {handle->_cv.notify_all();}
            for (pending_task_t *t : env._pending_tasks)        {t->_cond_var.notify_all();}
        }
        else if (command == "redraw")       {session.scene_update(UPDATE_REDRAW);}
        else if (command == "loglevel")     {ref_size_t = &session._loglevel;}
        else if (command == "debug")        {ref_bool = &session._debug;}
        else if (command == "next")
        {
            if (args.size() > 1){session._m_frame += std::stoi(args[1]);}
            else                {++session._m_frame;}
            session_update |= UPDATE_FRAME;
        }
        else if (command == "prev")
        {
            if (args.size() > 1){session._m_frame -= std::stoi(args[1]);}
            else                {--session._m_frame;}
            session_update |= UPDATE_FRAME;
        }
        else if (command == "diffbackward") {ref_int32_t = &session._diffbackward;session_var |= UPDATE_SESSION;}
        else if (command == "diffforward")  {ref_int32_t = &session._diffforward; session_var |= UPDATE_SESSION;}
        else if (command == "difffallback") {ref_bool    = &session._difffallback;session_var |= UPDATE_SESSION;}
        else if (command == "diffnormalize"){ref_bool    = &session._diffnormalize;session_var|= UPDATE_SESSION;}
        else if (command == "screenshot")
        {
            session._screenshot = args[1];
        }
        else if (command == "screenshot2")
        {
            screenshot_handle_t handle;
            handle._ignore_nan = false;
            handle._prerendering = std::numeric_limits<size_t>::max();
            handle._task = TAKE_SCREENSHOT;
            assert_argument_count(6, args.size());
            if (args.size() > 6)
            {
                handle._ignore_nan = std::stoi(args[6]);
                if (args.size() > 7)
                {
                    for (size_t k = 7; k < args.size(); ++k)
                    {
                        std::string const & arg = args[k];
                        if      (arg == "pre")      {handle._prerendering = std::stoi(args[++k]);}
                        else if (arg == "vcam")     {handle._vcam.push_back(args[++k]);}
                        else                        {out << "Unknown argument" << arg << std::endl;}
                    }
                }
            }
            handle._type = get_viewtype(args[5]);
            std::string const & output = args[1];
            pending_task.assign(PENDING_FILE_WRITE | PENDING_TEXTURE_READ | PENDING_SCENE_EDIT);
            handle._camera= args[4];
            handle._width = std::stoi(args[2]);
            handle._height = std::stoi(args[3]);
            handle._channels = ends_with(output, ".exr") ? 0 : handle._type == VIEWTYPE_INDEX ? 1 : 3;
            handle.set_datatype(ends_with(output, ".exr") ? GL_FLOAT : GL_UNSIGNED_BYTE);
            handle._state = screenshot_state_inited;
            handle._flip = true;
            std::cout << handle._id << " queue screenshot" << std::endl;
            scene.queue_handle(handle);
            pending_task.unset(PENDING_SCENE_EDIT);
            handle.wait_until(screenshot_state_rendered_texture);
            pending_task.unset(PENDING_TEXTURE_READ);
            handle.wait_until(screenshot_state_copied);
            if (handle._state == screenshot_state_error)
            {
                out << handle._id << " error at getting texture" << std::endl;
            }
            else
            {
                out << handle._id << " success" << std::endl;
                save_lazy_screenshot(output, handle);
            }
            pending_task.unset(PENDING_FILE_WRITE);
        }
        else if (command == "write_texture")
        {
            assert_argument_count(3, args.size());
            screenshot_handle_t handle;
            handle._task = SAVE_TEXTURE;
            handle._texture = args[1];
            handle._state = screenshot_state_inited;
            handle.set_datatype(ends_with(args[2], ".exr") ? GL_FLOAT : GL_UNSIGNED_BYTE);
            pending_task.assign(PENDING_FILE_WRITE | PENDING_TEXTURE_READ | PENDING_SCENE_EDIT);
            scene.queue_handle(handle);
            pending_task.unset(PENDING_SCENE_EDIT);
            handle.wait_until(screenshot_state_copied);
            pending_task.unset(PENDING_TEXTURE_READ);
            if (handle._state == screenshot_state_error)
            {
                out << "error at getting texture" << std::endl;
            }
            else
            {
                out << "success" << std::endl;
                save_lazy_screenshot(args[2], handle);
            }
            pending_task.unset(PENDING_FILE_WRITE);
        }
        else if (command == "render_to_texture")
        {
            screenshot_handle_t handle;
            handle._prerendering = std::numeric_limits<size_t>::max();
            handle._ignore_nan = false;
            assert_argument_count(3, args.size());
            if (args.size() > 4)
            {
                handle._ignore_nan = std::stoi(args[4]);
                if (args.size() > 5)
                {
                    for (size_t k = 5; k < args.size(); ++k)
                    {
                        std::string const & arg = args[k];
                        if      (arg == "pre")      {handle._prerendering = std::stoi(args[++k]);}
                        else if (arg == "vcam")     {handle._vcam.push_back(args[++k]);}
                        else                        {out << "Unknown argument" << arg << std::endl;}
                    }
                }
            }
            handle._texture = args[1];
            handle._camera= args[2];
            handle._type = get_viewtype(args[3]);
            pending_task.assign(PENDING_FILE_WRITE | PENDING_TEXTURE_READ | PENDING_SCENE_EDIT);
            handle._task = RENDER_TO_TEXTURE;
            handle._state = screenshot_state_inited;
            handle._flip = true;
            std::cout << "queue screenshot" << std::endl;
            scene.queue_handle(handle);
            pending_task.unset(PENDING_SCENE_EDIT);
            handle.wait_until(screenshot_state_rendered_texture);
            pending_task.unset(PENDING_TEXTURE_READ);
            handle.wait_until(screenshot_state_copied);
            if (handle._state == screenshot_state_error)
            {
                out << "error at getting texture" << std::endl;
            }
            else
            {
                out << "success" << std::endl;
            }
            pending_task.unset(PENDING_FILE_WRITE);
        }
        else if (command == "diffrot")      {ref_bool    = &session._diffrot;        session_var |= UPDATE_SESSION;}
        else if (command == "difftrans")    {ref_bool    = &session._difftrans;      session_var |= UPDATE_SESSION;}
        else if (command == "smoothing")    {ref_size_t  = &session._smoothing;      session_var |= UPDATE_SESSION;}
        else if (command == "fov")          {ref_float_t = &session._fov;            session_var |= UPDATE_SESSION;}
        else if (command == "crop")         {ref_bool    = &session._crop;           session_var |= UPDATE_SESSION;}
        else if (command == "autouiupdate") {ref_bool    = &session._auto_update_gui;session_var |= UPDATE_SESSION;}
        else if (command == "culling")      {ref_size_t  = &session._culling;        session_var |= UPDATE_SESSION;}
        else if (command == "depthbuffersize")
        {
            if (args.size() > 1)
            {
                std::string const & depthstr = args[1];
                if      (depthstr == "16")  {session._depthbuffer_size = DEPTHBUFFER_16_BIT;}
                else if (depthstr == "24")  {session._depthbuffer_size = DEPTHBUFFER_24_BIT;}
                else if (depthstr == "32")  {session._depthbuffer_size = DEPTHBUFFER_32_BIT;}
                else                        {out << "Unknown Argument for depth" << std::endl;}
                session_update |= UPDATE_SESSION;
            }
            else
            {
                switch(session._depthbuffer_size)
                {
                    case DEPTHBUFFER_16_BIT: out << "16" << std::endl;break;
                    case DEPTHBUFFER_24_BIT: out << "24" << std::endl;break;
                    case DEPTHBUFFER_32_BIT: out << "32" << std::endl;break;
                    default:                 out << "Unknown Argument for depth" << std::endl;break;
                }
            }
        }
        else if (command == "reload")
        {
            assert_argument_count(2, args.size());
            if (args[1] == "shader")
            {
                session._reload_shader = true;
                session_update |= UPDATE_REDRAW;
            }
        }
        else if (command == "show")
        {
            assert_argument_count(2, args.size());
            bool *ref = nullptr;
            if (args[1] == "raytraced")     {ref = &session._show_raytraced;}
            else if (args[1] == "flow")     {ref = &session._show_flow;}
            else if (args[1] == "index")    {ref = &session._show_index;}
            else if (args[1] == "pos")      {ref = &session._show_position;}
            else if (args[1] == "depth")    {ref = &session._show_depth;}
            else if (args[1] == "curser")   {ref = &session._show_curser;}
            else if (args[1] == "arrows")   {ref = &session._show_arrows;}
            else if (args[1] == "framelists"){ref = &session._show_framelists;}
            else{out << "error, key not known" << std::endl;return;}
            if (args.size() > 2)
            {
                *ref = std::stoi(args[2]);
                session_update |= UPDATE_SESSION;
            }
            else
            {
                out << *ref << std::endl;
            }
        }
        else if (command == "preresolution"){ref_size_t = &session._preresolution;session_var |= UPDATE_SESSION;}
        else if (command == "echo")
        {
            print_elements(out, args.begin() + 1, args.end(), ' ') << std::endl;
        }
        else if (command == "cat")
        {
            std::string line;
            for (size_t i = 1; i < args.size(); ++i)
            {
                std::ifstream file(args[i]);
                while (std::getline(file, line))
                {
                    out << line;
                }
            }
        }
        else if (command == "modify")
        {
            assert_argument_count(3, args.size());
            std::string const & name = args[1];
            object_t *obj = scene.get_object(name);
            if (!obj){throw std::runtime_error("object not found");}
            bool *refb = nullptr;
            if (args[2] == "transform")
            {
                obj->_transformation.setToIdentity();
                read_transformations(obj->_transformation, args.begin() + 3, args.end());
            }
            else if (args[2] == "visible")   {refb = &obj->_visible;}//TODO
            else if (args[2] == "difftrans") {obj->_difftrans  = std::stoi(args[3]);}
            else if (args[2] == "diffrot")   {obj->_diffrot    = std::stoi(args[3]);}
            else if (args[2] == "trajectory"){obj->_trajectory = std::stoi(args[3]);}
            else if (args[2] == "aperture")  {static_cast<camera_t*>(obj)->_aperture = std::stof(args[3]);}
            else if (args[2] == "wireframe") {static_cast<camera_t*>(obj)->_wireframe = std::stoi(args[3]);}
            else
            {
                out << "error, key not known, valid keys are transform, visible" << std::endl;
            }
            if (refb)
            {
                *refb = std::stoi(args[3]);
            }
            session_update |= UPDATE_SCENE;
        }
        else if (command == "print")
        {
            std::string name = args[1];
            object_t *obj = scene.get_object(args[1]);
            if (!obj){throw std::runtime_error("object not found");}
            if (args[2] == "transform")
            {
                std::cout << obj->_transformation << obj->_key_pos[session._m_frame] << ' ' << obj->_key_rot[session._m_frame] << std::endl;
            }
        }
        else if (command == "run")
        {
            std::ifstream infile(args[1]);
            std::string line;
            fs::path script = args[1];
            exec_env subenv(script.parent_path());
            if (!infile)
            {
                std::cout << "error bad file: " << args[1] << std::endl;
            }
            std::vector<std::string> vars(args.begin() + 1, args.end());
            while(std::getline(infile, line))
            {
                std::cout << line << std::endl;
                exec(line, vars, subenv, out, session, subenv.emitPendingTask(line));
            }
            subenv.join(&pending_task, PENDING_ALL);
            infile.close();
        }
        else if (command == "python")
        {
            assert_argument_count(2, args.size());
            std::string exec_file = args[1];
            PYTHON::run(exec_file, &session);
        }
        else if (command == "exec")
        {
            assert_argument_count(2, args.size());
            std::string exec_command = args[1];
            for (auto iter = args.begin() + 2; iter < args.end(); ++iter)
            {
                exec_command.push_back(' ');
                exec_command += *iter;
            }
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(exec_command.c_str(), "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            //for windows _popen and _pclose
            exec_env subenv(args[1]);
            std::array<char, 1024> buffer;
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                std::string line = buffer.data();
                if (line.back() == '\n'){line.pop_back();}
                if (line.empty()){continue;}
                std::cout << "out " << line << std::endl;
                exec(line, std::vector<std::string>(args.begin() + 1, args.end()), subenv, out, session, subenv.emitPendingTask(line));
            }
        }
        else if (command == "delete")
        {
            assert_argument_count(3, args.size());
            if (args[1] == "camera")
            {
                std::string const & name = args[2];
                std::lock_guard<std::mutex> lck(scene._mtx);
                camera_t *cam = scene.get_camera(name);
                if (!cam){throw std::runtime_error("Can't find camera " + name);}
                scene._cameras.erase(static_cast<std::vector<camera_t>::iterator>(cam));
            }
            else if (args[1] == "texture")
            {
                std::string const & name = args[2];
                std::lock_guard<std::mutex> lck(scene._mtx);
                texture_t *tex = scene.get_texture(name);
                if (!tex){throw std::runtime_error("Can't find texture " + name);}
                scene._textures.erase(static_cast<std::vector<texture_t>::iterator>(tex));
            }
            else if (args[1] == "object")
            {
                std::string const & name = args[2];
                std::lock_guard<std::mutex> lck(scene._mtx);
                mesh_object_t *obj = scene.get_mesh(name);
                if (!obj){throw std::runtime_error("Can't find mesh " + name);}
                scene._objects.erase(static_cast<std::vector<mesh_object_t>::iterator>(obj));
            }
            else
            {
                throw std::runtime_error("Invalid object-type " + args[1] + " has to be one of camera, texture, object");
            }
        }
        else if (command == "camera")
        {
            if (args.size() > 1)
            {
                std::string name = args[1];
                scene._cameras.push_back(camera_t(name));
                read_transformations(scene._cameras.back()._transformation, args.begin() + 2, args.end());
                session_update |= UPDATE_SCENE;
            }
            else
            {
                for (camera_t const & cam : scene._cameras)
                {
                    out << cam._name << std::endl;
                }
            }
        }
        else if (command == "texture")
        {
            if (args.size() > 1)
            {
                pending_task.assign(PENDING_SCENE_EDIT);
                texture_t tex;
                tex._name = args[1];
                tex._width = std::stoi(args[2]);
                tex._height = std::stoi(args[3]);
                tex._channels = std::stoi(args[4]);
                tex._type = get_viewtype(args[5]);
                scene._textures.push_back(tex);
            }
            else
            {
                for (texture_t const & t : scene._textures)
                {
                    out << t._name << std::endl;
                }
            }
        }
        else if (command == "sleep")
        {
            assert_argument_count(2, args.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(args[1])));
        }
        else if (command == "wait")
        {
            wait_for_rendered_frame_t wait_obj(session._rendered_frames + 1);
            session.wait_for_frame(wait_obj);
            std::mutex mtx;
            std::unique_lock<std::mutex> lck(mtx);
            wait_obj._cv.wait(lck,std::ref(wait_obj));
        }
        else if (command == "join")
        {
            PendingFlag flag = PENDING_ALL;
            if (args.size() > 1)
            {
                flag = PENDING_NONE;
                for (auto iter = args.begin() + 1; iter != args.end(); ++iter)
                {
                    if      (*iter == "thread") {flag |= PENDING_THREAD;}
                    else if (*iter == "fwrite") {flag |= PENDING_FILE_WRITE;}
                    else if (*iter == "fread")  {flag |= PENDING_FILE_READ;}
                    else if (*iter == "swrite") {flag |= PENDING_SCENE_EDIT;}
                    else if (*iter == "sread")  {flag |= PENDING_TEXTURE_READ;}
                    else if (*iter == "all")    {flag |= PENDING_ALL;}
                }
            }
            env.join(&pending_task, flag);
        }//exec python3 python/export_frames.py 0 1 /dev/zero "\"cam0|cam1\"" "\"rendered|flow|depth|index\""
        else if (command == "framelist")
        {
            if (args.size() > 1)
            {
                std::string name = args[1];
                std::string framefilename = args[2];
                std::ifstream framefile(framefilename);
                std::vector<size_t> framelist = IO_UTIL::parse_framelist(framefile);
                {
                    std::lock_guard<std::mutex> lck(scene._mtx);
                    scene._framelists.emplace_back(name, framelist);
                }
                framefile.close();
                session_update |= UPDATE_SCENE;
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
                pending_task.assign(PENDING_FILE_READ | PENDING_SCENE_EDIT);
                std::string name = args[1];
                if (args.size() < 3)
                {
                    throw std::runtime_error("No file was given for mesh " + args[1]);
                }
                std::string meshfile = args[2];
                clock_t current_time = clock();
                mesh_object_t m = mesh_object_t(name, meshfile);
                std::cout << "mesh loading time: " << float( clock() - current_time ) / CLOCKS_PER_SEC << std::endl;
                pending_task.unset(PENDING_FILE_READ);
                {
                    std::lock_guard<std::mutex> lck(scene._mtx);
                    scene._objects.emplace_back(std::move(m));
                    read_transformations(scene._objects.back()._transformation, args.begin() + 3, args.end());
                }
                session_update |= UPDATE_SCENE;
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
            assert_argument_count(2, args.size());
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
                session_update |= UPDATE_SCENE;
            }
            else
            {
                out << "object not found" << std::endl;
            }
        }
        else if (command == "exit")
        {
            session._exit_program = true;
            session_update |= UPDATE_REDRAW;
        }
        else if (command == "anim")
        {
            pending_task.assign(PENDING_FILE_READ | PENDING_SCENE_EDIT);
            assert_argument_count(2, args.size());
            std::string animfile = args[1];
            std::ifstream animss(animfile);
            std::cout << "animfile: " << animfile << std::endl;
            clock_t current_time = clock();
            std::vector<std::vector<float> > anim_data = IO_UTIL::parse_csv(animss);
            clock_t loading_time = clock();
            animss.close();
            pending_task.unset(PENDING_FILE_READ);
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
                }
                else if (field == "frame")
                {
                    ++strIter;
                }
                else
                {
                    std::unique_lock<std::mutex> lck(scene._mtx);
                    bool found = false;
                    for (size_t i = 0; i < scene.num_objects(); ++i)
                    {
                        object_t & obj = scene.get_object(i);
                        if (obj._name == field)
                        {
                            found = true;
                            std::string type = *strIter;
                            if (type == "pos")
                            {
                                for (size_t fr = 0; fr < anim_data.size(); ++fr)
                                {
                                    size_t frame = fr;
                                    if (index_column != std::numeric_limits<size_t>::max())
                                    {
                                        frame = anim_data[fr][index_column];
                                    }
                                    obj._key_pos[frame] = {anim_data[fr][column], anim_data[fr][column + 1], anim_data[fr][column + 2]};
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
                            else if (type == "aperture")
                            {
                                camera_t *cam = dynamic_cast<camera_t *>(&obj);
                                if (!cam)
                                {
                                    throw std::runtime_error("Object is not a camera");
                                }
                                for (size_t fr = 0; fr < anim_data.size(); ++fr)
                                {
                                    size_t frame = fr;
                                    if (index_column != std::numeric_limits<size_t>::max())
                                    {
                                        frame = anim_data[fr][index_column];
                                    }
                                    cam->_key_aperture[frame] = anim_data[fr][column];
                                }
                                column += 1;
                            }
                        }
                    }
                    if (found)
                    {
                        ++strIter;
                    }
                    else
                    {
                        out << "Warning didn't found key " << field << std::endl;
                    }
                }
            }
            std::cout << "anim loading time: " << float( loading_time - current_time) / CLOCKS_PER_SEC << anim_data.size() << " trajctory creation: " << float(clock() - loading_time) / CLOCKS_PER_SEC << std::endl;
            session_update = UPDATE_SCENE;
        }
        else
        {
            out << "Unknown command: " << input << std::endl;
        }
        if (ref_int32_t)
        {
            if (args.size() > 1)
            {
                int32_t tmp = std::stoi(args[1]);
                if (tmp != *ref_int32_t)
                {
                    session_update |= session_var;
                    *ref_int32_t = tmp;
                }
            }
            else
            {
                out << *ref_int32_t << std::endl;
            }
        }
        if (ref_size_t)
        {
            if (args.size() > 1)
            {
                size_t tmp = std::stoi(args[1]);
                if (tmp != *ref_size_t)
                {
                    session_update |= session_var;
                    *ref_size_t = tmp;
                }
            }
            else
            {
                out << *ref_size_t << std::endl;
            }
        }
        if (ref_float_t)
        {
            if (args.size() > 1)
            {
                float tmp = std::stof(args[1]);
                if (tmp != *ref_float_t)
                {
                    session_update |= session_var;
                    *ref_float_t = tmp;
                }
            }
            else
            {
                out << *ref_float_t << std::endl;
            }
        }
        if (ref_bool)
        {
            if (args.size() > 1)
            {
                bool tmp = std::stof(args[1]);
                if (tmp != *ref_bool)
                {
                    session_update |= session_var;
                    *ref_bool = tmp;
                }
            }
            else
            {
                out << *ref_bool << std::endl;
            }
        }
        if (session_update)
        {
            session.scene_update(session_update);
        }
    }catch (const std::exception &e)
    {
        out << "Caught exception " << e.what() << std::endl;
        pending_task.assign(PENDING_NONE);
        throw;
    }catch (...){
        out << "Caught exception" << std::endl;
        pending_task.assign(PENDING_NONE);
        throw;
    }
    pending_task.assign(PENDING_NONE);
}

template<typename R>bool is_ready(std::future<R> const& f){return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

void exec(std::string input, std::vector<std::string> const & variables, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task)
{
    input = std::string(input.begin(), std::find(input.begin(), input.end(), '#'));
    if (input.size()==0)
    {
        pending_task.assign(PENDING_NONE);
        return;
    }
    IO_UTIL::find_and_replace_all(input, "${sdir}", env._script_dir);
    for (size_t i = 0; i < variables.size(); ++i)
    {
        IO_UTIL::find_and_replace_all(input, "${" + std::to_string(i) + "}", variables[i]);
    }
    if (input[input.size() - 1] == '\n')
    {
        input.pop_back();
    }
    if (input == "if true" || input == "if 1")
    {
        env._code_stack.push_back(CODE_TRUE_IF);
    }
    else if (input == "if false" || input == "if 0")
    {
        env._code_stack.push_back(CODE_FALSE_IF);
    }
    else if (input == "else")
    {
        if (env._code_stack.empty())
        {
            throw std::runtime_error("error, wrong stack state");
        }
        if (env._code_stack.back() == CODE_TRUE_IF)
        {
            env._code_stack.back() = CODE_FALSE_IF;
        }
        else if (env._code_stack.back() == CODE_FALSE_IF)
        {
            env._code_stack.back() = CODE_TRUE_IF;
        }
        else
        {
            throw std::runtime_error("error, wrong stack state");
        }
    }
    else if (input == "endif")
    {
        if (env._code_stack.empty() || (env._code_stack.back() != CODE_TRUE_IF && env._code_stack.back() != CODE_FALSE_IF))
        {
            throw std::runtime_error("error, wrong stack state");
        }
        env._code_stack.pop_back();
    }
    else if (env.code_active())
    {
        if (input[input.size() - 1] == '&')
        {
            try
            {
                input.pop_back();
                std::cout << "start background " << input << std::endl;
                env.clean();
                pending_task._future = std::move(std::async(std::launch::async, exec_impl, input, std::ref(env), std::ref(out), std::ref(session), std::ref(pending_task)));
            }catch (std::system_error const & error){
                if (error.what() == std::string("Resource temporarily unavailable"))
                {
                    std::cout << "Task couldn't be started in background: Resource temporarily unavailable" << std::endl;
                    exec_impl(input, env, out, session, pending_task);
                }
                else
                {
                    throw error;
                }
            }
        }
        else
        {
            exec_impl(input, env, out, session, pending_task);
        }
    }
}
