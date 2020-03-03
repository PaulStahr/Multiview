#include "qt_util.h"

#include <algorithm>
#include <cmath>
#include "util.h"

bool contains_nan(QMatrix4x4 const & mat)
{
    return std::any_of(mat.constData(), mat.constData() + 16, UTIL::isnan<float>);
}
