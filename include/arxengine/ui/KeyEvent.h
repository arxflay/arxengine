/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_KEYEVENT_H
#define ARX_KEYEVENT_H
#include "arxengine/ArxDefines.h"
#include "arxengine/evt/Event.h"
#include <set>

ARX_NAMESPACE_BEGIN

/*because of time shortage keyevents are handled only by window*/

class ARX_EXPORTS KeyEvent : public Event
{
public:

    /*LIMITED RANGE OF KEYS FROM ANSI LAYOUT*/
    enum class Key
    {
        Zero = 48, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
        A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        SquareBracketOpen = 91, BackwardSlash,  SquareBracketClose,
        Space = 32, CapsLk = 280, 
        Esc = 256, Enter, Tab, Backspace,
        Shift = 340, Ctrl, Alt, Super,
        RightShift = 344, RightSuper, RightSuperAlt,
        ArrowLeft = 262, ArrowRight, ArrowDown, ArrowUp,
        PageUp = 266, PageDown,
        Delete = 261,
        Semicolon = 59, Quotes = 39,
        Less = 44, Greater = 46, Slash,
        Minus = 45, Equals = 61,
    };

    void SetKey(Key key);
    Key GetKey() const;
    int GetKeyChar() const;
private:
    void HandleEvent() override = 0;
    Key m_key;
};

class ARX_EXPORTS KeyDownEvent final : public KeyEvent
{
private:
    void HandleEvent() override {}
};

//only last held key will be processed
class ARX_EXPORTS KeyRepeatEvent final : public KeyEvent
{
private:
    void HandleEvent() override {}
};

class ARX_EXPORTS KeyUpEvent final : public KeyEvent
{
private:
    void HandleEvent() override {}
};

using KeySet = std::set<KeyEvent::Key>;

class ARX_EXPORTS KeyHoldEvent final : public Event 
{
public:
    KeyHoldEvent(const KeySet &keys);

    const KeySet &GetHeldKeys() const;
private:
    const KeySet &m_keys;
    void HandleEvent() override {}
};


ARX_NAMESPACE_END


#endif
