#ifndef ARX_ABSTRACTANIMATIONACTION_H
#define ARX_ABSTRACTANIMATIONACTION_H
#include <memory>
#include "arxengine/ArxDefines.h"

ARX_NAMESPACE_BEGIN
class ARX_EXPORTS AbstractAnimationAction
{
public:
    AbstractAnimationAction(std::unique_ptr<AbstractAnimationAction> &&next = nullptr);
    void PerformAction();
    virtual ~AbstractAnimationAction() = default;
private:
    virtual void PerformActionImpl() = 0;
    std::unique_ptr<AbstractAnimationAction> m_next;
};
ARX_NAMESPACE_END

#endif
