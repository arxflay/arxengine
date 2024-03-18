#include "arxengine/internal/gl/ClippingArea.h"
#include <glad/glad.h>
ARX_NAMESPACE_BEGIN

ClippingArea::ClippingArea()
    : m_clipBox(ClipBox{0, 0, 0, 0})
{
}

/*static*/ void ClippingArea::EnableClipping(bool useClipping)
{
    if (useClipping)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

}

/*static*/ bool ClippingArea::IsClippingEnabled()
{
    return !!glIsEnabled(GL_SCISSOR_TEST);
}

void ClippingArea::SetClipBox(const ClipBox &clippingBox) 
{ 
    m_clipBox = clippingBox;
    m_clipBox.x = std::max(m_clipBox.x, 0);
    m_clipBox.y = std::max(m_clipBox.y, 0);
    m_clipBox.width = std::max(m_clipBox.width, 0);
    m_clipBox.height = std::max(m_clipBox.height, 0);
}

void ClippingArea::UseThisClippingArea() 
{
    if (!IsClippingEnabled())
        EnableClipping(true);
    
    glScissor(m_clipBox.x, m_clipBox.y, m_clipBox.width, m_clipBox.height);
}

/*static*/ ClippingArea ClippingArea::GetLastlyUsedClippingArea()
{
    ClippingArea::ClipBox clipBox;
    glGetIntegerv(GL_SCISSOR_BOX, reinterpret_cast<GLint*>(&clipBox));
    ClippingArea area;
    area.SetClipBox(clipBox);
    return area;
}

const ClippingArea::ClipBox &ClippingArea::GetClipBox() const { return m_clipBox; }


OldClippingAreaGuard::OldClippingAreaGuard()
{
    m_wasEnabled = ClippingArea::IsClippingEnabled();
    if (m_wasEnabled)
        m_clippingArea = ClippingArea::GetLastlyUsedClippingArea();
}

OldClippingAreaGuard::~OldClippingAreaGuard()
{
    ClippingArea::EnableClipping(m_wasEnabled);
    if (m_wasEnabled)
        m_clippingArea.UseThisClippingArea();
}

const ClippingArea &OldClippingAreaGuard::GetOldClippingArea()
{
    return m_clippingArea;
}

bool OldClippingAreaGuard::WasClippingEnabled() const
{
    return m_wasEnabled;
}

ARX_NAMESPACE_END
