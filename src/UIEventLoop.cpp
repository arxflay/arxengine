#include "UIEventLoop.h"
#include <GLFW/glfw3.h>
#include "ArxWindow.h"

ARX_NAMESPACE_BEGIN

void UIEventLoop::CallBeforeLoopIteration() //TODO
{
    glfwPollEvents();
}

void UIEventLoop::CallBeforeProcessing(Event &evt) //TODO
{
    UIObject *obj = dynamic_cast<UIObject*>(evt.GetSender());
    if (obj)
    {
        if (obj->GetOwnerWindow())
            dynamic_cast<ArxWindow*>(obj)->SetAsCurrentContext();
        else
            obj->GetOwnerWindow()->SetAsCurrentContext();
    }
}

ARX_NAMESPACE_END
