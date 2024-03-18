#ifndef ARX_SCISSORBOX_H
#define ARX_SCISSORBOX_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/Position.h"
#include "arxengine/ui/Size.h"
#include <optional>
ARX_NAMESPACE_BEGIN

class ClippingArea final
{
public:
    struct ClipBox
    {
        int x;
        int y;
        int width;
        int height;
    };

    ClippingArea();
    
    static void EnableClipping(bool useClipping = true);
    static bool IsClippingEnabled();
    static ClippingArea GetLastlyUsedClippingArea();

    //sets clipbox
    //if values are below zero, clipbox will automaticaly set values to 0
    void SetClipBox(const ClipBox &clipBox);

    //enables clipping if not enabled
    void UseThisClippingArea() ;
    const ClipBox &GetClipBox() const;
private:
    ClipBox m_clipBox;
};

//reverts old clipping settings
class OldClippingAreaGuard
{
public:
    OldClippingAreaGuard();
    ~OldClippingAreaGuard();

    const ClippingArea &GetOldClippingArea();
    bool WasClippingEnabled() const;

private:
    ClippingArea m_clippingArea;
    bool m_wasEnabled;
};


ARX_NAMESPACE_END

#endif
