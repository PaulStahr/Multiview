#include "cmd.h"

std::array<std::string, 32> init_array()
{
    std::array<std::string, 32> res;
    for (size_t i = 0; i < res.size(); ++i)
        res[i] = "${" + std::to_string(i) + "}";
    return res;
}
