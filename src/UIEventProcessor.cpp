#include "UIEventProcessor.h"
#include "ArxWindow.h"

ARX_NAMESPACE_BEGIN

void UIEventProcessor::CallBeforeProcessing(Event &evt) 
{
    UIObject *obj = dynamic_cast<UIObject*>(evt.GetSender());
    if (obj)
    {
        if (obj->GetOwnerWindow())
            obj->GetOwnerWindow()->SetAsCurrentContext();
        else
            dynamic_cast<ArxWindow*>(obj)->SetAsCurrentContext();
    }
}

ARX_NAMESPACE_END
