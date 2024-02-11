#include "Event.h"

ARX_NAMESPACE_BEGIN

Event::Event()
    : Event(nullptr)
{
}

Event::Event(ArxObject *sender)
    : m_skipped(false)
    , m_sender(sender)
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

ARX_NAMESPACE_END
