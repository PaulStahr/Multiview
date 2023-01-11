#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/operators.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/numpy.hpp>
#include <QtGui/QMatrix4x4>
#include <iostream>
#include "session.h"
#include "python_binding.h"
#include "data.h"
#include "io_util.h"

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


void screenshot_py(
    exec_env & env,
    session_t & session,
    std::string const & output,
    std::string const & camera,
    viewtype_t viewtype,
    int width,
    int height,
    std::vector<std::string> const & vcam,
    bool ignore_nan,
    bool background)
{
    screenshot(
        env.emitPendingTask("screenshot"),
        session._scene,
        output,
        viewtype,
        camera,
        width,
        height,
        vcam,
        ignore_nan,
        background);
}

boost::shared_ptr<QMatrix4x4> initMat(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34){
    return boost::shared_ptr<QMatrix4x4>(new QMatrix4x4(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, 0, 0, 0, 1));
}

enum GL_TYPE_ENUM
{
    GL_ENUM_UNSIGNED_BYTE   = GL_UNSIGNED_BYTE,
    GL_ENUM_UNSIGNED_SHORT  = GL_UNSIGNED_SHORT,
    GL_ENUM_UNSIGNED_INT    = GL_UNSIGNED_INT,
    GL_ENUM_BYTE            = GL_BYTE,
    GL_ENUM_SHORT           = GL_SHORT,
    GL_ENUM_INT             = GL_INT,
    GL_ENUM_FLOAT           = GL_FLOAT,
    GL_ENUM_DOUBLE          = GL_DOUBLE
};

bp::numpy::ndarray get_screenshot_data(screenshot_handle_t & handle) {
    static bool numpy_inited = false;
    if (!numpy_inited)
    {
        bp::numpy::initialize();
        numpy_inited = true;
    }
    if (handle.get_state() != screenshot_state_copied)
    {
        throw std::runtime_error("Screenshot handle in wrong state");
    }
    //Py_intptr_t shape[3] = {
    bp::tuple shape = bp::make_tuple(static_cast<long int>(handle._width),static_cast<long int>(handle._height),static_cast<long int>(handle._channels));
    switch (handle.get_datatype())
    {
        case GL_UNSIGNED_BYTE:
        {
            bp::numpy::ndarray result = bp::numpy::empty(shape, bp::numpy::dtype::get_builtin<uint8_t>());
            uint8_t* data = handle.get_data<uint8_t>();
            std::copy(data, data + handle.num_elements(), reinterpret_cast<uint8_t*>(result.get_data()));
            return result;
        }
        case GL_FLOAT:
        {
            bp::numpy::ndarray result = bp::numpy::empty(shape, bp::numpy::dtype::get_builtin<float>());
            float* data = handle.get_data<float>();
            std::copy(data, data + handle.num_elements(), reinterpret_cast<float*>(result.get_data()));
            return result;
        }
    }
    throw std::runtime_error("Type not supported");
}

BOOST_PYTHON_MODULE(Multiview)
{
    bp::enum_<SessionUpdateType>("SessionUpdateType")
        .value("none",       UPDATE_NONE)
        .value("animating",  UPDATE_ANIMATING)
        .value("redraw",     UPDATE_REDRAW)
        .value("session",    UPDATE_SESSION)
        .value("scene",      UPDATE_SCENE)
        .value("frame",      UPDATE_FRAME);

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

    bp::enum_<GL_TYPE_ENUM>("GlType")
        .value("unsigned_byte",     GL_ENUM_UNSIGNED_BYTE)
        .value("unsigned_short",    GL_ENUM_UNSIGNED_SHORT)
        .value("unsigned_int",      GL_ENUM_UNSIGNED_INT)
        .value("byte",              GL_ENUM_BYTE)
        .value("short",             GL_ENUM_SHORT)
        .value("int",               GL_ENUM_INT)
        .value("float",             GL_ENUM_FLOAT)
        .value("double",            GL_ENUM_DOUBLE);

    bp::enum_<viewtype_t>("Viewtype")
        .value("rendered",  VIEWTYPE_RENDERED)
        .value("position",  VIEWTYPE_POSITION)
        .value("depth",     VIEWTYPE_DEPTH)
        .value("flow",      VIEWTYPE_FLOW)
        .value("index",     VIEWTYPE_INDEX)
        .value("visibulity",VIEWTYPE_VISIBILITY)
        .value("end",       VIEWTYPE_END);

    bp::class_<screenshot_handle_t, boost::noncopyable>("ScreenshotHandle")
        .add_property("texture",        &screenshot_handle_t::_texture)
        .def_readwrite("camera",         &screenshot_handle_t::_camera)
        .def_readwrite("prerendering",   &screenshot_handle_t::_prerendering)
        .def_readwrite("viewtype",       &screenshot_handle_t::_type)
        .def_readwrite("channels",       &screenshot_handle_t::_channels)
        .def_readwrite("flip",           &screenshot_handle_t::_flip)
        .def_readwrite("width",          &screenshot_handle_t::_width)
        .def_readwrite("height",         &screenshot_handle_t::_height)
        .def_readwrite("ignore_nan",     &screenshot_handle_t::_ignore_nan)
        .def_readwrite("vcam",           &screenshot_handle_t::_vcam)
        .def_readwrite("task",          &screenshot_handle_t::_task)
        .add_property("state",          &screenshot_handle_t::get_state, &screenshot_handle_t::set_state)
        .def("set_datatype",            &screenshot_handle_t::set_datatype)
        .def("wait_until",              &screenshot_handle_t::wait_until)
        .def("get_data",                &get_screenshot_data);

    bp::enum_<program_error::action>("ProgramErrorAction")
        .value("Ignore",  program_error::action::ignore)
        .value("Skip",    program_error::action::skip)
        .value("Panic",   program_error::action::panic);

    bp::enum_<program_error::error_type>("ProgramErrorType")
        .value("File",      program_error::error_type::file)
        .value("Key",       program_error::error_type::key)
        .value("Animation", program_error::error_type::animation)
        .value("Object",    program_error::error_type::object)
        .value("Syntax",    program_error::error_type::syntax);

    bp::class_<program_error::error_rule>("ErrorRule",bp::init<program_error::error_type, program_error::action>());

    typedef void (std::vector<program_error::error_rule>::*ProgramErrorPushBackReference)(const program_error::error_rule &);
    
    bp::class_<std::vector<program_error::error_rule> >("ErrorStack")
        .def(bp::vector_indexing_suite<std::vector<program_error::error_rule> >())
        .def("popBack", &std::vector<program_error::error_rule>::pop_back)
        .def("pushBack",(ProgramErrorPushBackReference)&std::vector<program_error::error_rule>::push_back);

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
        .add_property("motionblur",     &session_t::_m_frame,        &session_t::set<frameindex_t,   &session_t::_motion_blur,     UPDATE_SESSION>)
        .add_property("frame",          &session_t::_m_frame,        &session_t::set<frameindex_t,   &session_t::_m_frame,         UPDATE_SESSION>)
        .add_property("framedenominator",&session_t::_m_frame,       &session_t::set<frameindex_t,   &session_t::_framedenominator,UPDATE_SESSION>)
        .add_property("fov",            &session_t::_fov,            &session_t::set<float, &session_t::_fov,            UPDATE_SESSION>)
        .add_property("preresolution",  &session_t::_preresolution,  &session_t::set<size_t,&session_t::_preresolution,  UPDATE_SESSION>)
        .add_property("loglevel",       &session_t::_loglevel,       &session_t::set<size_t,&session_t::_loglevel,       UPDATE_NONE>)
        .add_property("smoothing",      &session_t::_smoothing,      &session_t::set<size_t,&session_t::_smoothing,      UPDATE_SESSION>)
        .add_property("crop",           &session_t::_crop,           &session_t::set<bool,  &session_t::_crop,           UPDATE_SESSION>)
        .add_property("auto_update_gui",&session_t::_auto_update_gui,&session_t::set<bool,  &session_t::_auto_update_gui,UPDATE_NONE>)
        .add_property("debug",          &session_t::_debug,          &session_t::set<bool,  &session_t::_debug,          UPDATE_NONE>)
        .add_property("culling",        &session_t::_culling,        &session_t::set<size_t,&session_t::_culling,        UPDATE_SESSION>)
        .add_property("play",           &session_t::_play,           &session_t::set<int,   &session_t::_play,           UPDATE_SESSION>)
        .add_property("indirect",       &session_t::_indirect_rendering,&session_t::set<bool, &session_t::_indirect_rendering, UPDATE_SESSION>)
        .add_property("animating",      &session_t::_animating,      &session_t::set<RedrawScedule,&session_t::_animating,UPDATE_NONE>)
        .add_property("show_visibility",&session_t::_show_rendered_visibility,&session_t::set<bool,  &session_t::_show_rendered_visibility,UPDATE_SESSION>)
        .add_property("coordinate_system",&session_t::_coordinate_system,   &session_t::set<coordinate_system_t,  &session_t::_coordinate_system,   UPDATE_SESSION>)
        .add_property("scene",          &session_t::_scene)
        .add_property("error_handling_rules",&session_t::error_handling_rules)
        .def("update_session",  &session_t::scene_update)
        .def("load_mesh",       &session_t::load_mesh,bp::return_value_policy<bp::reference_existing_object>())
        .def("exit",            &session_t::exit);

    bp::class_<QMatrix4x4>("QMatrix4x4")
        .def("__init__", bp::make_constructor(&initMat, bp::default_call_policies()))
        .def("translate",      static_cast<void (QMatrix4x4::*)(float x, float y, float z) >(&QMatrix4x4::translate))
        .def("scale",          static_cast<void (QMatrix4x4::*)(float x, float y, float z) >(&QMatrix4x4::scale))
        .def("rotate",         static_cast<void (QMatrix4x4::*)(float a, float x, float y, float z) >(&QMatrix4x4::rotate))
        .def("dot",            static_cast<QMatrix4x4 & (QMatrix4x4::*)(QMatrix4x4 const & rhs) >(&QMatrix4x4::operator*=),bp::return_value_policy<bp::reference_existing_object>())
        .def(bp::self *= QMatrix4x4());

    bp::class_<pending_task_t, boost::noncopyable>("PendingTask", bp::no_init);

    bp::class_<std::vector<std::string> >("SArray");

    bp::class_<exec_env, boost::noncopyable>("ExecEnv", bp::no_init)
        .add_property("script_dir", &exec_env::_script_dir)
        .def("join", &exec_env::join)
        .def("emit", &exec_env::emitPendingTask,bp::return_value_policy<bp::reference_existing_object>());

    bp::enum_<PendingFlag>("PendingFlag")
        .value("thread",        PENDING_THREAD)
        .value("scene_edit",    PENDING_SCENE_EDIT)
        .value("file_write",    PENDING_FILE_WRITE)
        .value("texture_read",  PENDING_TEXTURE_READ)
        .value("file_read",     PENDING_FILE_READ)
        .value("all",           PENDING_ALL)
        .value("none",          PENDING_NONE);

    bp::class_<object_t, boost::noncopyable>("Object", bp::no_init)
        .add_property("name",           &object_t::_name)
        .def_readwrite("id",             &object_t::_id)
        .def_readwrite("visible",        &object_t::_visible)
        .def_readwrite("diffrot",        &object_t::_diffrot)
        .def_readwrite("difftrans",      &object_t::_difftrans)
        .add_property("trajectory",     &object_t::_trajectory)
        .add_property("transformation", &object_t::_transformation);

    typedef void (std::vector<frameindex_t>::*FrameindexPushBackReference)(const frameindex_t &);

    bp::class_<std::vector<frameindex_t> >("Frames")
        .def(bp::vector_indexing_suite<std::vector<frameindex_t> >())
        .def("popBack", &std::vector<frameindex_t>::pop_back)
        .def("pushBack",(FrameindexPushBackReference)&std::vector<frameindex_t>::push_back);

    bp::class_<camera_t,        boost::noncopyable,bp::bases<object_t> >("Camera", bp::init<std::string>());
    bp::class_<objl::Material,  boost::noncopyable>("Material")
        .add_property("ambient",        &objl::Material::Ka)
        .add_property("diffuse",        &objl::Material::Kd)
        .add_property("specular",       &objl::Material::Ks);
    bp::class_<objl::Mesh,      boost::noncopyable>("SubMesh")
        .add_property("material",       &objl::Mesh::_material)
        .add_property("triangles",      &objl::Mesh::Indices)
        .add_property("vertices",       &objl::Mesh::_vertices);
    bp::class_<mesh_object_t,   bp::bases<object_t> >("Mesh", bp::no_init)
        .add_property("material",       &mesh_object_t::_materials)
        .add_property("meshes",         &mesh_object_t::_meshes);
    bp::class_<texture_t,       boost::noncopyable>("Texture", bp::no_init);
    bp::class_<framelist_t,     boost::noncopyable>("Framelist", bp::no_init)
        .add_property("name",           &framelist_t::_name)
        .add_property("frames",         &framelist_t::_frames);

    bp::class_<scene_t, boost::noncopyable>("Scene")
        .def("get_camera",      &scene_t::get_camera,bp::return_value_policy<bp::reference_existing_object>())
        .def("get_mesh",        &scene_t::get_mesh,bp::return_value_policy<bp::reference_existing_object>())
        .def("add_mesh",        static_cast<mesh_object_t & (scene_t::*)(mesh_object_t const & )>(&scene_t::add_mesh),bp::return_value_policy<bp::reference_existing_object>())
        .def("add_camera",      static_cast<camera_t &(scene_t::*)(std::string const &) >(&scene_t::add_camera),bp::return_value_policy<bp::reference_existing_object>())
        .def("get_framelist",   &scene_t::get_framelist, bp::return_value_policy<bp::reference_existing_object>())
        .def("add_framelist",   static_cast<framelist_t &(scene_t::*)(framelist_t const &) >(&scene_t::add_framelist), bp::return_value_policy<bp::reference_existing_object>())
        .def("add_framelist",   static_cast<framelist_t &(scene_t::*)(std::string const &, std::string const &, bool, bool) >(&scene_t::add_framelist), bp::return_value_policy<bp::reference_existing_object>())
        .def("queue_screenshot", &scene_t::queue_handle);
//        .def("queue_screenhot", &scene_t::queue_handle);

    bp::def("trajectory2mesh",  static_cast<mesh_object_t (&)(std::string const &, object_t const &, time_t, time_t, uint32_t)>(trajectory2mesh));
    bp::def("exec",             exec_stdout);
    bp::def("connect",          SCENE::connect);
    bp::def("disconnect",       SCENE::disconnect);
    bp::def("sarray",           py_list_to_std_vector<std::string>);
    bp::def("screenshot",       screenshot_py);
    bp::def("get_programpath",  IO_UTIL::get_programpath);
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
        Py_Finalize();
    }catch (...){PyErr_Print();bp::handle_exception();}
    }
}
