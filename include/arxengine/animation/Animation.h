#ifndef ARX_ANIMATION_H
#define ARX_ANIMATION_H
#include "arxengine/ArxObject.h"
#include <chrono>

ARX_NAMESPACE_BEGIN

class AbstractAnimationAction;

class ARX_EXPORTS Animation : public arx::ArxObject
{
friend class AnimationManager;
public:
    Animation(arx::ArxObject *parent = nullptr);
    void AddKeyFrame(size_t keyFrameNum, std::unique_ptr<AbstractAnimationAction> &&action);
    void RemoveKeyFrame(size_t keyFrameNum);
    std::optional<size_t> GetLastFrameNum();
    void Run(bool loop);
    void Stop();
    virtual ~Animation();
private:
    std::chrono::time_point<std::chrono::steady_clock> m_animatorTimePoint;
    std::map<size_t, AbstractAnimationAction*> m_frames;
};

ARX_NAMESPACE_END

#endif 
