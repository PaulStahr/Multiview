#ifndef CMD_H
#define CMD_H
#include <array>
#include <cstddef>
#include <string>

std::array<std::string, 32> init_array();

const std::array<std::string, 32> var_literals = init_array();

#endif
