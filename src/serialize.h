/*
Copyright (c) 2018 Paul Stahr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <ostream>
#include <istream>
#include <string>
#include <vector>

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
