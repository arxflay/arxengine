#ifndef ARX_PEN_H
#define ARX_PEN_H
#include "ArxDefines.h"
#include "Color.h"

ARX_NAMESPACE_BEGIN

class Pen
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
