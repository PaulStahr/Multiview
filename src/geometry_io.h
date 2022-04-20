#pragma once

#include <iostream>
#include "io_util.h"

template <typename T, size_t N>
std::ostream & operator << (std::ostream & out, matharray<T,N> const & array)
{
    return print_elements(out << '(', array.begin(), array.end(),' ') << ')';
}
