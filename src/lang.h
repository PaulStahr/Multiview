#ifndef LANG_H
#define LANG_H

#include "data.h"

const std::pair<size_t, const char*>                culling_values[]        = {{0,"None"},{1,"Front"},{2,"Back"},{3,"Front and Back"}};
const std::pair<RedrawScedule, const char*>         redraw_scedule_values[] = {{REDRAW_ALWAYS, "Always"},{REDRAW_AUTOMATIC, "Automatic"},{REDRAW_MANUAL, "Manual"}};
const std::pair<depthbuffer_size_t, const char*>    depthbuffer_values[]    = {{DEPTHBUFFER_16_BIT, "16 bit"},{DEPTHBUFFER_24_BIT, "24 bit"},{DEPTHBUFFER_32_BIT, "32 bit"}};
const std::pair<coordinate_system_t, const char*>   coordinate_system_values[]={{COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS, "Spherical Multipass"},{COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS, "Spherical Singlepass"},{COORDINATE_SPHERICAL_APPROXIMATED, "Spherical Approximated"}};
const std::pair<viewtype_t, const char*>            viewtype_values[]       = {{VIEWTYPE_RENDERED, "Rendered"},{VIEWTYPE_FLOW,"Flow"},{VIEWTYPE_POSITION,"Position"},{VIEWTYPE_INDEX,"Index"},{VIEWTYPE_DEPTH,"Depth"}};

namespace lang
{
    const char *get_animating_string(RedrawScedule rs);
    
    RedrawScedule gt_animating_value(const char* value);
}

#endif
