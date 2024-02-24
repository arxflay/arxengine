#include "internal/gl/Scissorbox.h"
ARX_NAMESPACE_BEGIN

ClipGuard::ClipGuard(Position clipboxPos, Size clipboxSize)
{
    m_wasEnabled = glIsEnabled(GL_SCISSOR_TEST);
    if (m_wasEnabled)
        glGetIntegerv(GL_SCISSOR_BOX, reinterpret_cast<GLint*>(&m_scissorBox));
    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(clipboxPos.x), static_cast<GLint>(clipboxPos.y), static_cast<GLint>(clipboxSize.width), static_cast<GLint>(clipboxSize.height));
}

ClipGuard::~ClipGuard()
{
    if (!m_wasEnabled)
        glDisable(GL_SCISSOR_TEST);
    else
        glScissor(m_scissorBox.x, m_scissorBox.y, m_scissorBox.width, m_scissorBox.height);
}

const ScissorBox &ClipGuard::GetOldScissorBox() const
{
    return m_scissorBox;
}

GLboolean ClipGuard::WasScissorBoxEnabled() const
{
    return m_wasEnabled;
}

ARX_NAMESPACE_END
