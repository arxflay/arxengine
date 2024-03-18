#include "arxengine/ui/KeyEvent.h"

ARX_NAMESPACE_BEGIN

void KeyEvent::SetKey(Key key)
{
    m_key = key;
}

KeyEvent::Key KeyEvent::GetKey() const
{
    return m_key;
}

int KeyEvent::GetKeyChar() const
{
    return static_cast<int>(m_key);
}


ARX_NAMESPACE_END
