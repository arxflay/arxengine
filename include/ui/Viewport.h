#ifndef ARX_VIEWPORT_H
#define ARX_VIEWPORT_H
#include "ArxDefines.h"
#include <glm/mat4x4.hpp>
#include "Size.h"

ARX_NAMESPACE_BEGIN

struct Viewport
{
    glm::mat4 projectionMatrix;
    arx::Size size;
};


ARX_NAMESPACE_END


#endif
