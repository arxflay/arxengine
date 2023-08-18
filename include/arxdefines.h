#pragma once
#include <cstdint>

#define ARX_NAMESPACE_BEGIN namespace arx {
#define ARX_NAMESPACE_END }
#define ARX_NAMESPACE_USE using namespace arx


#ifdef WIN32
#define ARX_IMPORTS __declspec(dllimport)
#define ARX_EXPORTS __declspec(dllexport)
#else
#define ARX_IMPORTS
#define ARX_EXPORTS
#endif
