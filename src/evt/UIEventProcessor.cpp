#include "evt/UIEventProcessor.h"
#include "ui/ArxWindow.h"

ARX_NAMESPACE_BEGIN

void UIEventProcessor::CallBeforeProcessing(Event &evt) 
{
    UIControl *obj = dynamic_cast<UIControl*>(evt.GetSender());
    if (obj)
        obj->GetWindow()->SetAsCurrentContext(); 
}

ARX_NAMESPACE_END
