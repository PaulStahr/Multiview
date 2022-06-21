#include <boost/python.hpp>
#include <iostream>
#include "session.h"
#include "python_binding.h"

namespace bp = boost::python;

/*
 * Still missing
    bool            _diffflipy = true;
    int             _perm = 0;
    std::string     _show_only;
    bool            _reload_shader = false;
    bool            _realtime = false;
    size_t          _frames_per_step = 1;
    size_t          _frames_per_second = 60;
*/


template<typename T>
inline
std::vector< T > py_list_to_std_vector( const boost::python::object& iterable )
{
    return std::vector< T >( boost::python::stl_input_iterator< T >( iterable ),
                             boost::python::stl_input_iterator< T >( ) );
}

BOOST_PYTHON_MODULE(Multiview)
{
    bp::enum_<SessionUpdateType>("SessionUpdateType")
        .value("update_none",       UPDATE_NONE)
        .value("update_animating",  UPDATE_ANIMATING)
        .value("update_redraw",     UPDATE_REDRAW)
        .value("update_session",    UPDATE_SESSION)
        .value("update_scene",      UPDATE_SCENE)
        .value("update_frame",      UPDATE_FRAME);

    bp::enum_<RedrawScedule>("RedrawScedule")
        .value("redraw_always",     REDRAW_ALWAYS)
        .value("redraw_automatic",  REDRAW_AUTOMATIC)
        .value("redraw_manual",     REDRAW_MANUAL);

    bp::enum_<coordinate_system_t>("CoordinateSystem")
        .value("spherical_approximated", COORDINATE_SPHERICAL_APPROXIMATED)
        .value("spherical_multipass",    COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS)
        .value("spherical_singlepass",   COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS);

    bp::enum_<screenshot_task>("ScreenshotTask")
        .value("take_screenshot",       TAKE_SCREENSHOT)
        .value("save_texture",          SAVE_TEXTURE)
        .value("render_to_texture",     RENDER_TO_TEXTURE);

    bp::enum_<screenshot_state>("ScreenshotState")
        .value("screenshot_state_inited",           screenshot_state_inited)
        .value("screenshot_state_queued",           screenshot_state_queued)
        .value("screenshot_state_rendered_texture", screenshot_state_rendered_texture)
        .value("screenshot_state_rendered_buffer",  screenshot_state_rendered_buffer)
        .value("screenshot_state_copied",           screenshot_state_copied)
        .value("screenshot_state_saved",            screenshot_state_saved)
        .value("screenshot_state_error",            screenshot_state_error);

    bp::class_<screenshot_handle_t, boost::noncopyable>("ScreenshotHandle")
        .add_property("texture",        &screenshot_handle_t::_texture)
        .add_property("camera",         &screenshot_handle_t::_camera)
        .add_property("prerendering",   &screenshot_handle_t::_prerendering)
        .add_property("viewtype_t",     &screenshot_handle_t::_type)
        .add_property("flip",           &screenshot_handle_t::_flip)
        .add_property("width",          &screenshot_handle_t::_width)
        .add_property("height",         &screenshot_handle_t::_height)
        .def("wait_until",              &screenshot_handle_t::wait_until)
        .def("get_data_f",              &screenshot_handle_t::copy_data<float>)
        .def("get_data_b",              &screenshot_handle_t::copy_data<uint8_t>)
        .def("get_data_s",              &screenshot_handle_t::copy_data<uint16_t>);

    bp::class_<session_t, boost::noncopyable>("Session")
        .add_property("diffbackward",   &session_t::_diffbackward,   &session_t::set<int,   &session_t::_diffbackward,   UPDATE_SESSION>)
        .add_property("diffforward",    &session_t::_diffforward,    &session_t::set<int,   &session_t::_diffforward,    UPDATE_SESSION>)
        .add_property("diffrot",        &session_t::_diffrot,        &session_t::set<bool,  &session_t::_diffrot,        UPDATE_SESSION>)
        .add_property("difftrans",      &session_t::_difftrans,      &session_t::set<bool,  &session_t::_difftrans,      UPDATE_SESSION>)
        .add_property("diffobjects",    &session_t::_diffobjects,    &session_t::set<bool,  &session_t::_diffobjects,    UPDATE_SESSION>)
        .add_property("diffnormalize",  &session_t::_diffnormalize,  &session_t::set<bool,  &session_t::_diffnormalize,  UPDATE_SESSION>)
        .add_property("difffallback",   &session_t::_difffallback,   &session_t::set<bool,  &session_t::_difffallback,   UPDATE_SESSION>)
        .add_property("show_raytraced", &session_t::_show_raytraced, &session_t::set<bool,  &session_t::_show_raytraced, UPDATE_SESSION>)
        .add_property("show_flow",      &session_t::_show_flow,      &session_t::set<bool,  &session_t::_show_flow,      UPDATE_SESSION>)
        .add_property("show_arrows",    &session_t::_show_arrows,    &session_t::set<bool,  &session_t::_show_arrows,    UPDATE_SESSION>)
        .add_property("show_index",     &session_t::_show_index,     &session_t::set<bool,  &session_t::_show_index,     UPDATE_SESSION>)
        .add_property("show_position",  &session_t::_show_position,  &session_t::set<bool,  &session_t::_show_position,  UPDATE_SESSION>)
        .add_property("show_depth",     &session_t::_show_depth,     &session_t::set<bool,  &session_t::_show_depth,     UPDATE_SESSION>)
        .add_property("show_curser",    &session_t::_show_curser,    &session_t::set<bool,  &session_t::_show_curser,    UPDATE_SESSION>)
        .add_property("show_visibility",&session_t::_show_visibility,&session_t::set<bool,  &session_t::_show_visibility,UPDATE_SESSION>)
        .add_property("show_framelists",&session_t::_show_framelists,&session_t::set<bool,  &session_t::_show_framelists,UPDATE_SESSION>)
        .add_property("depth_testing",  &session_t::_depth_testing,  &session_t::set<bool,  &session_t::_depth_testing,  UPDATE_SESSION>)
        .add_property("depth_scale",    &session_t::_depth_scale,    &session_t::set<float, &session_t::_depth_scale,    UPDATE_SESSION>)
        .add_property("frame",          &session_t::_m_frame,        &session_t::set<frameindex_t,   &session_t::_m_frame,         UPDATE_SESSION>)
        .add_property("framedenominator",&session_t::_m_frame,       &session_t::set<frameindex_t,   &session_t::_framedenominator,UPDATE_SESSION>)
        .add_property("motionblur",&session_t::_m_frame,             &session_t::set<frameindex_t,   &session_t::_motion_blur,     UPDATE_SESSION>)
        .add_property("frame",          &session_t::_m_frame,        &session_t::set<frameindex_t,   &session_t::_m_frame,         UPDATE_SESSION>)
        .add_property("fov",            &session_t::_fov,            &session_t::set<float, &session_t::_fov,            UPDATE_SESSION>)
        .add_property("preresolution",  &session_t::_preresolution,  &session_t::set<size_t,&session_t::_preresolution,  UPDATE_SESSION>)
        .add_property("loglevel",       &session_t::_loglevel,       &session_t::set<size_t,&session_t::_loglevel,       UPDATE_NONE>)
        .add_property("smoothing",      &session_t::_smoothing,      &session_t::set<size_t,&session_t::_smoothing,      UPDATE_SESSION>)
        .add_property("crop",           &session_t::_crop,           &session_t::set<bool,  &session_t::_crop,           UPDATE_SESSION>)
        .add_property("auto_update_gui",&session_t::_auto_update_gui,&session_t::set<bool,  &session_t::_auto_update_gui,UPDATE_NONE>)
        .add_property("debug",          &session_t::_debug,          &session_t::set<bool,  &session_t::_debug,          UPDATE_NONE>)
        .add_property("culling",        &session_t::_culling,        &session_t::set<size_t,&session_t::_culling,        UPDATE_SESSION>)
        .add_property("play",           &session_t::_play,           &session_t::set<int,   &session_t::_play,           UPDATE_SESSION>)
        .add_property("animating",      &session_t::_animating,      &session_t::set<RedrawScedule,&session_t::_animating,UPDATE_NONE>)
        .add_property("show_visibility",&session_t::_show_rendered_visibility,&session_t::set<bool,  &session_t::_show_rendered_visibility,UPDATE_SESSION>)
        .add_property("coordinate_system",&session_t::_coordinate_system,   &session_t::set<coordinate_system_t,  &session_t::_coordinate_system,   UPDATE_SESSION>)
        .def("update_session", &session_t::scene_update);

    bp::class_<pending_task_t, boost::noncopyable>("PendingTask", bp::no_init);

    bp::class_<std::vector<std::string> >("SArray");

    bp::class_<exec_env, boost::noncopyable>("ExecEnv", bp::no_init)
        .add_property("script_dir", &exec_env::_script_dir)
        .def("join", &exec_env::join)
        .def("emit", &exec_env::emitPendingTask,bp::return_value_policy<bp::reference_existing_object>());

    bp::enum_<PendingFlag>("PendingFlag")
        .value("pending_thread",        PENDING_THREAD)
        .value("pending_scene_edit",    PENDING_SCENE_EDIT)
        .value("pending_file_write",    PENDING_FILE_WRITE)
        .value("pending_texture_read",  PENDING_TEXTURE_READ)
        .value("pending_file_read",     PENDING_FILE_READ)
        .value("pending_all",           PENDING_ALL)
        .value("pending_none",          PENDING_NONE);

    bp::class_<object_t, boost::noncopyable>("Object", bp::no_init)
        .add_property("name",           &object_t::_name)
        .add_property("id",             &object_t::_id)
        .add_property("visible",        &object_t::_visible)
        .add_property("diffrot",        &object_t::_diffrot)
        .add_property("difftrans",      &object_t::_difftrans)
        .add_property("trajectory",     &object_t::_trajectory);

    bp::class_<camera_t,        boost::noncopyable,bp::bases<object_t> >("Camera", bp::no_init);
    bp::class_<mesh_object_t,   boost::noncopyable,bp::bases<object_t> >("Mesh", bp::no_init);
    
    bp::class_<scene_t, boost::noncopyable>("Scene")
        .def("get_camera",  &scene_t::get_camera,bp::return_value_policy<bp::reference_existing_object>())
        .def("get_mesh",    &scene_t::get_mesh,bp::return_value_policy<bp::reference_existing_object>());
//        .def("queue_screenhot", &scene_t::queue_handle);

    bp::def("exec",exec_stdout);
    bp::def("sarray",py_list_to_std_vector<std::string>);
}

namespace PYTHON{
#if PY_MAJOR_VERSION >= 3
#   define INIT_MODULE PyInit_Multiview
    extern "C" PyObject* INIT_MODULE();
#else
#   define INIT_MODULE initMultiview
    extern "C" void INIT_MODULE();
#endif

void run(std::string const & file, exec_env & env, session_t *session, std::vector<std::string> const & argv){
    try{
        wchar_t** argvc = new wchar_t*[argv.size()];
        PyImport_AppendInittab("Multiview", &PyInit_Multiview);
        Py_Initialize();
        PyEval_InitThreads();
        assert(PyEval_ThreadsInitialized());
        Py_BEGIN_ALLOW_THREADS
        PyGILState_STATE state = PyGILState_Ensure();
        bp::object main = bp::import("__main__");
        bp::dict global = bp::extract<bp::dict>(main.attr("__dict__"));
        bp::object a = bp::import("Multiview");
        bp::object s(boost::ref(session));
        exec_env *tmp = &env;
        bp::object e(boost::ref(tmp));
        global["session"] = s;
        global["env"] = e;
        
        for (size_t i = 0; i < argv.size(); ++i)
        {
            argvc[i] = new wchar_t[argv[i].size() + 1];
            mbstowcs( argvc[i], argv[i].data(), argv[i].size() + 1);
        }
        PySys_SetArgvEx(argv.size(), argvc, false);
        bp::object result = bp::exec_file(file.c_str(), global, global);
        PyGILState_Release(state);
        Py_END_ALLOW_THREADS
        for (size_t i = 0; i < argv.size(); ++i)
        {
            delete[] argvc[i];
        }
        delete[] argvc;
        //Py_Finalize();
    }catch (...){PyErr_Print();bp::handle_exception();}
    }
}
