#include "arxengine/ui/Animation.h"
#include "arxengine/ui/AbstractAnimationAction.h"
#include <thread>

ARX_NAMESPACE_BEGIN

class AnimationPerformEvent : public arx::Event
{
public:
    AnimationPerformEvent(std::map<size_t, std::unique_ptr<AbstractAnimationAction>> &frames, const std::chrono::time_point<std::chrono::steady_clock> &animatorTimePoint, bool loop)
        : m_currentFrame(0), m_frames(frames), m_timePoint(animatorTimePoint), m_animatorTimePoint(animatorTimePoint), m_loop(loop)
    {
    }
private:
    void HandleEvent()
    {
        if (m_timePoint != m_animatorTimePoint)
            return;

        if (m_currentFrame > GetLastFrameNum())
        {
            if (!m_loop)
                return;

            m_currentFrame = 0;
        }

        auto frameIt = m_frames.find(m_currentFrame);
        if (frameIt != m_frames.end())
            m_frames.at(m_currentFrame)->PerformAction();
        m_currentFrame++;
        ScheduleAfterProcessing(true);
    }

    size_t GetLastFrameNum()
    {
        return m_frames.rbegin()->first;
    }
private:
    size_t m_currentFrame;
    std::map<size_t, std::unique_ptr<AbstractAnimationAction>> &m_frames;
    std::chrono::time_point<std::chrono::steady_clock> m_timePoint;
    const std::chrono::time_point<std::chrono::steady_clock> &m_animatorTimePoint;
    bool m_loop;
};

Animation::Animation(arx::ArxObject *parent)
    : arx::ArxObject(parent)
{
}

void Animation::AddKeyFrame(size_t keyFrameNum, std::unique_ptr<AbstractAnimationAction> &&action)
{
    auto it = m_frames.find(keyFrameNum);
    if (it != m_frames.end())
        it->second = std::move(action);
    else
        m_frames.emplace(keyFrameNum, std::move(action));
}

void Animation::RemoveKeyFrame(size_t keyFrameNum)
{
    m_frames.erase(keyFrameNum);
}

void Animation::Run(bool loop)
{
    m_animatorTimePoint = std::chrono::steady_clock::now();
    GetEventManager().QueueEvent<AnimationPerformEvent>(std::make_unique<AnimationPerformEvent>(m_frames, m_animatorTimePoint, loop));
}
void Animation::Stop()
{
    m_animatorTimePoint = decltype(m_animatorTimePoint)(std::chrono::nanoseconds(0));
    std::this_thread::sleep_for(std::chrono::nanoseconds(0));
}

std::optional<size_t> Animation::GetLastFrameNum()
{
    return m_frames.empty() ? std::nullopt : std::make_optional(m_frames.rbegin()->first);
}

ARX_NAMESPACE_END
