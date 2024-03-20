/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_VIEWPORT_H
#define ARX_VIEWPORT_H
#include "arxengine/ArxDefines.h"
#include <glm/mat4x4.hpp>
#include "arxengine/ui/Size.h"
#include "arxengine/ui/Position.h"

ARX_NAMESPACE_BEGIN

struct Viewport
{
    glm::mat4 projectionMatrix;
    SizeF size;
};

static inline Position GetViewportAffectedPosition(const Viewport &viewPort, SizeF windowSize, Position pos)
{
    pos.x *=  viewPort.size.width / windowSize.width;
    pos.y *= viewPort.size.height / windowSize.height;
    return pos;
}

ARX_NAMESPACE_END


#endif
