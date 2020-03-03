#include "serialize.h"

namespace SERIALIZE 
{
template <>
std::istream & read_value<std::string>(std::istream & in, std::string & value)
{
    value.clear();
    std::string::value_type tmp;
    read_values(in, std::back_inserter(value), tmp);
    return in;
}

template <>
std::ostream & write_value<std::string>(std::ostream & out, std::string const & value)
{
    write_values(out, value.begin(), value.end());
    return out;
}
}