/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_PEN_H
#define ARX_PEN_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/Color.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Pen
{
public:
    Pen(const Color &color);
    const Color &GetColor() const;
    void SetColor(const Color &col);
private:
    Color m_color;
};

ARX_NAMESPACE_END

#endif
