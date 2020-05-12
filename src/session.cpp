#include "session.h"

#include <future>

void exec_impl(std::string input, exec_env & env, std::ostream & out, session_t & session, pending_task_t &pending_task)
{
    while (input.back() == 10)
    {
        input.pop_back();
    }
    std::cout << std::endl;
    std::vector<std::string> args;
    scene_t & scene = session._scene;
    IO_UTIL::split_in_args(args, input);
    //std::stringstream ss(input);
    std::string command = args[0];
    size_t *ref_size_t = nullptr;
    //uint32_t *ref_uint32_t = nullptr;
    int32_t *ref_int32_t = nullptr;
    float *ref_float_t = nullptr;
    bool *ref_bool = nullptr;
    bool session_var = false;
    SessionUpdateType session_update = UPDATE_NONE;
    if (command == "help")
    {
        out << "frame (<frame>)" << std::endl;
        out << "play (<speed>)" << std::endl;
        out << "loglevel (<ivalue>)" << std::endl;
        out << "next (<frames>)"<< std::endl;
        out << "prev (<frames>)"<< std::endl;
        out << "show_only (<framelist>)" << std::endl;
        out << "diffbackward (<num_frames>)" <<std::endl;
        out << "diffforward (<num_frames>)" << std::endl;
        out << "screenshot <filename>" << std::endl;
        out << "screenshot2 <filename> <width> <height> <camera> <type> (<export nan>)" << std::endl;
        out << "camera <name>" << std::endl;
        out << "diffrot (<activated>)" << std::endl;
        out << "difftrans (<activated>)" << std::endl;
        out << "preresolution (<num_pixels>)" << std::endl;
        out << "approximated (<activated>)" << std::endl;
        out << "echo <...>" << std::endl;
        out << "run <scriptfile>" << std::endl;
        out << "exec <command>" << std::endl;
        out << "wait -> wait for next redraw" << std::endl;
        out << "join (<thread sread swrite fread fwrite all>)-> wait for all tasks in the pipeline to fininsh" << std::endl;
        out << "framelist <filename> <name>" <<std::endl;
        out << "object <name> <filename> (<transformation>)" << std::endl;
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
    
    else if (command == "show_only")
    {
        if (args.size() > 1)
        {
            session._show_only = args[1];
        }
        else
        {
            session._show_only = "";
        }
    }
    else if (command == "viewmode")
    {
        if (args[1] == "equidistant")
        {
            session._viewmode = EQUIDISTANT;
            session_update |= UPDATE_SESSION;
        }
        else if (args[1] == "perspective")
        {
            session._viewmode = PERSPECTIVE;
            session_update |= UPDATE_SESSION;
        }
    }
    else if (command == "frame" || command == "goto")
    {
        ref_int32_t = &session._m_frame;
        session_var = true;
    }
    else if (command == "play")
    {
        ref_int32_t = &session._play;
    }
    else if (command == "approximated")
    {
        ref_bool = &session._approximated;
    }
    else if (command == "animating")
    {
        if (args.size() > 1)
        {
            RedrawScedule animating;
            if (args[1] == "always"){animating = REDRAW_ALWAYS;}
            else if (args[1] == "automatic"){animating = REDRAW_AUTOMATIC;}
            else if (args[1] == "manual"){animating = REDRAW_MANUAL;}
            else{throw std::runtime_error("Option " + args[1] + " not known");}
            if (animating != session._animating)
            {
                session._animating = animating;
                session.scene_update(UPDATE_ANIMATING);
            }
        }
        else
        {
            switch(session._animating)
            {
                case REDRAW_ALWAYS:     out << "always"    << std::endl; break;
                case REDRAW_AUTOMATIC:  out << "automatic" << std::endl; break;
                case REDRAW_MANUAL:     out << "manual"    << std::endl; break;
                default:                throw std::runtime_error("Unknown redraw type");
            }
        }
    }
    else if (command == "redraw")
    {
        session.scene_update(UPDATE_REDRAW);
    }
    else if (command == "loglevel")
    {
        ref_size_t = &session._loglevel;
    }
    else if (command == "debug")
    {
        ref_bool = &session._debug;
    }
    else if (command == "next")
    {
        if (args.size() > 1)
        {
            session._m_frame += std::stoi(args[1]);
        }
        else
        {
            ++session._m_frame;
        }
        session_update |= UPDATE_FRAME;
    }
    else if (command == "prev")
    {
        if (args.size() > 1)
        {
            session._m_frame -= std::stoi(args[1]);
        }
        else
        {
            --session._m_frame;
        }
        session_update |= UPDATE_FRAME;
    }
    else if (command == "diffbackward")
    {
        ref_int32_t = &session._diffbackward;
        session_var = true;
    }
    else if (command == "diffforward")
    {
        ref_int32_t = &session._diffforward;
        session_var = true;
    }
    else if (command == "screenshot")
    {
        session._screenshot = args[1];
    }
    else if (command == "screenshot2")
    {
        //screenshot2 test.png 512 512 left_eye rendered
        //screenshot2 test.tga 512 512 left_eye rendered
        //screenshot2 test.jpg 512 512 left_eye rendered
        bool export_nan = false;
        size_t prerendering = std::numeric_limits<size_t>::max();
        if (args.size() > 6)
        {
            export_nan = std::stoi(args[6]);
            if (args.size() > 7)
            {
                prerendering = std::stoi(args[7]);
            }
        }
        viewtype_t view;
        if      (args[5] == "rendered") {view = VIEWTYPE_RENDERED;}
        else if (args[5] == "position") {view = VIEWTYPE_POSITION;}
        else if (args[5] == "index")    {view = VIEWTYPE_INDEX;}
        else if (args[5] == "depth")    {view = VIEWTYPE_DEPTH;}
        else if (args[5] == "flow")     {view = VIEWTYPE_FLOW;}
        else                            {throw std::runtime_error("type not known");}
        std::string const & output = args[1];
        //if (output.ends_with(".png") || output.ends_with(".jpg") || output.ends_with(".exr")
        //{
        pending_task.assign(PENDING_FILE_WRITE | PENDING_TEXTURE_READ | PENDING_SCENE_EDIT);
        screenshot_handle_t handle;
        queue_lazy_screenshot_handle(output, std::stoi(args[2]), std::stoi(args[3]), args[4], view, export_nan, prerendering, scene, handle);
        pending_task.unset(PENDING_SCENE_EDIT);
        int ret = wait_until_ready(handle);
        pending_task.unset(PENDING_TEXTURE_READ);
        if (ret != 0)
        {
            out << "error at getting texture" << std::endl;
        }
        else
        {
            save_lazy_screenshot(output, handle);
        }
        pending_task.unset(PENDING_FILE_WRITE);
        //}
        out << "success" << std::endl;
    }
    else if (command == "diffrot")
    {
        ref_bool = &session._diffrot;
        session_var = true;
    }
    else if (command == "difftrans")
    {
        ref_bool = &session._difftrans;
        session_var = true;
    }
    else if (command == "smoothing")
    {
        ref_size_t = &session._smoothing;
        session_var = true;
    }
    else if (command == "fov")
    {
        ref_float_t = &session._fov;
        session_var = true;
    }
    else if (command == "reload")
    {
        if (args[1] == "shader")
        {
            session._reload_shader = true;
            session_update |= UPDATE_REDRAW;
        }
    }
    else if (command == "show")
    {
        bool *ref = nullptr;
        if (args[1] == "raytraced")     {ref = &session._show_raytraced;}
        else if (args[1] == "flow")     {ref = &session._show_flow;}
        else if (args[1] == "index")    {ref = &session._show_index;}
        else if (args[1] == "pos")      {ref = &session._show_position;}
        else if (args[1] == "depth")    {ref = &session._show_depth;}
        else if (args[1] == "curser")   {ref = &session._show_curser;}
        else if (args[1] == "arrows")   {ref = &session._show_arrows;}
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
    else if (command == "preresolution")
    {
        ref_size_t = &session._preresolution;
        session_var = true;
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
                else
                {
                    out << "error, key not known, valid keys are transform, visible" << std::endl;
                }
            }
        }
        session_update |= UPDATE_SCENE;
    }
    else if (command == "run")
    {
        std::ifstream infile(args[1]);
        std::string line;
        std::cout << "run " <<args[1] << std::endl;
        exec_env subenv(args[1]);
        if (!infile)
        {
            std::cout << "error bad file: " << args[1] << std::endl;
        }
        std::vector<std::string> vars(args.begin() + 1, args.end());
        while(std::getline(infile, line))
        {
            std::cout << line << std::endl;
            exec(line, vars, subenv, out, session, subenv.emitPendingTask());
        }
        subenv.join(&pending_task, PENDING_ALL);
        infile.close();
    }
    else if (command == "exec")
    {
        std::array<char, 1024> buffer;
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
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            std::string line = buffer.data();
            if (line.back() == '\n')
            {
                line.pop_back();
            }
            std::cout << "out " << line << std::endl;
            exec(line, std::vector<std::string>(args.begin() + 1, args.end()), subenv, out, session, subenv.emitPendingTask());
        }
    }
    else if (command == "camera")
    {
        if (args.size() > 1)
        {
            std::string name = args[1];
            session._scene._cameras.push_back(camera_t(name));
            read_transformations(session._scene._cameras.back()._transformation, args.begin() + 2, args.end());
            session_update |= UPDATE_SCENE;
        }
        else
        {
            for (camera_t const & cam : session._scene._cameras)
            {
                out << cam._name << std::endl;
            }
        }
    }
    else if (command == "sleep")
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(args[1])));
    }
    else if (command == "wait")
    {
        std::mutex mtx;
        std::unique_lock<std::mutex> lck(mtx);
        scene._mtx.lock();
        //Wait until fhe next frame
        wait_for_rendered_frame wait_obj(session._rendered_frames + 1);
        session._wait_for_rendered_frame_handles.push_back(&wait_obj);
        scene._mtx.unlock();
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
                if (*iter == "thread")      {flag |= PENDING_THREAD;}
                else if (*iter == "fwrite") {flag |= PENDING_FILE_WRITE;}
                else if (*iter == "fread")  {flag |= PENDING_FILE_READ;}
                else if (*iter == "swrite") {flag |= PENDING_SCENE_EDIT;}
                else if (*iter == "sread")  {flag |= PENDING_TEXTURE_READ;}
                else if (*iter == "all")    {flag |= PENDING_ALL;}
            }
        }
        std::cout << "joining " << &pending_task << std::endl;
        env.join(&pending_task, flag);
        std::cout << "joined " << &pending_task << std::endl;
    }
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
            std::string meshfile = args[2];
            mesh_object_t m = mesh_object_t(name, meshfile);
            pending_task.unset(PENDING_FILE_READ);
            {
                std::lock_guard<std::mutex> lck(scene._mtx);
                scene._objects.emplace_back(std::move(m));//TODO
            }
            read_transformations(scene._objects.back()._transformation, args.begin() + 3, args.end());
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
    }
    else if (command == "anim")
    {
        pending_task.assign(PENDING_FILE_READ | PENDING_SCENE_EDIT);
        std::string animfile = args[1];
        std::ifstream animss(animfile);
        std::cout << "animfile: " << animfile << std::endl;
        std::vector<std::vector<float> > anim_data = IO_UTIL::parse_csv(animss);
        animss.close();
        pending_task.unset(PENDING_FILE_READ);
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
                std::unique_lock<std::mutex> lck(scene._mtx);
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
                else
                {
                    out << "Warning didn't found key " << field << std::endl;
                }
            }
        }
        session_update = UPDATE_SCENE;
    }
    else
    {
        out << "Unknown command: " << input << std::endl;
    }
    if (ref_int32_t != nullptr)
    {
        if (args.size() > 1)
        {
            int32_t tmp = std::stoi(args[1]);
            if (tmp != *ref_int32_t && session_var)
            {
                session_update |= UPDATE_SESSION;
            }
            *ref_int32_t = tmp;
        }
        else
        {
            out << *ref_int32_t << std::endl;
        }
    }
    if (ref_size_t != nullptr)
    {
        if (args.size() > 1)
        {
            size_t tmp = std::stoi(args[1]);
            if (tmp != *ref_size_t && session_var)
            {
                session_update |= UPDATE_SESSION;
            }
            *ref_size_t = tmp;
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
            if (tmp != *ref_float_t && session_var)
            {
                session_update |= UPDATE_SESSION;
            }
            *ref_float_t = tmp;
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
            if (tmp != *ref_bool && session_var)
            {
                session_update |= UPDATE_SESSION;
            }
            *ref_bool = tmp;
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
    pending_task.assign(PENDING_NONE);
}

template<typename R>bool is_ready(std::future<R> const& f){return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

void exec(std::string input, std::vector<std::string> const & variables, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task)
{
    input = std::string(input.begin(), std::find(input.begin(), input.end(), '#'));
    {
        pending_task.assign(PENDING_NONE);
        return;
    }
    IO_UTIL::find_and_replace_all(input, "{sdir}", env._script_dir);
    for (size_t i = 0; i < variables.size(); ++i)
    {
        IO_UTIL::find_and_replace_all(input, "{$" + std::to_string(i) + "}", variables[i]);
    }
    if (input[input.size() - 1] == '\n')
    {
        input.pop_back();
    }
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
