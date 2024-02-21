#include "Event.h"

ARX_NAMESPACE_BEGIN


Event::Event()
    : m_skipped(false)
    , m_scheduleAfterProcessing(false)
    , m_sender(nullptr)
{
}

void Event::Skip(bool skip)
{
    m_skipped = skip;
}

bool Event::WasSkipCalled()
{
    return m_skipped;
}

ArxObject *Event::GetSender()
{
    return m_sender;
}

void Event::SetSender(ArxObject *sender)
{
    m_sender = sender;
}

void Event::ScheduleAfterProcessing(bool reschedule)
{
    m_scheduleAfterProcessing = reschedule;
}

bool Event::IsScheduledAfterProcessing() const //TODO
{
    return m_scheduleAfterProcessing;
}

ARX_NAMESPACE_END
