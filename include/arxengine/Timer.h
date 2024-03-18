/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_TIMER_H
#define ARX_TIMER_H

#include "arxengine/ArxDefines.h"
#include "arxengine/ArxObject.h"
#include "arxengine/evt/Event.h"
#include <chrono>

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS TimerEvent final : public Event 
{
private:
    void HandleEvent() override;
};

using TimerTime = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

class ARX_EXPORTS Timer : public ArxObject
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

private:
    friend class TimerCheckEvent;
    TimerTime m_past;
    TimerTime m_whenStartWasCalled;
    std::chrono::nanoseconds m_interval;
    TimerType m_timerType;
    bool m_isRunning;
};

ARX_NAMESPACE_END

#endif
