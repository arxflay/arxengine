/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_DATAEXTRACTOR_H
#define ARX_DATAEXTRACTOR_H
#include "arxengine/ArxDefines.h"
#include <cstddef>
#include <type_traits>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <string_view>
ARX_NAMESPACE_BEGIN

class DataExtractor
{
public:
    DataExtractor(const uint8_t *beginPtr, size_t contentSize)
        : m_currentPos(beginPtr), m_end(beginPtr + contentSize)
    {
    }
    
    constexpr void ThrowIfOutOfBounds(size_t elSize)
    {
        if (IsOutOfBoundsAfter(elSize))
            throw std::out_of_range("m_currentPos + sizeof >= m_end");
    }
    
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, T> Extract()
    {
        ThrowIfOutOfBounds(sizeof(T));
        
        T data = *(reinterpret_cast<const T*>(m_currentPos));
        m_currentPos += sizeof(T);

        return data;
    }

    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, std::vector<T>> Extract(size_t len)
    {
        size_t lenInBytes = sizeof(T) * len;
        ThrowIfOutOfBounds(lenInBytes);
        
        std::vector<T> out(len);
        std::copy_n(m_currentPos, lenInBytes, reinterpret_cast<uint8_t*>(out.data()));
        m_currentPos += lenInBytes;

        return out;
    }

    template<typename T>
    std::enable_if_t<std::is_same<T, std::string_view>::value, std::string_view> Extract(size_t len)
    {
        ThrowIfOutOfBounds(len);
        std::string_view view(reinterpret_cast<const char *>(m_currentPos), len);
        m_currentPos += len;

        return view;
    }
    
    std::string_view Peek(size_t len)
    {
        ThrowIfOutOfBounds(len);
        std::string_view view(reinterpret_cast<const char *>(m_currentPos), len);
        
        return view;
    }

    constexpr bool IsOutOfBoundsAfter(size_t len)
    {
        return (m_currentPos + len > m_end);
    }

    constexpr void Skip(size_t bytes)
    {
        ThrowIfOutOfBounds(bytes);
        m_currentPos += bytes;
    }

private:
    const uint8_t *m_currentPos;
    const uint8_t *m_end;
};

ARX_NAMESPACE_END

#endif
