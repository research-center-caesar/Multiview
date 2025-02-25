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

#ifndef IO_UTIL_H
#define IO_UTIL_H

#include <ostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>

#if __has_include("fast_float/include/fast_float/fast_float.h") 
#define FAST_FLOAT
#endif
#ifdef FAST_FLOAT
#include "fast_float/include/fast_float/fast_float.h"
#endif

#include <charconv>

class NullBuffer : public std::streambuf
{
public:
  int overflow(int c);
};

class NullStream : public std::ostream {
    public: 
       NullStream();
    private:
        NullBuffer m_sb;
};

bool ends_with(std::string const & value, std::string const & ending);

std::string get_next_free_filenumber(std::string const & filename, std::string const & suffix);

std::string get_next_free_filename(std::string const & folder, std::string const & filename, std::string const & suffix);

template <class InputIt>
std::ostream & print_elements(std::ostream & out, InputIt begin, InputIt end)
{
    while (begin != end)
    {
        out << *begin;
        ++begin;
    }
    return out;
}

template <typename V>
struct print_as_struct
{
    print_as_struct(){}

    template <typename T>
    std::ostream & operator()(std::ostream & out, T const & elem) const
    {
        return out << static_cast<V>(elem);
    }
};

template <typename V>
static const print_as_struct<V> print_as;

template <typename F>
struct print_convert_struct
{
    F _func;
    print_convert_struct(F func_) : _func(func_){};
    
    template <typename T>
    std::ostream & operator()(std::ostream & out, T const & elem) const
    {
        return out << _func(elem);
    }
};

template <typename F>
print_convert_struct<F> print_convert(F func)
{
    return print_convert_struct<F>(func);
}

struct print_struct
{
    print_struct();
    
    template <typename T>
    std::ostream & operator()(std::ostream & out, T const & elem) const
    {
        return out << elem;
    }
};

static const print_struct print;

struct printer_struct
{
    printer_struct();
    template <typename T>
    std::ostream & operator()(std::ostream & out, T const & elem) const
    {
        return out << elem;
    }
};

static const printer_struct printer;

struct print_dummy_t
{
    print_dummy_t(){}
    template <typename T>
    std::ostream & operator()(std::ostream & out, T const &)
    {
        return out;
    }
};
//inline print_dummy_t::print_dummy_t() = default;

static const print_dummy_t print_dummy;

template <typename InputIter, typename PrintFunction>
struct print_iter_element
{
    InputIter _iter;
    PrintFunction _print;
    print_iter_element(InputIter iter_, PrintFunction print_) : _iter(iter_), _print(print_){}
    std::ostream & operator()(std::ostream & out, size_t index){return _print(out, _iter[index]);}
};

template <typename InputIter>
print_iter_element<InputIter, print_struct> get_print_element_func(InputIter iter)
{
    return print_iter_element<InputIter, print_struct>(iter, print);
}

template <typename InputIter, typename PrintFunction>
print_iter_element<InputIter, PrintFunction> get_print_element_func(InputIter iter, PrintFunction print)
{
    return print_iter_element<InputIter, PrintFunction>(iter, print);
}

template <class InputIt, class Seperator, class PrintFunction>
std::ostream & print_elements(std::ostream & out, InputIt begin, InputIt end, Seperator const & seperator, PrintFunction func)
{
    if (begin != end)
    {
        func(out,*begin);
        while (++begin != end)
        {
            func(out << seperator,*begin);
        }
    }
    return out;
}

template <class Container, class Seperator, class PrintFunction>
std::ostream & print_elements(std::ostream & out, Container const & cont, Seperator const & seperator, PrintFunction func)
{
    return print_elements(out, cont.cbegin(), cont.cend(), seperator, func);
}

template <class InputIt, class Seperator>
std::ostream & print_elements(std::ostream & out, InputIt begin, InputIt end, Seperator const & seperator)
{
    typedef typename std::iterator_traits<InputIt>::value_type value_t;
    return print_elements(out, begin, end, seperator, [](std::ostream & outl, value_t const & val) -> std::ostream &{return outl << val;});
}

template <class Container, class Seperator>
std::ostream & print_elements(std::ostream & out, Container const & cont, Seperator const & seperator)
{
    return print_elements(out, cont.cbegin(), cont.cend(), seperator);
}

template <typename Container>
std::ostream & print_matrix(std::ostream & out, Container const & vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        print_elements(out, vec[i].begin(), vec[i].end(), ' ')<<std::endl;
    }
    return out;
}

template <typename InputIter>
std::ostream & print_matrix(std::ostream & out, size_t width, size_t height, InputIter iter)
{
    for (size_t i = 0; i < height; ++i)
    {
        for (size_t j = 0; j < width; ++j)
        {
            if (j != 0)
            {
                out << ' ';
            }
            out << *iter;
            ++iter;
        }
        out << std::endl;
    }
    return out;
}

namespace IO_UTIL
{
bool find_and_replace_all(std::string & data, std::string const & toSearch, std::string const & replaceStr);
    
std::string do_readlink(std::string const& path);

std::string get_programpath();

std::string get_selfpath();

std::string read_file(std::string const & file);

template <typename ... Ts>
std::string & append (std::string & res, Ts && ... multi_inputs)
{
    ([&] (auto & input) {res += input;} (multi_inputs), ...);
    return res;
}

template <typename T>
struct string_to_struct : std::unary_function<std::string, T>
{
    string_to_struct(){}
    
    T operator()(std::string const & str) const;
};

template<typename T>
T string_to_struct<T>::operator()(const std::string& str) const
{
    T res;
    std::stringstream ss(str);
    if (!(ss >> res))
    {
        std::string error;
        error += '"';
        error += str;
        error += "\" not castable to ";
        error += typeid(T).name();
        throw std::invalid_argument(error);
    }
    return res;
}

template <>
struct string_to_struct<bool>: std::unary_function<std::string, bool>
{
    bool operator()(std::string const & str) const;
};

template <typename T>
extern const string_to_struct<T> string_to = string_to_struct<T>();

std::vector<std::vector<float> > parse_csv(std::istream & stream, std::vector<std::string> & column_names);

std::vector<size_t> parse_framelist(std::istream & stream);

void split_in_args(std::vector<std::string>& qargs, std::string const & command);

template <class UnaryPredicate>
class split_iterator
{
private:
    std::string::const_iterator _in_beg;
    std::string::const_iterator _in_end;
    std::string::const_iterator _beg;
    std::string::const_iterator _end;
    UnaryPredicate _p;
    std::string word;
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::string_view;
    using pointer = value_type*;
    using reference = value_type&;

    split_iterator (const std::string &in_, UnaryPredicate p_);
    split_iterator(const split_iterator&);
    ~split_iterator();
    void str(std::string const & str);
    void str(std::string::const_iterator in_beg, std::string::const_iterator in_end);
    split_iterator& operator=(const split_iterator&);
    split_iterator& operator++();
    inline std::string_view operator*() const;
    inline std::string_view remaining() const;
    std::string & get(std::string & ptr);
    std::string::const_iterator begin();
    std::string::const_iterator end();
    split_iterator end_iterator() const;
    bool valid() const;
    bool operator==(split_iterator<UnaryPredicate> const & other) const;
    bool operator!=(split_iterator<UnaryPredicate> const & other) const;
    inline std::errc parse (float & result);
    inline void parse (int32_t & result);
    inline void parse (int64_t & result);
    inline std::errc increment_and_parse(float & result);
    inline void increment_and_parse(int32_t & result);
    inline bool increment_and_parse(int64_t & result);
    template <typename Parser, typename T> inline bool increment_and_parse(T & result, Parser & p);
    inline size_t size(){return std::distance(_beg, _end);}
    const char& operator[](size_t idx){return _beg[idx];}

    //friend void swap(split_iterator<UnaryPredicate>& lhs, split_iterator<UnaryPredicate>& rhs);
};

template<class UnaryPredicate>
split_iterator<UnaryPredicate> split_iterator<UnaryPredicate>::end_iterator() const
{
    split_iterator<UnaryPredicate> tmp = *this;
    tmp._beg = tmp._end = tmp._in_end;
    return tmp;
}

template<class UnaryPredicate>
std::string_view split_iterator<UnaryPredicate>::remaining() const
{
    return std::string_view(&*_beg, std::distance(_beg, _in_end));
}

template<class UnaryPredicate>
std::string_view split_iterator<UnaryPredicate>::operator*() const
{
    return std::string_view(&*_beg, std::distance(_beg, _end));
}

template<class UnaryPredicate>
split_iterator<UnaryPredicate>::split_iterator (const std::string &in_, UnaryPredicate p_) : _in_beg(in_.begin()), _in_end(in_.end()), _beg(in_.begin()), _end(in_.begin()), _p(p_){
    ++(*this);
}

template<class UnaryPredicate>
std::string& split_iterator<UnaryPredicate>::get(std::string & ptr) {ptr.assign(_beg, _end); return ptr;}

template<class UnaryPredicate>
split_iterator<UnaryPredicate> & split_iterator<UnaryPredicate>::operator++(){
    _beg = std::find_if_not(_end, _in_end, _p);
    _end = std::find_if(_beg, _in_end, _p);
    return *this;
}

template<class UnaryPredicate>
void split_iterator<UnaryPredicate>::str(const std::string& str)
{
    _end = _beg = _in_beg = str.begin();
    _in_end = str.end();
    ++(*this);
}

template<class UnaryPredicate>
void split_iterator<UnaryPredicate>::str(std::string::const_iterator in_beg, std::string::const_iterator in_end)
{
    _end = _beg = _in_beg = in_beg;
    _in_end = in_end;
    ++(*this);
}

template<class UnaryPredicate>
std::string::const_iterator split_iterator<UnaryPredicate>::begin(){return _beg;}

template<class UnaryPredicate>
std::string::const_iterator split_iterator<UnaryPredicate>::end(){return _end;}

template<class UnaryPredicate>
bool split_iterator<UnaryPredicate>::valid() const{return _beg != _in_end;}

template<class UnaryPredicate>
bool split_iterator<UnaryPredicate>::operator==(split_iterator<UnaryPredicate> const & other) const
{
    return    _in_beg == other._in_beg
            &&_in_end == other._in_end
            &&_beg    == other._beg
            &&_end    == other._end;
}

template<class UnaryPredicate>
bool split_iterator<UnaryPredicate>::operator!=(split_iterator<UnaryPredicate> const & other) const{return !(*this == other);}

template <class UnaryPredicate>
split_iterator<UnaryPredicate> make_split_iterator(std::string const & str, UnaryPredicate p){return split_iterator<UnaryPredicate>(str, p);}

template <class UnaryPredicate>
std::errc split_iterator<UnaryPredicate>::parse(float & result){
    #ifdef FAST_FLOAT
    return fast_float::from_chars(&*begin(), &*end(), result).ec;
    #else
    try{result = std::stof(get(word))}
    catch(std::invalid_argument const & e){return std::invalid_argument;}
    #endif
}

template <class UnaryPredicate>
void split_iterator<UnaryPredicate>::parse(int32_t & result){
    std::from_chars(&*begin(),&*end(), result);
}

template <class UnaryPredicate>
void split_iterator<UnaryPredicate>::parse(int64_t & result){
    std::from_chars(&*begin(),&*end(), result);
}

template <class UnaryPredicate>
std::errc split_iterator<UnaryPredicate>::increment_and_parse(float & result){
    #ifdef FAST_FLOAT
    _beg = std::find_if_not(_end, _in_end, _p);
    fast_float::from_chars_result res = fast_float::from_chars(&*begin(), &*_in_end, result);
    _end = static_cast<std::string::const_iterator>(std::find_if(res.ptr, &*_in_end, _p));
    return res.ec;
    #else
    ++(*this);
    result = std::stof(get(word));
    return std::errc();
    #endif
}

template <class UnaryPredicate>
void split_iterator<UnaryPredicate>::increment_and_parse(int32_t & result){
    _beg = std::find_if_not(_end, _in_end, _p);
    std::from_chars_result res = std::from_chars(&*begin(),&*_in_end, result);
    _end = static_cast<std::string::const_iterator>(std::find_if(res.ptr, &*_in_end, _p));
}

template <class UnaryPredicate>
bool split_iterator<UnaryPredicate>::increment_and_parse(int64_t & result){
    _beg = std::find_if_not(_end, _in_end, _p);
    if (_beg != _in_end)
    {
        std::from_chars_result res = std::from_chars(&*begin(),&*_in_end, result);
        _end = static_cast<std::string::const_iterator>(std::find_if(res.ptr, &*_in_end, _p));
        return true;
    }
    return false;
}

template <class UnaryPredicate>
template <typename Parser, typename T>
bool split_iterator<UnaryPredicate>::increment_and_parse(T & result, Parser & p){
    _beg = std::find_if_not(_end, _in_end, _p);
    if (_beg != _in_end)
    {
        std::from_chars_result res = p(&*begin(),&*_in_end, result);
        _end = static_cast<std::string::const_iterator>(std::find_if(res.ptr, &*_in_end, _p));
        return true;
    }
    return false;
}

template<class UnaryPredicate>
split_iterator<UnaryPredicate>::~split_iterator(){}
}

#endif
