/*
 * @author Alexej Fedorenko
 */
#ifndef ARX_EMPTYANIMATIONACTION_H
#define ARX_EMPTYANIMATIONACTION_H
#include "arxengine/animation/AbstractAnimationAction.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS EmptyAnimationAction : public AbstractAnimationAction
{ 
public:
    EmptyAnimationAction(std::unique_ptr<AbstractAnimationAction> &&next = nullptr)
        : AbstractAnimationAction(std::move(next))
    {
    }

    void PerformActionImpl() override { }
};

ARX_NAMESPACE_END

#endif
