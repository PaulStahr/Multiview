#ifndef LANG_H
#define LANG_H

#include "data.h"
#include <tuple>

const std::pair<size_t, const char*>                culling_values[]        = {{0,"None"},{1,"Front"},{2,"Back"},{3,"Front and Back"}};
const std::pair<RedrawScedule, const char*>         redraw_scedule_values[] = {{REDRAW_ALWAYS, "Always"},{REDRAW_AUTOMATIC, "Automatic"},{REDRAW_MANUAL, "Manual"}};
const std::pair<depthbuffer_size_t, const char*>    depthbuffer_values[]    = {{DEPTHBUFFER_16_BIT, "16 bit"},{DEPTHBUFFER_24_BIT, "24 bit"},{DEPTHBUFFER_32_BIT, "32 bit"}};
const std::tuple<coordinate_system_t, const char*, const char*>   coordinate_system_values[]={
    {COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS,    "spherical_approximated","Spherical Multipass"},
    {COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS,   "spherical_singlepass"  ,"Spherical Singlepass"},
    {COORDINATE_SPHERICAL_APPROXIMATED,         "spherical_multipass"   ,"Spherical Approximated"},
    {COORDINATE_EQUIRECTANGULAR,                "equirectangular"       ,"Equirectangular"},
    {COORDINATE_END,                            "invalid"               ,"Invalid"}};
const std::pair<viewtype_t, const char*>            viewtype_values[]       = {{VIEWTYPE_RENDERED, "Rendered"},{VIEWTYPE_FLOW,"Flow"},{VIEWTYPE_POSITION,"Position"},{VIEWTYPE_INDEX,"Index"},{VIEWTYPE_DEPTH,"Depth"}};
const std::pair<motion_blur_curve_t, const char*>   motionblur_curve_values[]={{MOTION_BLUR_CONSTANT,"Constant"},{MOTION_BLUR_LINEAR,"Linear"},{MOTION_BLUR_QUADRATIC,"Quadratic"},{MOTION_BLUR_CUBIC,"Cubic"},{MOTION_BLUR_CUSTOM,"Cubic"}};

namespace lang
{
    const char *get_animating_string(RedrawScedule rs);
    
    RedrawScedule gt_animating_value(const char* value);
    
    const char *get_motion_blur_curve_string(motion_blur_curve_t mpc);

    motion_blur_curve_t get_motion_blur_curve_value(const char* mpc);
    
    const std::tuple<coordinate_system_t, const char*, const char*> & get_coordinate_system_by_name(const char* mpc);

    const std::tuple<coordinate_system_t, const char*, const char*> & get_coordinate_system_by_enum(coordinate_system_t cs);
}

#endif
