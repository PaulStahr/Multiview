#include "session.h"

#include <future>

void exec_impl(std::string input, exec_env & env, std::ostream & out, session_t & session)
{
    std::vector<std::string> args;
    scene_t & scene = session._scene;
    IO_UTIL::split_in_args(args, input);
    //std::stringstream ss(input);
    std::string command = args[0];
    size_t *ref_size_t = nullptr;
    uint32_t *ref_uint32_t = nullptr;
    int32_t *ref_int32_t = nullptr;
    float *ref_float_t = nullptr;
    bool *ref_bool = nullptr;
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
            session._viewmode = EQUIDISTANT;
        }
        else if (args[1] == "perspective")
        {
            session._viewmode = PERSPECTIVE;
        }
    }
    else if (command == "frame")
    {
        ref_int32_t = &session._m_frame;
    }
    else if (command == "goto")
    {
        ref_int32_t = &session._m_frame;
    }
    else if (command == "play")
    {
        ref_int32_t = &session._play;
    }
    else if (command == "loglevel")
    {
        ref_size_t = &session._loglevel;
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
    else if (command == "diffbackward")
    {
        ref_int32_t = &session._diffbackward;
    }
    else if (command == "diffforward")
    {
        ref_int32_t = &session._diffforward;
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
        std::string const & output = args[1];
        //if (output.ends_with(".png") || output.ends_with(".jpg") || output.ends_with(".exr")
        //{
        take_lazy_screenshot(output, std::stoi(args[2]), std::stoi(args[3]), args[4], view, export_nan, scene);
        //}
        std::cout << "success" << std::endl;
    }
    else if (command == "diffrot")
    {
        ref_bool = &session._diffrot;
    }
    else if (command == "difftrans")
    {
        ref_bool = &session._difftrans;
    }
    else if (command == "smoothing")
    {
        ref_size_t = &session._smoothing;
    }
    else if (command == "fov")
    {
        ref_float_t = &session._fov;
    }
    else if (command == "reload")
    {
        if (args[1] == "shader")
        {
            session._reload_shader = true;
        }
    }
    else if (command == "show")
    {
        bool *ref = nullptr;
        if (args[1] == "raytraced")
        {
            ref = &session._show_raytraced;
        }
        else if (args[1] == "flow")
        {
            ref = &session._show_flow;
        }
        else if (args[1] == "index")
        {
            ref = &session._show_index;
        }
        else if (args[1] == "pos")
        {
            ref = &session._show_position;
        }
        else if (args[1] == "depth")
        {
            ref = &session._show_depth;
        }
        else if (args[1] == "curser")
        {
            ref = &session._show_curser;
        }
        else if (args[1] == "arrows")
        {
            ref = &session._show_arrows;
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
        ref_size_t = &session._preresolution;
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
            exec(line, env, out, session);
        }
        infile.close();
    }
    else if (command == "camera")
    {
        if (args.size() > 1)
        {
            std::string name = args[1];
            session._scene._cameras.push_back(camera_t(name));
            read_transformations(session._scene._cameras.back()._transformation, args.begin() + 2, args.end());
        }
        else
        {
            for (camera_t const & cam : session._scene._cameras)
            {
                out << cam._name << std::endl;
            }
        }
    }
    else if (command == "wait")
    {
        std::mutex mtx;
        std::unique_lock<std::mutex> lck(mtx);
        session._scene._mtx.lock();
        wait_for_rendered_frame wait_obj(session._rendered_frames + 1);
        session._wait_for_rendered_frame_handles.push_back(&wait_obj);
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
        session._exit_program = true;
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
    else
    {
        std::cout << "Unknown command" << std::endl;
    }
    if (ref_int32_t != nullptr)
    {
        if (args.size() > 1)
        {
            *ref_int32_t = std::stoi(args[1]);
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
            *ref_size_t = std::stoi(args[1]);
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
            *ref_float_t = std::stof(args[1]);
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
            *ref_bool = static_cast<bool>(std::stoi(args[1]));
        }
        else
        {
            out << *ref_bool << std::endl;
        }
    }
}

void exec(std::string const & input, exec_env & env, std::ostream & out, session_t & session)
{
    if (input[input.size() - 1] == '&')
    {
        std::string substr = input.substr(0, input.size() - 1);
        std::cout << "start background " << substr << std::endl;
        auto f = std::async(std::launch::async, exec_impl, substr, std::ref(env), std::ref(out), std::ref(session));

        env._mtx.lock();
        env._pending_futures.push_back(std::move(f));
        env._mtx.unlock();

        //new std::thread(std::ref(exec_impl), substr, std::ref(out));
    }
    else
    {
        exec_impl(input, env, out, session);
    }
}
