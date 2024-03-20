/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_SIZE_H
#define ARX_SIZE_H
#include "arxengine/ArxDefines.h"
#include "arxengine/misc/Math.h"
#include <type_traits>
#include <glm/vec2.hpp>

ARX_NAMESPACE_BEGIN



template<typename T>
struct Size
{
    Size()
        : Size(0.0f, 0.0f)
    {
    }
    
    Size(T widthIn, T heightIn)
        : width(widthIn), height(heightIn)
    {
    }

    friend Size<T> operator+(const Size<T> &s1, const Size<T> &s2)
    {
        return Size<T>(s1.width + s2.width, s1.height + s2.height);
    }

    friend Size<T> operator+(const glm::vec<2, T> &vec, const Size<T> &s)
    {
        return Size<T>(vec.x + s.width, vec.y + s.height);
    }

    friend Size<T> operator+(const Size<T> &s, const glm::vec<2, T> &vec)
    {
        return Size<T>(vec.x + s.width, vec.y + s.height);
    }
    
    bool operator==(const Size<T> &size) const
    {
        return AreNumsEqual(width, size.width) && AreNumsEqual(height, size.height);
    }

    bool operator!=(const Size<T> &size) const
    {
        return !(*this == size);
    }

    void *GetPtr() 
    { 
        return (void*)(this); 
    }

    std::enable_if_t<std::is_arithmetic_v<T>, T> width;
    T height;

    static const Size<T> DEFAULT_SIZE;
    static const Size<T> IGNORE_SIZE; 
};

template<typename T>
const Size<T> Size<T>::DEFAULT_SIZE = Size<T>(1, 1);

template<typename T>
const Size<T> Size<T>::IGNORE_SIZE = Size<T>(constants::IGNORE_VALUE, constants::IGNORE_VALUE);

using SizeF = Size<float>;
using SizeI = Size<int32_t>;
using SizeU = Size<uint32_t>;
using SizeUL = Size<uint64_t>;

ARX_NAMESPACE_END

#endif
