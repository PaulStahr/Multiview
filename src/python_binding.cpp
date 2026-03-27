#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/operators.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/numpy.hpp>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector4D>
#include <iostream>
#include <filesystem>
#include "session.h"
#include "python_binding.h"
#include "data.h"
#include "io_util.h"
#include "mesh.h"
#include "geometry.h"

namespace bp = boost::python;
namespace np = boost::python::numpy;

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

template <class T, T V>
struct template_constant
{
    template_constant(){};    
    constexpr operator T() const { return V; }
    template <typename W>
    constexpr T operator () (W const &) const {return V;}
    template <typename W, typename X>
    constexpr T operator ()(W const &, X const &)const {return V;}
};

class GilRelease {
    PyThreadState* state;
public:
    GilRelease() : state(PyEval_SaveThread()) {}
    ~GilRelease() { PyEval_RestoreThread(state); }
};

static template_constant<bool, true> const logical_true;
static template_constant<bool, false> const logical_false;
static template_constant<size_t, 4> const unsigned_four;

template <class T, T V>
T template_constant_function() {return V;}
    
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
        session,
        output,
        viewtype,
        camera,
        width,
        height,
        vcam,
        ignore_nan,
        background);
}


static bool numpy_inited = false;

static void init_numpy()
{
    if (!numpy_inited)
    {
        np::initialize();
        numpy_inited = true;
    }
}

static objl::VertexArrayHighres* vertices_from_array(
    np::ndarray position,
    np::ndarray normal,
    np::ndarray texcoord)
{
    if (!numpy_inited)
    {
        np::initialize();
        numpy_inited = true;
    }
    if (position.get_nd() != 2 || position.shape(1) != 3)
        PyErr_SetString(PyExc_ValueError, "Position array must be Nx3");
    if (normal.get_nd() != 2 || normal.shape(1) != 3)
        PyErr_SetString(PyExc_ValueError, "Normal array must be Nx3");
    if (texcoord.get_nd() != 2 || texcoord.shape(1) != 2)
        PyErr_SetString(PyExc_ValueError, "Texcoord array must be Nx2");

    int N = position.shape(0);
    if (normal.shape(0) != N || texcoord.shape(0) != N)
        PyErr_SetString(PyExc_ValueError, "All arrays must have the same number of vertices");

    if (position.get_dtype() != np::dtype::get_builtin<float>())
        PyErr_SetString(PyExc_TypeError, "Position must be float32");
    if (normal.get_dtype() != np::dtype::get_builtin<int16_t>())
        PyErr_SetString(PyExc_TypeError, "Normal must be int16");
    if (texcoord.get_dtype() != np::dtype::get_builtin<uint16_t>())
        PyErr_SetString(PyExc_TypeError, "Texcoord must be uint16");

    std::vector<objl::VertexHighres> verts;
    verts.reserve(N);

    for (int i = 0; i < N; ++i) {
        float* p = reinterpret_cast<float*>(position.get_data() + i * position.strides(0));
        int16_t* n = reinterpret_cast<int16_t*>(normal.get_data() + i * normal.strides(0));
        uint16_t* t = reinterpret_cast<uint16_t*>(texcoord.get_data() + i * texcoord.strides(0));

        verts.emplace_back(objl::VertexHighres{
            {p[0], p[1], p[2]},
            {n[0], n[1], n[2]},
            {t[0], t[1]}
        });
    }

    return new objl::VertexArrayHighres(verts);
}

static std::vector<triangle_t>* triangles_from_array(np::ndarray array)
{
    if (array.get_nd() != 2 || array.shape(1) != 3)
        PyErr_SetString(PyExc_ValueError, "Array must be of shape (N, 3)");

    if (array.get_dtype() != np::dtype::get_builtin<uint32_t>())
        PyErr_SetString(PyExc_TypeError, "Array must be of dtype uint32");

    int N = array.shape(0);
    std::vector<triangle_t> triangles;
    triangles.reserve(N);

    for (int i = 0; i < N; ++i) {
        uint32_t* tri = reinterpret_cast<uint32_t*>(array.get_data() + i * array.strides(0));
        triangles.push_back({tri[0], tri[1], tri[2]});
    }

    return new std::vector<triangle_t>(std::move(triangles));
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

np::ndarray get_screenshot_data(screenshot_handle_t & handle) {
    if (!numpy_inited)
    {
        np::initialize();
        numpy_inited = true;
    }
    if (handle.get_state() != screenshot_state_copied)
    {
        throw std::runtime_error("Screenshot handle in wrong state" + std::to_string(handle.get_state()));
    }
    if (!handle.has_data())
    {
        throw std::runtime_error("No texture-data to get");
    }
    //Py_intptr_t shape[3] = {
    bp::tuple shape = bp::make_tuple(static_cast<long int>(handle._height),static_cast<long int>(handle._width),static_cast<long int>(handle._channels));
    switch (handle.get_datatype())
    {
        case GL_UNSIGNED_BYTE:
        {
            np::ndarray result = np::empty(shape, np::dtype::get_builtin<uint8_t>());
            uint8_t* data = handle.get_data<uint8_t>();
            std::copy(data, data + handle.num_elements(), reinterpret_cast<uint8_t*>(result.get_data()));
            return result;
        }
        case GL_FLOAT:
        {
            np::ndarray result = np::empty(shape, np::dtype::get_builtin<float>());
            float* data = handle.get_data<float>();
            std::copy(data, data + handle.num_elements(), reinterpret_cast<float*>(result.get_data()));
            return result;
        }
    }
    throw std::runtime_error("Type not supported");
}

QQuaternion fromEulerAngles(float x, float y, float z)
{
    return QQuaternion::fromEulerAngles(x,y,z);
}

void wait_until_wrapper(screenshot_handle_t& handle, screenshot_state st)
{
    if (!handle.check_state(st))
    {
        GilRelease release;
        handle.wait_until(st);
    }
}

template<typename T>
void setitem(T &v, bp::object index, bp::object value) {
    bp::extract<int> idx(index);
    if (idx.check()) {
        // Single index
        int i = idx();
        if (i < 0) i += v.size();
        if (i < 0 || i >= (int)v.size()) {
            PyErr_SetString(PyExc_IndexError, "Index out of range");
            bp::throw_error_already_set();
        }
        v[i] = bp::extract<float>(value);
    } else {
        // Slice case
        bp::slice s = bp::extract<bp::slice>(index);
        bp::object start_obj = s.start();
        bp::object stop_obj = s.stop();
        bp::object step_obj = s.step();

        int start = start_obj == bp::object() ? 0 : (int)bp::extract<int>(start_obj);
        int stop = stop_obj == bp::object() ? 3 : (int)bp::extract<int>(stop_obj);
        int step = step_obj == bp::object() ? 1 : (int)bp::extract<int>(step_obj);

        // Normalize indices like Python
        if (start < 0) start += v.size();
        if (stop < 0) stop += v.size();

        if (step == 0) {
            PyErr_SetString(PyExc_ValueError, "slice step cannot be zero");
            bp::throw_error_already_set();
        }
        int num_steps = 0;

        if (step > 0 && start < stop)
            num_steps = (stop - start + step - 1) / step;
        else if (step < 0 && start > stop)
            num_steps = (start - stop - step - 1) / step;
        else if (step < 0 && start < stop){
            start += v.size() - 1;
            stop -= v.size() + 1;
            num_steps = (start - stop - step - 1) / step;
        }

        int laststep = start + (num_steps - 1) * step;
        bp::list val_list = bp::extract<bp::list>(value);
        num_steps = std::min((boost::python::ssize_t)num_steps, bp::len(val_list));

        if (start < 0 || start >= (int)v.size()|| laststep < 0 || laststep >= (int)v.size())
        {
            PyErr_SetString(PyExc_IndexError, "slice index out of range");
            bp::throw_error_already_set();
        }

        if (num_steps != bp::len(val_list)) {
            PyErr_SetString(PyExc_ValueError, "Length mismatch in slice assignment");
            bp::throw_error_already_set();
        }

        int out_index = start;
        for (int i = 0; i < num_steps; ++i, out_index += step) {
            v[out_index] = bp::extract<float>(val_list[i]);
        }
    }
}


template <typename Ret, typename Class, typename Arg>
auto wrap_method_without_gil(Ret (Class::*method)(Arg))
{
    return [method](Class& self, Arg arg) -> Ret {
        GilRelease release;
        return (self.*method)(arg);
    };
}

BOOST_PYTHON_MODULE(Multiview)
{
    bp::enum_<SessionUpdateType>("SessionUpdateType")
        .value("none",       UPDATE_NONE)
        .value("animating",  UPDATE_ANIMATING)
        .value("redraw",     UPDATE_REDRAW)
        .value("session",    UPDATE_SESSION)
        .value("scene",      UPDATE_SCENE)
        .value("frame",      UPDATE_FRAME)
        .value("shader",     UPDATE_SHADER).export_values();

    bp::def("session_update_or", static_cast<SessionUpdateType (*)(SessionUpdateType, SessionUpdateType)>(&operator|));
    bp::def("session_update_and", static_cast<SessionUpdateType (*)(SessionUpdateType, SessionUpdateType)>(&operator&));

    bp::enum_<RedrawScedule>("RedrawScedule")
        .value("redraw_always",     REDRAW_ALWAYS)
        .value("redraw_automatic",  REDRAW_AUTOMATIC)
        .value("redraw_manual",     REDRAW_MANUAL);

    bp::enum_<coordinate_system_t>("CoordinateSystem")
        .value("spherical_approximated", COORDINATE_SPHERICAL_APPROXIMATED)
        .value("spherical_multipass",    COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS)
        .value("spherical_singlepass",   COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS)
        .value("equirectangular",        COORDINATE_EQUIRECTANGULAR)
        .value("custom",                 COORDINATE_CUSTOM);

    bp::enum_<depthbuffer_size_t>("DepthBufferSize")
        .value("depthbuffer_16",        DEPTHBUFFER_16_BIT)
        .value("depthbuffer_24",        DEPTHBUFFER_24_BIT)
        .value("depthbuffer_32",        DEPTHBUFFER_32_BIT);

    bp::enum_<screenshot_task>("ScreenshotTask")
        .value("take_screenshot",       TAKE_SCREENSHOT)
        .value("save_texture",          SAVE_TEXTURE)
        .value("render_to_texture",     RENDER_TO_TEXTURE);

    bp::enum_<screenshot_state>("ScreenshotState")
        .value("screenshot_state_inited",           screenshot_state_inited)
        .value("screenshot_state_queued",           screenshot_state_queued)
        .value("screenshot_state_glqueued",         screenshot_state_gl_queued)
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
        .value("visibility",VIEWTYPE_VISIBILITY)
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
        .def("get_datatype",            &screenshot_handle_t::get_datatype)
        .def("set_datatype",            &screenshot_handle_t::set_datatype)
        .def("wait_until",              &wait_until_wrapper)
        .def("get_data",                &get_screenshot_data)
        .def("has_data",                &screenshot_handle_t::has_data);

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
        .add_property("depthbuffer_size",&session_t::_depthbuffer_size, &session_t::set<depthbuffer_size_t, &session_t::_depthbuffer_size, UPDATE_SESSION>)
        .add_property("depth_scale",    &session_t::_depth_scale,    &session_t::set<float, &session_t::_depth_scale,    UPDATE_SESSION>)
        .add_property("motionblur",     &session_t::_m_frame,        &session_t::set<frameindex_t,   &session_t::_motion_blur,     UPDATE_SESSION>)
        .add_property("frame",          &session_t::_m_frame,        &session_t::set<frameindex_t,   &session_t::_m_frame,         UPDATE_SESSION>)
        .add_property("framedenominator",&session_t::_m_frame,       &session_t::set<frameindex_t,   &session_t::_framedenominator,UPDATE_SESSION>)
        .add_property("fov",            &session_t::_fov,            &session_t::set<float, &session_t::_fov,            UPDATE_SESSION>)
        .add_property("znear",          &session_t::_znear,          &session_t::set<float, &session_t::_znear,          UPDATE_SHADER>)
        .add_property("zfar",           &session_t::_zfar,           &session_t::set<float, &session_t::_zfar,           UPDATE_SHADER>)
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
        .add_property("coordinate_system",&session_t::_coordinate_system,   &session_t::set<coordinate_system_t,  &session_t::_coordinate_system, UPDATE_SESSION>)
        .add_property("light_direction",&session_t::_light_direction,&session_t::set<vec3f_t,&session_t::_light_direction,UPDATE_SESSION>)
        .add_property("scene",          &session_t::_scene)
        .def("queue_screenshot",        &session_t::queue_handle)
        .add_property("error_handling_rules",&session_t::error_handling_rules)
        .def("update_session",
        +[](session_t& self, SessionUpdateType t)
        {
            GilRelease release;
            self.scene_update(t);
        }).def("get_object_transform",    &session_t::get_object_transform)
        .def("load_mesh",               &session_t::load_mesh,bp::return_value_policy<bp::reference_existing_object>())
        .def("exit",                    &session_t::exit);

    bp::class_<QVector4D>("QVector4D", bp::init<float, float, float, float>())
        .def("__getitem__", static_cast<float & (QVector4D::*)(int)>(&QVector4D::operator[]),bp::return_value_policy<bp::copy_non_const_reference>())
        .def("__len__", &template_constant_function<size_t, 4>);


    bp::class_<QQuaternion>("QQuaternion", bp::init<QVector4D const &>())
        .def("fromEulerAngles", &fromEulerAngles).staticmethod("fromEulerAngles")
        .def("toVector4D", static_cast<QVector4D (QQuaternion::*)() const>(&QQuaternion::toVector4D))
        .def("__len__", &template_constant_function<size_t, 4>);

    bp::class_<QMatrix4x4>("QMatrix4x4")
        .def("__init__", bp::make_constructor(&initMat, bp::default_call_policies()))
        .def("set2identity",   &QMatrix4x4::setToIdentity)
        .def("translate",      static_cast<void (QMatrix4x4::*)(float x, float y, float z) >(&QMatrix4x4::translate))
        .def("scale",          static_cast<void (QMatrix4x4::*)(float x, float y, float z) >(&QMatrix4x4::scale))
        .def("rotate",         static_cast<void (QMatrix4x4::*)(float a, float x, float y, float z) >(&QMatrix4x4::rotate))
        .def("rotate",         static_cast<void (QMatrix4x4::*)(QQuaternion const & v) >(&QMatrix4x4::rotate))
        .def("row",            static_cast<QVector4D (QMatrix4x4::*)(int index) const> (&QMatrix4x4::row))
        .def("column",         static_cast<QVector4D (QMatrix4x4::*)(int index) const> (&QMatrix4x4::column))
        .def("__getitem__", +[](QMatrix4x4 &m, bp::tuple idx) -> float {return m(bp::extract<int>(idx[0]), bp::extract<int>(idx[1]));})
        .def("__setitem__", +[](QMatrix4x4 &m, bp::tuple idx, float value) {m(bp::extract<int>(idx[0]), bp::extract<int>(idx[1])) = value;})
        .def("dot",            static_cast<QMatrix4x4 & (QMatrix4x4::*)(QMatrix4x4 const & rhs) >(&QMatrix4x4::operator*=),bp::return_value_policy<bp::reference_existing_object>())
        .def(bp::self *= QMatrix4x4());

    bp::class_<pending_task_t, boost::noncopyable>("PendingTask", bp::no_init);

    bp::class_<std::vector<std::string> >("SArray")
        .def(bp::vector_indexing_suite<std::vector<std::string> >());

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

    bp::enum_<DRAWTYPE::drawtype>("DrawType")
        .value("frameline",     DRAWTYPE::frameline)
        .value("line",          DRAWTYPE::line)
        .value("wireframe",     DRAWTYPE::wireframe)
        .value("solid",         DRAWTYPE::solid);
    

    bp::class_<std::vector<objl::VertexLowres> >("VertexArrayDataLowres", bp::no_init);
    bp::class_<std::vector<objl::VertexHighres> >("VertexArrayDataHighres", bp::no_init);

    bp::class_<objl::VertexArrayHighres, boost::noncopyable>("VertexArrayHighres", bp::no_init)
        .def("from_array", &vertices_from_array, bp::return_value_policy<bp::manage_new_object>())
        .add_property("data",       &objl::VertexArrayHighres::_data)
        .staticmethod("from_array");

    bp::class_<std::vector<triangle_t>>("Triangles")
        .def("from_array", &triangles_from_array, bp::return_value_policy<bp::manage_new_object>())
        .staticmethod("from_array");
        
    bp::class_<object_transform_base_t, boost::noncopyable>("Trajectory", bp::no_init);

    bp::class_<vec3f_t>("Vector3f", bp::init<>())
        .def(bp::init<float, float, float>())
        .def("__getitem__", static_cast<float & (vec3f_t::*)(size_t)>(&vec3f_t::operator[]),bp::return_value_policy<bp::copy_non_const_reference>())
        .def("__setitem__", setitem<vec3f_t>)
        .add_property("x", static_cast<float (vec3f_t::*)()>(&vec3f_t::get<0>),&vec3f_t::set<0>)
        .add_property("y", static_cast<float (vec3f_t::*)()>(&vec3f_t::get<1>),&vec3f_t::set<1>)
        .add_property("z", static_cast<float (vec3f_t::*)()>(&vec3f_t::get<2>),&vec3f_t::set<2>)
        .def("__len__", &vec3f_t::size);


    bp::class_<rotation_t>("Rotation")
        .def("__getitem__", static_cast<float & (rotation_t::*)(size_t)>(&rotation_t::operator[]),bp::return_value_policy<bp::copy_non_const_reference>())
        .def("__setitem__", setitem<rotation_t>)
        .add_property("x", static_cast<float (rotation_t::*)()>(&rotation_t::get<0>),&rotation_t::set<0>)
        .add_property("y", static_cast<float (rotation_t::*)()>(&rotation_t::get<1>),&rotation_t::set<1>)
        .add_property("z", static_cast<float (rotation_t::*)()>(&rotation_t::get<2>),&rotation_t::set<2>)
        .add_property("w", static_cast<float (rotation_t::*)()>(&rotation_t::get<3>),&rotation_t::set<3>)
        .def("inverse",    &rotation_t::inverse)
        .def("normalize",  &rotation_t::normalize)
        .def("normalized", &rotation_t::normalized)
        .def("__len__", &rotation_t::size);
        
    bp::class_<std::map<frameindex_t, vec3f_t> >("TrajectoryTranslate")
        .def(bp::map_indexing_suite<std::map<frameindex_t, vec3f_t> >());

    bp::class_<std::map<frameindex_t, rotation_t> >("TrajectoryRotate")
        .def(bp::map_indexing_suite<std::map<frameindex_t, rotation_t> >());

    bp::class_<dynamic_trajectory_t<vec3f_t>, boost::noncopyable,bp::bases<object_transform_base_t> >("DynamicPositionTrajectory", bp::no_init)
        .add_property("key_transforms",&dynamic_trajectory_t<vec3f_t>::_key_transforms);

    bp::class_<dynamic_trajectory_t<rotation_t>, boost::noncopyable,bp::bases<object_transform_base_t> >("DynamicPositionTrajectory", bp::no_init)
        .add_property("key_transforms",&dynamic_trajectory_t<rotation_t>::_key_transforms);

    bp::class_<std::pair<std::shared_ptr<object_transform_base_t>, bool> >("TransformPipelineEntry", bp::no_init)
        .add_property("transform", &std::pair<std::shared_ptr<object_transform_base_t>, bool>::first)
        .add_property("invert",    &std::pair<std::shared_ptr<object_transform_base_t>, bool>::second);
        
    bp::class_<std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool> > >("TransformPipeline", bp::no_init)
        .def(bp::vector_indexing_suite<std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool> > >())
        .def("push_back", static_cast<void (std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool>>::*)(const std::pair<std::shared_ptr<object_transform_base_t>, bool>&)>(&std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool>>::push_back))
        .def("pop_back",  &std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool> >::pop_back)
        .def("clear",     &std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool> >::clear)
        .def("__len__",   &std::vector<std::pair<std::shared_ptr<object_transform_base_t>, bool> >::size);

    bp::class_<object_t, boost::noncopyable>("Object", bp::no_init)
        .add_property("name",           &object_t::_name)
        .def_readwrite("id",             &object_t::_id)
        .def_readwrite("visible",        &object_t::_visible)
        .def_readwrite("diffrot",        &object_t::_diffrot)
        .def_readwrite("transform_pipeline",&object_t::_transform_pipeline)
        .def_readwrite("difftrans",      &object_t::_difftrans)
        .def_readwrite("depth_offset",   &object_t::_depth_offset)
        .add_property("trajectory",     &object_t::_trajectory)
        .add_property("transformation", &object_t::_transformation);

    
    typedef void (std::vector<objl::Mesh>::*MeshPushBackReference)(const objl::Mesh &);
    bp::class_<std::vector<objl::Mesh> >("Meshes")
        .def("popBack", &std::vector<objl::Mesh>::pop_back)
        .def("pushBack",(MeshPushBackReference)&std::vector<objl::Mesh>::push_back);

    typedef void (std::vector<frameindex_t>::*FrameindexPushBackReference)(const frameindex_t &);
    bp::class_<std::vector<frameindex_t> >("Frames")
        .def(bp::vector_indexing_suite<std::vector<frameindex_t> >())
        .def("popBack", &std::vector<frameindex_t>::pop_back)
        .def("pushBack",(FrameindexPushBackReference)&std::vector<frameindex_t>::push_back);

    bp::class_<std::vector<std::shared_ptr<objl::Material> > >("Materials")
        .def(bp::vector_indexing_suite<std::vector<std::shared_ptr<objl::Material> > >());

    //bp::register_ptr_to_python<std::shared_ptr<objl::Material> >();

    bp::class_<camera_t,        boost::noncopyable,bp::bases<object_t> >("Camera", bp::init<std::string>())
        .def_readwrite("projection_map",        &camera_t::_projection_map_file);

    typedef objl::Material&(std::shared_ptr<objl::Material>::*MaterialSharedPointerDereferenceRef)();

    bp::class_<std::shared_ptr<objl::Material> >("MaterialPointer")
        .def("get",              (MaterialSharedPointerDereferenceRef)&std::shared_ptr<objl::Material>::operator*, bp::return_value_policy<bp::reference_existing_object>());
    ;
    bp::class_<objl::Material,  boost::noncopyable>("Material", bp::init())
        .def_readwrite("name",          &objl::Material::name)
        .add_property("ambient",        &objl::Material::Ka)
        .add_property("diffuse",        &objl::Material::Kd)
        .add_property("specular",       &objl::Material::Ks)
        .def_readwrite("alpha",         &objl::Material::d);
    bp::class_<objl::Mesh,      boost::noncopyable>("SubMesh", bp::init<std::string const &, std::vector<objl::VertexHighres> const &, std::vector<triangle_t> const & >())
        .add_property("material",       &objl::Mesh::_material)
        .add_property("triangles",      &objl::Mesh::Indices)
        .add_property("vertices",       &objl::Mesh::_vertices)
        .add_property("material",
              +[](const objl::Mesh& self) { return self._material; },
              +[](objl::Mesh& self, std::shared_ptr<objl::Material> const& mat) { self._material = mat; });
    bp::class_<mesh_object_t,   bp::bases<object_t> >("Mesh", bp::init<std::string const &>())
        .add_property("materials",      &mesh_object_t::_materials)
        .add_property("meshes",         &mesh_object_t::_meshes)
        .def_readwrite("dt",            &mesh_object_t::_dt);
    bp::class_<texture_t,       boost::noncopyable>("Texture", bp::no_init);
    bp::class_<framelist_t,     boost::noncopyable>("Framelist", bp::no_init)
        .add_property("name",           &framelist_t::_name)
        .add_property("frames",         &framelist_t::_frames);

    bp::class_<std::vector<camera_t>, boost::noncopyable>("Cameras", bp::no_init)
        .def("__iter__", bp::iterator<std::vector<camera_t>, bp::return_internal_reference<>>());

    bp::class_<scene_t, boost::noncopyable>("Scene")
        .def("get_camera",      &scene_t::get_camera,bp::return_value_policy<bp::reference_existing_object>())
        .def("get_mesh",        &scene_t::get_mesh,bp::return_value_policy<bp::reference_existing_object>())
        .def("add_mesh",        static_cast<mesh_object_t & (scene_t::*)(mesh_object_t const & )>(&scene_t::add_mesh),bp::return_value_policy<bp::reference_existing_object>())
        .def("add_camera",      static_cast<camera_t &(scene_t::*)(std::string const &) >(&scene_t::add_camera),bp::return_value_policy<bp::reference_existing_object>())
        .def("get_framelist",   &scene_t::get_framelist, bp::return_value_policy<bp::reference_existing_object>())
        .def("add_framelist",   static_cast<framelist_t &(scene_t::*)(framelist_t const &) >(&scene_t::add_framelist), bp::return_value_policy<bp::reference_existing_object>())
        .def("add_framelist",   static_cast<framelist_t &(scene_t::*)(std::string const &, std::string const &, bool, bool) >(&scene_t::add_framelist), bp::return_value_policy<bp::reference_existing_object>())
        .def("get_trajectory",  &scene_t::get_trajectory_pt, bp::return_value_policy<bp::reference_existing_object>())
        .def("get_trojectory",        +[](scene_t & sc,std::string const & s){return sc.get_trajectory_pt(s);},bp::return_value_policy<bp::reference_existing_object>())
        .add_property("cameras",      &scene_t::_cameras)
        .add_property("trajectories", &scene_t::_trajectories);
//        .def("queue_screenhot", &scene_t::queue_handle);

    bp::def("trajectory2mesh",  static_cast<mesh_object_t (&)(std::string const &, object_t const &, time_t, time_t, uint32_t)>(trajectory2mesh));
    bp::def("exec",             exec_stdout);
    bp::def("connect",          SCENE::connect);
    bp::def("disconnect",       SCENE::disconnect);
    bp::def("sarray",           py_list_to_std_vector<std::string>);
    bp::def("screenshot",       screenshot_py);
    bp::def("get_programpath",  IO_UTIL::get_programpath);
    bp::def("init_numpy",     &init_numpy);
    bp::def("removenan",        removenan);
}

namespace PYTHON{
#if PY_MAJOR_VERSION >= 3
#   define INIT_MODULE PyInit_Multiview
    extern "C" PyObject* INIT_MODULE();
#else
#   define INIT_MODULE initMultiview
    extern "C" void INIT_MODULE();
#endif

class python_environment
{
public:
    python_environment(){
        PyImport_AppendInittab("Multiview", &PyInit_Multiview);
        Py_Initialize();
        PyEval_InitThreads();
        assert(PyEval_ThreadsInitialized());
    }

    ~python_environment(){
        Py_Finalize();
    }
};

static python_environment* pe = nullptr;

void exit()
{
    delete pe;
    pe = nullptr;
}

void run(std::string const & file, exec_env & env, session_t *session, std::vector<std::string> const & argv){
    if (!std::filesystem::exists(file))
    {
        throw std::runtime_error("File " + file + " doesn't exist");
    }
    try{
        if (!pe)
        {
            pe = new python_environment();
        }
        wchar_t** argvc = new wchar_t*[argv.size()];
        bp::object main = bp::import("__main__");
        bp::dict global = bp::extract<bp::dict>(main.attr("__dict__"));
        bp::object a = bp::import("Multiview");
        bp::object s(boost::ref(session));
        exec_env *tmp = &env;
        bp::object e(boost::ref(tmp));
        Py_BEGIN_ALLOW_THREADS
        PyGILState_STATE state = PyGILState_Ensure();
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
        /*for (size_t i = 0; i < argv.size(); ++i)
        {
            delete[] argvc[i];
        }
        delete[] argvc;*/
    }catch (...){PyErr_Print();bp::handle_exception();}
    }
}
