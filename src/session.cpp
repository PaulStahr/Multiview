#include "session.h"

#include <future>
//#include <filesystem>
#include <experimental/filesystem>
#include <boost/algorithm/string/case_conv.hpp>
#include <fstream>
#include "OBJ_Loader.h"

#include "lang.h"
#include "python_binding.h"
#include "io_util.h"
#include "geometry_io.h"


//namespace fs = std::filesystem;
namespace fs = std::experimental::filesystem;

void session_t::scene_update(SessionUpdateType sup)
{
    _scene_updates.emplace_back(sup);
    _updateListener.erase(std::remove_if(_updateListener.begin(), _updateListener.end(), [sup](std::shared_ptr<session_updater_t> & ul){
        return !(*ul)(sup);
    }),_updateListener.end());
}

void session_t::wait_for_frame(wait_for_rendered_frame_t & wait_obj)
{
    std::lock_guard<std::mutex> lockGuard(_scene._mtx);
    _wait_for_rendered_frame_handles.push_back(&wait_obj);
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


template <typename T>
void convert_columns(std::vector<std::vector<float> > & anim_data, size_t index_column, T add_elem)
{
    if (index_column != std::numeric_limits<size_t>::max())
    {
        for (std::vector<float> & row : anim_data)
        {
            add_elem(row[index_column],row.data());
        }
    }
    else
    {
        for (std::vector<float> & row : anim_data)
        {
            add_elem(std::distance(anim_data.data(), &row),row.data());
        }
    }
}

template <typename T>
void convert_columns(std::vector<std::vector<float> > & anim_data, size_t column, size_t index_column, T add_elem)
{
    if (index_column != std::numeric_limits<size_t>::max())
    {
        for (std::vector<float> & row : anim_data)
        {
            add_elem(row[index_column],row.data() + column);
        }
    }
    else
    {
        for (std::vector<float> & row : anim_data)
        {
            add_elem(std::distance(anim_data.data(), &row),row.data() +column);
        }
    }
}

template <size_t N>
std::array<size_t, N> get_named_columns(std::vector<std::string> const & column_names, std::string *strIter)
{
    std::array<size_t,N> result;
    for (size_t i= 0; i < N; ++i)
    {
        result[i] = std::distance(column_names.begin(), std::find(column_names.begin(), column_names.end(), *strIter));
        ++strIter;
    }
    return result;
}

template <typename T>
T stov(std::string & ){throw std::runtime_error("Not implemented");}
template <> int32_t stov(std::string & str){return std::stoi (str);}
template <> int64_t stov(std::string & str){return std::stoll(str);}
template <> size_t  stov(std::string & str){return std::stoi (str);}
template <> float   stov(std::string & str){return std::stof (str);}
template <> bool    stov(std::string & str){return std::stoi (str);}

template <typename T>
bool read_or_print(T * ref, std::string *begin, std::string *end, SessionUpdateType & session_update, SessionUpdateType update, std::ostream & out)
{
    if (ref)
    {
        if (begin != end)
        {
            T tmp = stov<T>(*begin);
            if (tmp != *ref)
            {
                session_update |= update;
                *ref = tmp;
                return true;
            }
        }
        else
        {
            out << *ref << std::endl;
        }
    }
    return false;
}

void exec_impl(std::string input, exec_env & env, std::ostream & out, session_t & session, pending_task_t &pending_task)
{
    try
    {
        while (!input.empty() && input.back() == 10){input.pop_back();}
        std::vector<std::string> args;
        scene_t & scene = session._scene;
        IO_UTIL::split_in_args(args, input);
        if (args.empty()){
            pending_task.assign(PENDING_NONE);
            return;
        }
        //uint32_t *ref_uint32_t = nullptr;
        size_t  *ref_size_t  = nullptr;
        int32_t *ref_int32_t = nullptr;
        float   *ref_float_t = nullptr;
        bool    *ref_bool    = nullptr;
        frameindex_t *ref_frameindex_t = nullptr;
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
            out << "render_to_texture <texture> <camera> <type>" << std::endl;
            out << "write_texture <texture> <filename>" << std::endl;
            out << "camera <name>" << std::endl;
            out << "diffrot (<activated>)" << std::endl;
            out << "difftrans (<activated>)" << std::endl;
            out << "preresolution (<num_pixels>)" << std::endl;
            out << "coordinate_system (<spherical_approximated|spherical_singlepass|spherical_multipass|equidistant>)" << std::endl;
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
            if (args.size() > 1)
            {
                session._show_only = args[1] == "all" ? "" : args[1];
            }
            else
            {
                out << session._show_only << std::endl;
            }
        }
        else if (command == "frame" || command == "goto")   {ref_frameindex_t = &session._m_frame;   session_var |= UPDATE_FRAME;}
        else if (command == "play")                         {ref_int32_t = &session._play;}
        else if (command == "coordinate_system")            {
            if (args.size() > 1)
            {
                coordinate_system_t coordinate_system = std::get<0>(lang::get_coordinate_system_by_name(args[1].c_str()));
                if (coordinate_system == COORDINATE_END){throw std::runtime_error("Option " + args[1] + " not known");}
                if (coordinate_system != session._coordinate_system)
                {
                    session._coordinate_system = coordinate_system;
                    session.scene_update(UPDATE_SESSION);
                }
            }
            else
            {
                out << std::get<1>(lang::get_coordinate_system_by_enum(session._coordinate_system)) << std::endl;
            }
        }
        else if (command == "animating")
        {
            if (args.size() > 1)
            {
                RedrawScedule animating = lang::get_redraw_scedule_value(args[1].c_str());
                if (animating == REDRAW_END){throw std::runtime_error("Option " + args[1] + " not known");}
                if (animating != session._animating)
                {
                    session._animating = animating;
                    session.scene_update(UPDATE_ANIMATING);
                }
            }
            else
            {
                const char* res = lang::get_redraw_scedule_string(session._animating);
                if (!res){throw std::runtime_error("Unknown redraw type");}
                out << boost::algorithm::to_lower_copy(std::string(res))<< std::endl;
            }
        }
        else if (command == "status")
        {
            out << "textures" << std::endl;
            for (texture_t const & t: scene._textures)
            {
                out << t._id << ' ' << t._width << ' ' << t._height << ' ' << t._channels << ' ' << t._datatype << ' ' << t._name << std::endl;
            }
            out << "scene" << std::endl;
            out << "frame " << session._m_frame << std::endl;
            out << "texture_ids "       << gl_texture_id::count << std::endl;
            out << "buffer_ids "        << gl_buffer_id::count << std::endl;
            out << "framebuffer_ids "   << gl_framebuffer_id::count << std::endl;
            out << "renderbuffer_ids "  << gl_renderbuffer_id::count << std::endl;
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
            handle._type = lang::get_viewtype_type(args[5].c_str());
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
            handle._texture = std::move(args[1]);
            handle._camera= std::move(args[2]);
            handle._type = lang::get_viewtype_type(args[3].c_str());
            pending_task.assign(PENDING_FILE_WRITE | PENDING_TEXTURE_READ | PENDING_SCENE_EDIT);
            handle._task = RENDER_TO_TEXTURE;
            handle._state = screenshot_state_inited;
            handle._flip = true;
            scene.queue_handle(handle);
            pending_task.unset(PENDING_SCENE_EDIT);
            handle.wait_until(screenshot_state_rendered_texture);
            pending_task.unset(PENDING_TEXTURE_READ);
            handle.wait_until(screenshot_state_copied);
            if (handle._state == screenshot_state_error)
            {
                out << "error at getting texture" << std::endl;
            }
            pending_task.unset(PENDING_FILE_WRITE);
        }
        else if (command == "diffrot")      {ref_bool    = &session._diffrot;           session_var |= UPDATE_SESSION;}
        else if (command == "octree")       {ref_size_t  = &session._octree_batch_size; session_var |= UPDATE_SESSION;}
        else if (command == "premaps")      {ref_int32_t = &session._max_premaps;       session_var |= UPDATE_SESSION;}
        else if (command == "difftrans")    {ref_bool    = &session._difftrans;         session_var |= UPDATE_SESSION;}
        else if (command == "smoothing")    {ref_size_t  = &session._smoothing;         session_var |= UPDATE_SESSION;}
        else if (command == "fov")          {ref_float_t = &session._fov;               session_var |= UPDATE_SESSION;}
        else if (command == "crop")         {ref_bool    = &session._crop;              session_var |= UPDATE_SESSION;}
        else if (command == "autouiupdate") {ref_bool    = &session._auto_update_gui;   session_var |= UPDATE_SESSION;}
        else if (command == "culling")      {ref_size_t  = &session._culling;           session_var |= UPDATE_SESSION;}
        else if (command == "framedenominator"){ref_frameindex_t = &session._framedenominator;session_var |= UPDATE_SESSION;}
        else if (command == "motionblur")   {ref_frameindex_t = &session._motion_blur;  session_var |= UPDATE_SESSION;}
        else if (command == "motionblurcurve")
        {
            if (args.size() > 1)
            {
                session._motion_blur_curve = lang::get_motion_blur_curve_value(args[1].c_str());
                session_update |= UPDATE_SESSION;
            }
            else
            {
                out << lang::get_motion_blur_curve_string(session._motion_blur_curve);
            }
        }
        else if (command == "indirect")     {ref_bool    = &session._indirect_rendering;session_var |= UPDATE_FRAME;}
        else if (command == "depthbuffersize")
        {
            if (args.size() > 1)
            {
                depthbuffer_size_t elem = lang::get_depthbuffer_value(args[1].c_str());
                if (elem != DEPTHBUFFER_END){session._depthbuffer_size = elem; session_update |= UPDATE_SESSION;}
                else                        {out << "Unknown Argument for depth" << std::endl;}
            }
            else
            {
                out << std::get<1>(lang::get_depthbuffer_type(session._depthbuffer_size)) << std::endl;
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
            else if (args[1] == "visibility"){ref= &session._show_visibility;}
            else if (args[1] == "pos")      {ref = &session._show_position;}
            else if (args[1] == "depth")    {ref = &session._show_depth;}
            else if (args[1] == "curser")   {ref = &session._show_curser;}
            else if (args[1] == "arrows")   {ref = &session._show_arrows;}
            else if (args[1] == "framelists"){ref = &session._show_framelists;}
            else if (args[1] == "debug_info"){ref = &session._show_debug_info;}
            else{out << "error, key not known" << std::endl;return;}
            read_or_print(ref,&args[2], &*args.end(), session_update, UPDATE_SESSION, out);
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
            mesh_object_t *mesh = dynamic_cast<mesh_object_t*>(obj);
            camera_t *cam = dynamic_cast<camera_t*>(obj);
            if (!obj){throw std::runtime_error("object " + name + " not found");}
            if (args[2] == "transform")
            {
                obj->_transformation.setToIdentity();
                read_transformations(obj->_transformation, args.begin() + 3, args.end());
            }
            else if (args[2] == "transform_pipeline")
            {
                obj->_transform_pipeline.clear();
                auto begin = args.begin()+3;
                auto end   = args.end();
                std::unique_lock<std::mutex> lck(scene._mtx);
                while(begin != end)
                {
                    QMatrix4x4 matrix;
                    auto next = read_transformation(matrix, begin, end);
                    if (next != begin)
                    {
                        obj->_transform_pipeline.emplace_back(std::make_shared<constant_transform_t<QMatrix4x4> >(matrix), false);
                        begin = next;
                    }
                    else if (*begin == "anim")
                    {
                        ++begin;
                        assert_argument_count(std::distance(args.begin(), begin) + 1, args.size());
                        std::shared_ptr<object_transform_base_t> tr = scene.get_trajectory(*begin);
                        if(!tr){throw std::runtime_error(std::string("Key ") + *begin + std::string(" not found"));}
                        obj->_transform_pipeline.emplace_back(tr, false);
                    }
                    else if (*begin == "ianim")
                    {
                        ++begin;
                        assert_argument_count(std::distance(args.begin(), begin) + 1, args.size());
                        std::shared_ptr<object_transform_base_t> tr = scene.get_trajectory(*begin);
                        if(!tr){throw std::runtime_error(std::string("Key ") + *begin + std::string(" not found"));}
                        obj->_transform_pipeline.emplace_back(tr, true);
                    }
                    else
                    {
                        ++begin;
                    }
                }
            }
            else if (args[2] == "visible")   {
                if (args.size() > 4)
                {
                    bool visible = std::stoi(args[4]);
                    object_t *other = scene.get_object(args[3]);
                    if (!mesh)     mesh = dynamic_cast<mesh_object_t*>(other);
                    camera_t *cam = dynamic_cast<camera_t*>(obj);
                    if (!cam) cam = dynamic_cast<camera_t*>(other);
                    if (visible){SCENE::connect(*cam, *mesh);}
                    else        {SCENE::disconnect(*cam, *mesh);}
                }
                else
                {
                    read_or_print(&obj->_visible,&args[3], &*args.end(), session_update, UPDATE_SCENE, out);
                }
            }
            else if (mesh && args[2] == "ambient")   {
                vec3f_t ka(std::stof(args[3]),std::stof(args[4]),std::stof(args[5]));
                for (objl::Mesh & m : mesh->_meshes)
                {
                    if (!m._material){
                        m._material = std::shared_ptr<objl::Material>(new objl::Material(scene._null_material));
                    }
                    m._material->Ka = ka;
                }
            }
            else if (mesh && args[2] == "diffuse")   {
                vec3f_t kd(std::stof(args[3]),std::stof(args[4]),std::stof(args[5]));
                for (objl::Mesh & m : mesh->_meshes)
                {
                    if (!m._material){
                        m._material = std::shared_ptr<objl::Material>(new objl::Material(scene._null_material));
                    }
                    m._material->Kd = kd;
                }
            }
            else if (args[2] == "difftrans") {obj->_difftrans  = std::stoi(args[3]);}
            else if (args[2] == "diffrot")   {obj->_diffrot    = std::stoi(args[3]);}
            else if (args[2] == "trajectory"){obj->_trajectory = std::stoi(args[3]);}
            else if (args[2] == "id")        {obj->_id         = std::stoi(args[3]);}
            else if (cam && args[2] == "aperture")  {
                if      (args.size() == 2){out << cam->_aperture;}
                else if (args.size() == 3){cam->_aperture = vec2f_t(std::stof(args[3]));}
                else if (args.size() == 4){cam->_aperture = {std::stof(args[4]),std::stof(args[5])};}
            }
            else if (cam && args[2] == "wireframe") {cam->_wireframe = std::stoi(args[3]);}
            else
            {
                out << "error, key not known, valid keys are transform, visible, difftrans, diffrot, trajectory" << std::endl;
            }

            session_update |= UPDATE_SCENE;
        }
        else if (command == "print")
        {
            object_t *obj = scene.get_object(args[1]);
            if (!obj){throw std::runtime_error("object not found");}
            for (std::pair<std::shared_ptr<object_transform_base_t>, bool> elem : obj->_transform_pipeline)
            {
                object_transform_base_t *tr = elem.first.get();
                if (dynamic_cast<constant_transform_t<QMatrix4x4> *>(tr)) {out << "static Mat4x4 ";}
                if (dynamic_cast<dynamic_trajectory_t<vec3f_t>    *>(tr)) {out << "dynamic translation ";}
                if (dynamic_cast<dynamic_trajectory_t<rotation_t> *>(tr)) {out << "dynamic rotation ";}
            }
            out << std::endl;
        }
        else if (command == "run")
        {
            std::ifstream infile(args[1]);
            fs::path script = args[1];
            exec_env subenv(script.parent_path());
            if (!infile){throw std::runtime_error("error bad file: " + args[1]);}
            std::string line;
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
            std::string const & exec_file = args[1];
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
            if (!pipe) {throw std::runtime_error("popen() failed!");}
            //for windows _popen and _pclose
            exec_env subenv(args[1]);
            std::array<char, 1024> buffer;
            std::string line;
            std::vector<std::string> vars(args.begin() + 1, args.end());
            while (fgets(buffer.data(), buffer.size(), pipe.get())) {
                line = buffer.data();
                if (line.back() == '\n'){line.pop_back();}
                if (line.empty()){continue;}
                std::cout << "out " << line << std::endl;
                exec(line, vars, subenv, out, session, subenv.emitPendingTask(line));
            }
        }
        else if (command == "delete")
        {
            assert_argument_count(3, args.size());
            std::string const & name = args[2];
            if (args[1] == "camera")
            {
                std::lock_guard<std::mutex> lck(scene._mtx);
                camera_t *cam = scene.get_camera(name);
                if (!cam){throw std::runtime_error("Can't find camera " + name);}
                scene._cameras.erase(static_cast<std::vector<camera_t>::iterator>(cam));
            }
            else if (args[1] == "texture")
            {
                std::lock_guard<std::mutex> lck(scene._mtx);
                texture_t *tex = scene.get_texture(name);
                if (!tex){throw std::runtime_error("Can't find texture " + name);}
                scene._textures.erase(static_cast<std::vector<texture_t>::iterator>(tex));
            }
            else if (args[1] == "object")
            {
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
                std::string const & name = args[1];
                camera_t & cam = scene.add_camera(camera_t(name));
                read_transformations(cam._transformation, args.begin() + 2, args.end());
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
                assert_argument_count(6, args.size());
                pending_task.assign(PENDING_SCENE_EDIT);
                texture_t tex;
                tex._name = args[1];
                tex._width = std::stoi(args[2]);
                tex._height = std::stoi(args[3]);
                tex._channels = std::stoi(args[4]);
                tex._datatype = lang::get_gl_type_value(args[5].c_str());
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
        else if (command == "framelist" || command == "mframelist")
        {
            if (args.size() > 1)
            {
                assert_argument_count(3, args.size());
                std::string name = args[1];
                std::string framefilename = args[2];
                std::ifstream framefile(framefilename);
                std::vector<size_t> framelist = IO_UTIL::parse_framelist(framefile);
                if (command == "mframelist"){std::for_each(framelist.begin(), framelist.end(), UTIL::pre_decrement);}
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
                assert_argument_count(3, args.size());
                std::string const & name = args[1];
                std::string const & meshfile = args[2];
                clock_t current_time = clock();
                mesh_object_t m = mesh_object_t(name);
                objl::Loader loader;
                if (!loader.LoadFile(meshfile.c_str())){throw std::runtime_error("Could'n load object " + args[2]);}
                m._meshes = std::move(loader.LoadedMeshes);
                m._materials = std::move(loader.LoadedMaterials);
                clock_t after_mesh_loading_time = clock();
                std::cout << "mesh loading time: " << float( after_mesh_loading_time - current_time ) / CLOCKS_PER_SEC << std::endl;
                pending_task.unset(PENDING_FILE_READ);
                if (session._octree_batch_size)
                {
                    for (objl::Mesh & me : m._meshes)
                    {
                        me.octree = objl::create_octree(me, 0, me.Indices.size(), session._octree_batch_size);
                    }
                }
                clock_t after_octree_loading_time = clock();
                std::cout << "octree creation time: " << float(after_octree_loading_time - after_mesh_loading_time) / CLOCKS_PER_SEC << std::endl;
                auto begin = args.begin() + 3;
                if (args.size() > 3 && *begin == "compress")
                {
                    for (objl::Mesh & me : m._meshes)
                    {
                       objl::compress(me);
                    }
                    ++begin;
                    std::cout << "compressing time: " << float(clock() - after_octree_loading_time) / CLOCKS_PER_SEC << std::endl;
                }
                read_transformations(m._transformation, begin, args.end());
                {
                    std::lock_guard<std::mutex> lck(scene._mtx);
                    scene.add_mesh(std::move(m));
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
                read_or_print(& obj->_id, &args[2], &*args.end(), session_update, UPDATE_SCENE, out);
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
            std::string const & animfile = args[1];
            std::ifstream animss(animfile);
            if (!animss){throw std::runtime_error("Couldn't find animation file " + animfile);}
            clock_t current_time = clock();
            std::vector<std::string> column_names;
            std::vector<std::vector<float> > anim_data = IO_UTIL::parse_csv(animss, column_names);
            clock_t loading_time = clock();
            animss.close();
            pending_task.unset(PENDING_FILE_READ);
            size_t index_column = std::numeric_limits<size_t>::max();
            auto strIter = args.begin() + 2;
            bool named_columns = *strIter == "--named";
            if (named_columns)
            {
                ++strIter;
                if (*strIter == "frame")
                {
                    auto tmp = std::find(column_names.begin(), column_names.end(), args[2]);
                    if (tmp == column_names.end()){throw std::runtime_error("Column not found");}
                    index_column = std::distance(column_names.begin(), tmp);
                }
            }
            else
            {
                auto iter = std::find(strIter, args.end(), "frame");
                if (iter != args.end())
                {
                    index_column = std::stoi(*(++iter));
                }
            }
            size_t column = 0;
            std::multimap<std::string, std::shared_ptr<object_transform_base_t> > trajectories;
            for (; strIter != args.end();)
            {
                std::string const & field = *strIter;
                ++strIter;
                if (field == "skip")
                {
                    int32_t to_skip = std::stoi(*strIter);
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
                    object_t *obj = scene.get_object(field);
                    if (!obj){out << "Warning didn't found object " << field << std::endl;}
                    std::string const & type = *strIter;
                    if (type == "pos")
                    {
                        std::shared_ptr<dynamic_trajectory_t<vec3f_t> >pos = std::make_shared<dynamic_trajectory_t<vec3f_t> >();
                        IO_UTIL::append(pos->_name, field, '_', type);
                        auto & key_transforms = pos->_key_transforms;
                        if (named_columns)
                        {
                            std::array<size_t, 3> cols = get_named_columns<3>(column_names, &*strIter + 1);
                            strIter += cols.size();
                            convert_columns(anim_data, index_column, [&key_transforms, &cols](size_t idx, float* data){key_transforms[idx]={data[cols[0]],data[cols[1]],data[cols[2]]};});                            
                        }
                        else
                        {
                            convert_columns(anim_data, column, index_column, [&key_transforms](size_t idx, float* data){key_transforms[idx]={data[0],data[1],data[2]};});
                        }
                        trajectories.insert({field, pos});
                        scene._trajectories.push_back(pos);
                        column += 3;
                    }
                    else if (type == "rot")
                    {
                        std::shared_ptr<dynamic_trajectory_t<rotation_t> >pos = std::make_shared<dynamic_trajectory_t<rotation_t> >();
                        IO_UTIL::append(pos->_name, field, '_', type);
                        auto & key_transforms = pos->_key_transforms;
                        if (named_columns)
                        {
                            std::array<size_t, 4> cols = get_named_columns<4>(column_names, &*strIter + 1);
                            strIter += cols.size();
                            convert_columns(anim_data, index_column, [&key_transforms, &cols](size_t idx, float* data){key_transforms[idx]={data[cols[0]],data[cols[1]],data[cols[2]],data[cols[3]]};});                            
                        }
                        else
                        {
                            convert_columns(anim_data, column, index_column, [&key_transforms](size_t idx, float* data){key_transforms[idx]={data[0],data[1],data[2],data[3]};});
                        }
                        trajectories.insert({field, pos});
                        scene._trajectories.push_back(pos);
                        column += 4;
                    }
                    else if (type == "erot" || type == "erotd")
                    {
                        std::shared_ptr<dynamic_trajectory_t<rotation_t> >pos = std::make_shared<dynamic_trajectory_t<rotation_t> >();
                        IO_UTIL::append(pos->_name, field, '_', type);
                        auto & key_transforms = pos->_key_transforms;
                        vec3f_t v = {stof(strIter[1]), stof(strIter[2]), stof(strIter[3])};
                        if (named_columns){column = get_named_columns<1>(column_names, &*strIter + 1)[0];}
                        if (type == "erot") {convert_columns(anim_data, column, index_column, [&key_transforms, &v](size_t idx, float* data){key_transforms[idx]= euleraxis2quaternion(v[0], v[1], v[2],data[0]);});}
                        else                {convert_columns(anim_data, column, index_column, [&key_transforms, &v](size_t idx, float* data){key_transforms[idx]= euleraxis2quaternion(v[0], v[1], v[2],data[0] * (M_PI / 180));});}
                        trajectories.insert({field, pos});
                        scene._trajectories.push_back(pos);
                        strIter += 3;
                        column += 1;
                    }
                    else if (type == "aperture")
                    {
                        camera_t *cam = dynamic_cast<camera_t *>(obj);
                        if (!cam){throw std::runtime_error("Object is not a camera");}
                        auto & key_transforms = cam->_key_aperture;
                        if (named_columns){column = get_named_columns<1>(column_names, &*strIter + 1)[0];}
                        convert_columns(anim_data, column, index_column, [&key_transforms](size_t idx, float* data){key_transforms[idx]=data[0];});
                        column += 1;
                    }
                    else
                    {
                        out << "Warning unknown type";
                    }
                    ++strIter;
                }
            }
            for (size_t i=0;i<scene.num_objects();++i)
            {
                object_t& obj=scene.get_object(i);
                auto range = trajectories.equal_range(obj._name);
                for (auto elem = range.first; elem != range.second; ++elem)
                {
                    if (dynamic_cast<dynamic_trajectory_t<rotation_t>*>(elem->second.get())){obj._transform_pipeline.emplace_back(elem->second, false);}
                }
                for (auto elem = range.first; elem != range.second; ++elem)
                {
                    if (dynamic_cast<dynamic_trajectory_t<vec3f_t>*>(elem->second.get())){obj._transform_pipeline.emplace_back(elem->second, false);}
                }
            }
            std::cout << "anim loading time: " << float( loading_time - current_time) / CLOCKS_PER_SEC << anim_data.size() << " trajctory creation: " << float(clock() - loading_time) / CLOCKS_PER_SEC << std::endl;
            session_update = UPDATE_SCENE;
        }
        else
        {
            out << "Unknown command: " << input << std::endl;
        }
        read_or_print(ref_int32_t,      &args[1], &*args.end(), session_update, session_var, out);
        read_or_print(ref_float_t,      &args[1], &*args.end(), session_update, session_var, out);
        read_or_print(ref_bool,         &args[1], &*args.end(), session_update, session_var, out);
        read_or_print(ref_frameindex_t, &args[1], &*args.end(), session_update, session_var, out);
        read_or_print(ref_size_t,       &args[1], &*args.end(), session_update, session_var, out);
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

void session_t::add_update_listener(std::shared_ptr<session_updater_t>& sut)
{
    _updateListener.push_back(sut);
}

template<typename R>bool is_ready(std::future<R> const& f){return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

auto init_array() -> std::array<std::string, 32>
{
    std::array<std::string, 32> res;
    for (size_t i = 0; i < res.size(); ++i)
        res[i] = "${" + std::to_string(i) + "}";
    return res;
}

/*
void find_and_replace(std::string & data, std::map<std::string, std::string> vars)
{
    std::string result;
    size_t last = 0;
    size_t pos = data.find('$');
    if (pos != std::string::npos)
    {
        if (data[pos + 1] == '$')
        {
            ++pos;
            result.append(data.begin() + last, data.begin() + pos);
        }
        else if (data[pos + 1] == '{')
        {
            
        }
        return;
    }
    result.append(data.begin(), data.begin() + pos);
}*/

const std::array<std::string, 32> var_literals = init_array();

void exec(std::string input, std::vector<std::string> const & variables, exec_env & env, std::ostream & out, session_t & session, pending_task_t & pending_task)
{
    input.erase(std::find(input.begin(), input.end(), '#'), input.end());
    if (input.empty())
    {
        pending_task.assign(PENDING_NONE);
        return;
    }
    IO_UTIL::find_and_replace_all(input, "${sdir}", env._script_dir);
    for (size_t i = 0; i < variables.size(); ++i)
    {
        IO_UTIL::find_and_replace_all(input,var_literals[i], variables[i]);
    }
    if (input.back() == '\n')
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
        if (input.back() == '&')
        {
            try
            {
                input.pop_back();
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
