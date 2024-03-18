#ifndef ARX_UIEVENTPROCESSOR_H
#define ARX_UIEVENTPROCESSOR_H
#include "arxengine/ArxDefines.h"
#include "EventProcessor.h"

ARX_NAMESPACE_BEGIN
class ArxWindow;

class ARX_EXPORTS UIEventProcessor final : public EventProcessor
{
protected:
    void CallBeforeProcessing(Event &evt) override; 
};

ARX_NAMESPACE_END

#endif
