/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_PEN_H
#define ARX_PEN_H
#include "arxengine/ArxDefines.h"
#include "Color.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Pen
{
public:
    Pen(Color color);
    Color GetColor();
    void SetColor(Color col);
private:
    Color m_color;
};

ARX_NAMESPACE_END

#endif
