#include <boost/python.hpp>
#include <iostream>
#include "session.h"

static session_t *static_session;

void api_call()
{
     std::cout << "API call" << std::endl;
}

namespace bp = boost::python;

BOOST_PYTHON_MODULE(Multiview)
{
      bp::def("api_call", api_call);
    
      bp::class_<session_t, boost::noncopyable>("Session")
        .add_property("diffbackward",   &session_t::_diffbackward,  &session_t::set<int,   &session_t::_diffbackward,   UPDATE_SESSION>)
        .add_property("diffforward",    &session_t::_diffforward,   &session_t::set<int,   &session_t::_diffforward,    UPDATE_SESSION>)
        .add_property("show_raytraced", &session_t::_show_raytraced,&session_t::set<bool,  &session_t::_show_raytraced, UPDATE_SESSION>)
        .add_property("show_flow",      &session_t::_show_flow,     &session_t::set<bool,  &session_t::_show_flow,      UPDATE_SESSION>)
        .add_property("show_arrows",    &session_t::_show_arrows,   &session_t::set<bool,  &session_t::_show_arrows,    UPDATE_SESSION>)
        .add_property("show_index",     &session_t::_show_index,    &session_t::set<bool,  &session_t::_show_index,     UPDATE_SESSION>)
        .add_property("show_position",  &session_t::_show_position, &session_t::set<bool,  &session_t::_show_position,  UPDATE_SESSION>)
        .add_property("show_depth",     &session_t::_show_depth,    &session_t::set<bool,  &session_t::_show_depth,     UPDATE_SESSION>)
        .add_property("frame",          &session_t::_m_frame,       &session_t::set<int,   &session_t::_m_frame,        UPDATE_SESSION>)
        .add_property("approximated",   &session_t::_approximated,  &session_t::set<bool,  &session_t::_approximated,   UPDATE_SESSION>)
        .add_property("fov",            &session_t::_fov,           &session_t::set<float, &session_t::_fov,            UPDATE_SESSION>)
        .add_property("preresolution",  &session_t::_preresolution, &session_t::set<size_t,&session_t::_preresolution,  UPDATE_SESSION>)
        .add_property("diffnormalize",  &session_t::_diffnormalize, &session_t::set<bool,  &session_t::_diffnormalize,  UPDATE_SESSION>);
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
        static_session = session;
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
