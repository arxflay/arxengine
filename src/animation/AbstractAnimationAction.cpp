#include "arxengine/ui/AbstractAnimationAction.h"

ARX_NAMESPACE_BEGIN


AbstractAnimationAction::AbstractAnimationAction(std::unique_ptr<AbstractAnimationAction> &&next)
    : m_next(std::move(next))
{
}

void AbstractAnimationAction::PerformAction()
{
    PerformActionImpl();
    if (m_next)
        m_next->PerformActionImpl();
}


ARX_NAMESPACE_END


