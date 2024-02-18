#pragma once
#include "ArxDefines.h"
#include "EventProcessor.h"

ARX_NAMESPACE_BEGIN
class ArxWindow;

class UIEventProcessor final : public EventProcessor
{
protected:
    void CallBeforeProcessing(Event &evt) override; 
};

ARX_NAMESPACE_END
