#ifndef ARX_TIMER_H
#define ARX_TIMER_H

#include "ArxDefines.h"
#include "ArxObject.h"
#include "evt/Event.h"
#include <chrono>

ARX_NAMESPACE_BEGIN

class TimerEvent final : public Event 
{
private:
    void HandleEvent() override;
};

using TimerTime = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

class Timer : public ArxObject
{
public:
    Timer(ArxObject *parent);

    enum class TimerType
    {
        CONTINUOUS,
        SINGLE_FIRE
    };

    void Stop();
    void Start(TimerType type);
    void ChangeType(TimerType type);

    bool IsRunning() const;
    TimerType GetType() const;

    template<typename T>
    void SetInterval(T interval)
    {
        m_interval = std::chrono::duration_cast<std::chrono::nanoseconds>(interval);

        if (IsRunning())
        {
            Stop();
            Start(m_timerType);
        }
    }
    Timer *Clone() override;

private:
    Timer *AllocClone() override;
    friend class TimerCheckEvent;
    TimerTime m_past;
    TimerTime m_whenStartWasCalled;
    std::chrono::nanoseconds m_interval;
    TimerType m_timerType;
    bool m_isRunning;
};

ARX_NAMESPACE_END

#endif
