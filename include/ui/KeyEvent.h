#ifndef ARX_KEYEVENT_H
#define ARX_KEYEVENT_H
#include "ArxDefines.h"
#include "evt/Event.h"

ARX_NAMESPACE_BEGIN

/*KeyEventsBegim*/

/*because of time shortage keyevents are handled only by window*/

class KeyEvent : public Event
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

class KeyDownEvent final : public KeyEvent
{
private:
    void HandleEvent() override {}
};

class KeyHoldEvent final : public KeyEvent
{
private:
    void HandleEvent() override {}
};

class KeyUpEvent final : public KeyEvent
{
private:
    void HandleEvent() override {}
};
/*KeyEventsEnd*/

ARX_NAMESPACE_END


#endif
