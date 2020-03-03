#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <ostream>
#include <istream>
#include <string>
#include <vector>
#include <iostream>

namespace SERIALIZE
{
template <typename T>
std::istream & read_value(std::istream & in, T & value)
{
    if (!in.read(reinterpret_cast<char*>(&value), sizeof(T)))
    {
        throw std::runtime_error("Bad input Stream");
    }
    return in;
}

template <typename T>
std::ostream & write_value(std::ostream & out, T const & value)
{
    if (!out.write(reinterpret_cast<const char*>(&value), sizeof(T)))
    {
        throw std::runtime_error("Bad input Stream");
    }
    return out;
}

template <typename InputIterator>
std::ostream & write_values(std::ostream & out, InputIterator begin, InputIterator end)
{
    size_t size = std::distance(begin, end);
    write_value(out, size);
    for (; begin != end; ++begin)
    {
        write_value(out, *begin);
    }
    return out;
}

template <typename OutputIterator, typename T>
std::istream & read_values(std::istream & in, OutputIterator out, T & value, size_t size)
{
    for (; size --> 0; ++out)
    {
        read_value(in, value);
        *out = value;
    }
    return in;
}

template <typename OutputIterator, typename T>
std::istream & read_values(std::istream & in, OutputIterator out, T & value)
{
    size_t size;
    read_value(in, size);
    return read_values(in, out, value, size);
}

template <typename U>
std::istream & read_value(std::istream & in, std::vector<U> & value)
{
    value.clear();
    U tmp;
    size_t size;
    read_value(in, size);
    std::cout <<"read vector size:"<< size << std::endl;
    value.reserve(size);
    read_values(in, std::back_inserter(value), tmp, size);
    return in;
}

template <typename U>
std::ostream & write_value(std::ostream & out, std::vector<U> const & value)
{
    return write_values(out, value.begin(), value.end());
}

template <>
std::istream & read_value<std::string>(std::istream & in, std::string & value);

template <>
std::ostream & write_value<std::string>(std::ostream & out, std::string const & value);

}


#endif
