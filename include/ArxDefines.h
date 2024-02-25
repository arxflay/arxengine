#ifndef ARX_ARXDEFINES_H
#define ARX_ARXDEFINES_H
#include <cstdint>

#define ARX_NAMESPACE arx

#define ARX_NAMESPACE_BEGIN namespace ARX_NAMESPACE {
#define ARX_NAMESPACE_END }
#define ARX_NAMESPACE_USE using namespace ARX_NAMESPACE

#ifdef WIN32
#define ARX_IMPORTS __declspec(dllimport)
#define ARX_EXPORTS __declspec(dllexport)
#else
#define ARX_IMPORTS
#define ARX_EXPORTS
#endif

ARX_NAMESPACE_BEGIN
namespace defaults
{
    static inline constexpr int GL_VERSION_MAJOR = 3;
    static inline constexpr int GL_VERSION_MINOR = 3;
    static inline constexpr int IGNORE = -1;
}
ARX_NAMESPACE_END

#endif
