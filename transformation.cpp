#include "transformation.h"

#include <cmath>

std::array<float, 2> kart_to_equidistant(std::array<float, 3> const & data)
{
    float len = sqrt(data[0] * data[0] + data[1] * data[1]);
    len = atan2(len, data[2])/(len * (M_PI * 2));
    return std::array<float, 2>({data[0] * len, data[1] * len});
}
