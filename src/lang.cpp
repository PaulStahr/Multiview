#include "lang.h"
#include <algorithm>
#include <array>
#include <boost/algorithm/string/predicate.hpp>

namespace lang
{
const std::array<std::pair<GLuint, const char*>, 8> gl_types = {{
    {GL_UNSIGNED_BYTE,  "uint8"},
    {GL_BYTE,           "int8"},
    {GL_UNSIGNED_SHORT, "uint16"},
    {GL_SHORT,          "int16"},
    {GL_UNSIGNED_INT,   "uint32"},
    {GL_INT,            "int32"},
    {GL_FLOAT,          "float"},
    {GL_INVALID_ENUM,   "invalid"}}};
const std::array<std::tuple<coordinate_system_t, const char*, const char*>, 5> coordinate_system_values={{
    {COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS,    "spherical_approximated","Spherical Multipass"},
    {COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS,   "spherical_singlepass"  ,"Spherical Singlepass"},
    {COORDINATE_SPHERICAL_APPROXIMATED,         "spherical_multipass"   ,"Spherical Approximated"},
    {COORDINATE_EQUIRECTANGULAR,                "equirectangular"       ,"Equirectangular"},
    {COORDINATE_END,                            "invalid"               ,"Invalid"}}};
const std::array<std::pair<viewtype_t, const char*>, 7> viewtype_values= {{
    {VIEWTYPE_RENDERED, "Rendered"},
    {VIEWTYPE_FLOW,     "Flow"},
    {VIEWTYPE_POSITION, "Position"},
    {VIEWTYPE_INDEX,    "Index"},
    {VIEWTYPE_DEPTH,    "Depth"},
    {VIEWTYPE_VISIBILITY,"Visibility"},
    {VIEWTYPE_END,      nullptr}}};
const std::array<std::pair<motion_blur_curve_t, const char*>, 6> motionblur_curve_values={{
    {MOTION_BLUR_CONSTANT,  "Constant"},
    {MOTION_BLUR_LINEAR,    "Linear"},
    {MOTION_BLUR_QUADRATIC, "Quadratic"},
    {MOTION_BLUR_CUBIC,     "Cubic"},
    {MOTION_BLUR_CUSTOM,    "Cubic"},
    {MOTION_BLUR_END,       nullptr}}
};
const std::array<std::pair<RedrawScedule, const char*>, 4>         redraw_scedule_values = {{{REDRAW_ALWAYS, "Always"},{REDRAW_AUTOMATIC, "Automatic"},{REDRAW_MANUAL, "Manual"},{REDRAW_END, nullptr}}};
const std::array<const std::pair<size_t, const char*>, 5>          culling_values        = {{{0,"None"},{1,"Front"},{2,"Back"},{3,"Front and Back"},{4,nullptr}}};

    
    const char *get_culling_string(size_t value)
    {
        return std::find_if(culling_values.begin(), culling_values.end() - 1, [value](auto elem){return elem.first == value;}) -> second;
    }
    
    size_t get_culling_value(const char* value)
    {
        return std::find_if(culling_values.begin(), culling_values.end() - 1, [&value](auto elem){return elem.second == value;}) -> first;
    }

    const char *get_redraw_scedule_string(RedrawScedule value)
    {
        return std::find_if(redraw_scedule_values.begin(), redraw_scedule_values.end() - 1, [value](auto elem){return elem.first == value;}) -> second;
    }
    
    RedrawScedule get_redraw_scedule_value(const char* value)
    {
        return std::find_if(redraw_scedule_values.begin(), redraw_scedule_values.end() - 1, [&value](auto elem){return boost::iequals(elem.second, value);}) -> first;
    }

    const char *get_depthbuffer_string(size_t value)
    {
        return depthbuffer_values[value].second;
    }

    depthbuffer_size_t get_depthbuffer_value(const char* value)
    {
        return std::find_if(depthbuffer_values, depthbuffer_values + 4, [&value](auto elem){return boost::iequals(elem.second, value);}) -> first;
    }
    
    const char *get_motion_blur_curve_string(motion_blur_curve_t mpc)
    {
        return std::find_if(motionblur_curve_values.begin(), motionblur_curve_values.end(), [mpc](auto elem){return elem.first == mpc;}) -> second;
    }

    motion_blur_curve_t get_motion_blur_curve_value(const char* value)
    {
        return std::find_if(motionblur_curve_values.begin(), motionblur_curve_values.end() - 1, [value](auto elem){return boost::iequals(elem.second, value);})->first;
    }
    
    viewtype_t get_viewtype_type(const char* value)
    {
        return std::find_if(viewtype_values.begin(), viewtype_values.end() - 1, [value](auto elem){return boost::iequals(elem.second, value);}) -> first;
    }

    const char* get_viewtype_value(viewtype_t value)
    {
        return std::find_if(viewtype_values.begin(), viewtype_values.end() - 1, [value](auto elem){return elem.first == value;}) -> second;
    }

    const std::tuple<coordinate_system_t, const char*, const char*> & get_coordinate_system_by_name(const char* value)
    {
        return *std::find_if(coordinate_system_values.begin(), coordinate_system_values.end() - 1, [value](auto elem){return boost::iequals(std::get<1>(elem), value) || boost::iequals(std::get<2>(elem), value);}); 
    }

    const std::tuple<coordinate_system_t, const char*, const char*> & get_coordinate_system_by_enum(coordinate_system_t cs)
    {
        return *std::find_if(coordinate_system_values.begin(), coordinate_system_values.end() - 1, [cs](auto elem){return std::get<0>(elem) == cs;});
    }
    
    GLuint get_gl_type_value(const char* value)
    {
        return std::find_if(gl_types.begin(), gl_types.end() - 1, [value](auto elem){return boost::iequals(elem.second, value);})->first;
    }
    
    const char* get_gl_type_string(GLuint value)
    {
        return std::find_if(gl_types.begin(), gl_types.end() - 1, [value](auto elem){return value == elem.first;})->second;
    }
}
