#include <boost/python.hpp>
#include <iostream>
#include "session.h"

void api_call()
{
     std::cout << "API call" << std::endl;
}

namespace bp = boost::python;

/*
 * Still missing
    bool            _diffflipy = true;
    int             _perm = 0;
    std::string     _show_only;
    bool            _reload_shader = false;
    RedrawScedule   _animating = REDRAW_ALWAYS;
    bool            _realtime = false;
    size_t          _frames_per_step = 1;
    size_t          _frames_per_second = 60;
*/
    
BOOST_PYTHON_MODULE(Multiview)
{
    bp::def("api_call", api_call);
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
        .add_property("show_framelists",&session_t::_show_framelists,&session_t::set<bool,  &session_t::_show_framelists,UPDATE_SESSION>)
        .add_property("show_visibility",&session_t::_show_rendered_visibility,&session_t::set<bool,  &session_t::_show_rendered_visibility,UPDATE_SESSION>)
        .add_property("depth_testing",  &session_t::_depth_testing,  &session_t::set<bool,  &session_t::_depth_testing,  UPDATE_SESSION>)
        .add_property("depth_scale",    &session_t::_depth_scale,    &session_t::set<float, &session_t::_depth_scale,    UPDATE_SESSION>)
        .add_property("frame",          &session_t::_m_frame,        &session_t::set<int,   &session_t::_m_frame,        UPDATE_SESSION>)
        .add_property("approximated",   &session_t::_approximated,   &session_t::set<bool,  &session_t::_approximated,   UPDATE_SESSION>)
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
        .def("update_session", &session_t::scene_update);
}

namespace PYTHON{
#if PY_MAJOR_VERSION >= 3
#   define INIT_MODULE PyInit_Multiview
    extern "C" PyObject* INIT_MODULE();
#else
#   define INIT_MODULE initMultiview
    extern "C" void INIT_MODULE();
#endif
    
void run(std::string const & file, session_t *session){
        try{
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
        global["session"] = s;
        
        bp::object result = bp::exec_file(file.c_str(), global, global);
        PyGILState_Release(state);
        Py_END_ALLOW_THREADS
        //Py_Finalize();
    }catch (...){PyErr_Print();bp::handle_exception();}
    }
}
