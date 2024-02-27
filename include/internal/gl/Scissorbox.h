#ifndef ARX_SCISSORBOX_H
#define ARX_SCISSORBOX_H
#include "ArxDefines.h"
#include "ui/Position.h"
#include "ui/Size.h"
#include <glad/glad.h>
ARX_NAMESPACE_BEGIN

struct ScissorBox
{
    GLint x;
    GLint y;
    GLint width;
    GLint height;
};


//save old scissorbox and set clip area
class ClipGuard
{
public:
    ClipGuard(Position clipboxPos, Size clipboxSize);
    ~ClipGuard();

    const ScissorBox &GetOldScissorBox() const;
    GLboolean WasScissorBoxEnabled() const;

private:
    ScissorBox m_scissorBox;
    GLboolean m_wasEnabled;
};


ARX_NAMESPACE_END

#endif