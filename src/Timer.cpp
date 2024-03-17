#include "Timer.h"
ARX_NAMESPACE_BEGIN



void TimerEvent::HandleEvent()
{

}

class TimerCheckEvent final : public Event
{
public:
    void SetWhenStartWasCalled(TimerTime time)
    {
        m_whenStartWasCalled = time;
    }

private:
    void HandleEvent()
    {
        Timer *timer = static_cast<Timer*>(GetSender());
        if (!timer->IsRunning() || timer->m_whenStartWasCalled != m_whenStartWasCalled)
            return;
        
        auto timeNow = std::chrono::steady_clock::now();
        
        if(timer->m_past + timer->m_interval <= std::chrono::steady_clock::now())
        {
            timer->m_past = timeNow;
            TimerEvent evt;
            timer->GetEventManager().ProcessEvent<TimerEvent>(evt);

            if (timer->GetType() == Timer::TimerType::SINGLE_FIRE)
                timer->Stop();
        }

        if (timer->IsRunning())
            ScheduleAfterProcessing(true);
    }

    TimerTime m_whenStartWasCalled;
};

Timer::Timer(ArxObject *parent)
    : ArxObject(parent)
    , m_past(std::chrono::seconds(0))
    , m_whenStartWasCalled(std::chrono::seconds(0))
    , m_isRunning(false)
{
}

void Timer::Stop()
{
    m_isRunning = false;
}

void Timer::Start(TimerType type)
{
    if (!m_isRunning)
    {
        m_past = std::chrono::steady_clock::now();
        m_whenStartWasCalled = std::chrono::steady_clock::now();
        m_isRunning = true;
        m_timerType = type;
        std::unique_ptr<TimerCheckEvent> evt = std::make_unique<TimerCheckEvent>();
        evt->SetWhenStartWasCalled(m_whenStartWasCalled);
        GetEventManager().ScheduleEvent<TimerCheckEvent>(std::move(evt));
    }
}

void Timer::ChangeType(TimerType type)
{
    m_timerType = type;
}

bool Timer::IsRunning() const
{
    return m_isRunning;
}

Timer::TimerType Timer::GetType() const
{
    return m_timerType;
}

ARX_NAMESPACE_END
