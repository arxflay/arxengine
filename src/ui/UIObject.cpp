#include "arxengine/ui/UIObject.h"
#include "arxengine/ui/UIControl.h"
#include "arxengine/ui/ArxWindow.h"
#include "arxengine/ArxException.h"

ARX_NAMESPACE_BEGIN

//UIControl parent can't be nullptr, otherwise exception will be thrown
UIObject::UIObject(UIControl *parent)
{
    if (!parent)
        throw ArxException(ArxException::ErrorCode::GenericError, "Failed to set parent for UIObject, parent is null");

    ArxObject::Reparent(parent);
}

//Reparenting is allowed only if OwnerWindow is not changed otherwise expection will be thrown
void UIObject::Reparent(ArxObject *parent)
{
    UIControl *uiControl = dynamic_cast<UIControl*>(parent);
    if (!uiControl)
        throw ArxException(ArxException::ErrorCode::GenericError, "Failed to set parent for UIObject, parent is not an UIControl");

    if (GetWindow() != uiControl->GetOwnerWindow())
        throw ArxException(ArxException::ErrorCode::GenericError, "Can't change owner window in UIObject");

    ArxObject::Reparent(parent);
}

UIControl *UIObject::GetOwnerUIControl()
{
    return static_cast<UIControl*>(GetParent()); //dynamic cast is not required
}

ArxWindow *UIObject::GetWindow()
{
    return GetOwnerUIControl()->GetWindow();
}

const UIControl *UIObject::GetOwnerUIControl() const
{
    return static_cast<const UIControl*>(GetParent()); //dynamic cast is not required
}

const ArxWindow *UIObject::GetWindow() const
{
    return GetOwnerUIControl()->GetWindow();
}



ARX_NAMESPACE_END
