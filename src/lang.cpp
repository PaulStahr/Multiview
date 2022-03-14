#include "lang.h"
#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>

namespace lang
{
    const char *get_animating_string(RedrawScedule rs)
    {
        auto iter = std::find_if(redraw_scedule_values, redraw_scedule_values + 3, [rs](auto elem){return elem.first == rs;});
        return iter == redraw_scedule_values + 3 ? nullptr : iter->second;
    }
    
    RedrawScedule gt_animating_value(const char* value)
    {
        auto iter = std::find_if(redraw_scedule_values, redraw_scedule_values + 3, [value](auto elem){return boost::iequals(elem.second, value);});
        return iter == redraw_scedule_values + 3 ? REDRAW_INVALID : iter->first;
    }
    
    const char *get_motion_blur_curve_string(motion_blur_curve_t mpc)
    {
        auto iter = std::find_if(motionblur_curve_values, motionblur_curve_values + 4, [mpc](auto elem){return elem.first == mpc;});
        return iter == motionblur_curve_values + 4 ? nullptr : iter->second;
    }

    motion_blur_curve_t get_motion_blur_curve_value(const char* value)
    {
        auto iter = std::find_if(motionblur_curve_values, motionblur_curve_values + 4, [value](auto elem){return boost::iequals(elem.second, value);});
        return iter == motionblur_curve_values + 4 ? MOTION_BLUR_INVALID : iter->first;
    }
}
