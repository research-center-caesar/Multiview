#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

#include "util.h"
#include <vector>

template <typename Iter>
void flip(Iter pixels, size_t width, size_t height)
{
    Iter low = pixels;
    Iter high = pixels + width * (height - 1);
    while (low < high)
    {
        std::swap_ranges(low, low + width, high);
        low += width;
        high -= width;
    }
}

class advanced_image_base_t
{
    size_t _channels;
    size_t _width;
    size_t _height;
};

template<typename T>
class advanced_image_t : advanced_image_base_t
{
    std::vector<T> _data;
     
    advanced_image_t & operator += (advanced_image_t const & other){std::transform(_data.begin(), _data.end(), other._data.begin(), _data._begin(), std::plus<T>());}
    advanced_image_t & operator -= (advanced_image_t const & other){std::transform(_data.begin(), _data.end(), other._data.begin(), _data._begin(), std::minus<T>());}
    advanced_image_t & operator *= (advanced_image_t const & other){std::transform(_data.begin(), _data.end(), other._data.begin(), _data._begin(), std::multiplies<T>());}    
    advanced_image_t & operator /= (advanced_image_t const & other){std::transform(_data.begin(), _data.end(), other._data.begin(), _data._begin(), std::divides<T>());}    
    advanced_image_t & operator += (T value){std::for_each(_data.begin(), _data.end(), UTIL::add_to(value));}    
    advanced_image_t & operator -= (T value){std::for_each(_data.begin(), _data.end(), UTIL::subtract_from(value));}
    advanced_image_t & operator *= (T value){std::for_each(_data.begin(), _data.end(), UTIL::mult_by(value));}
    advanced_image_t & operator /= (T value){std::for_each(_data.begin(), _data.end(), UTIL::divide_by(value));}
};

#endif
