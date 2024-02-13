#pragma once
#include "ArxDefines.h"
#include "EventLoop.h"

ARX_NAMESPACE_BEGIN
class ArxWindow;

class UIEventLoop final : public EventLoop
{
protected:
    void CallBeforeLoopIteration() override; //TODO
    void CallBeforeProcessing(Event &evt) override; //TODO
private:
};

ARX_NAMESPACE_END
