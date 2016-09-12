#ifndef EVNCONTEXTINTERFACE_H
#define EVNCONTEXTINTERFACE_H

#include "Core/impBase.h"

#include <vector>

IMPGEARS_BEGIN

class IMP_API Event
{
  public:

    enum Type
    {
        Type_MouseMoved,
        Type_MouseEntered,
        Type_MouseLeft,
        Type_MousePressed,
        Type_MouseReleased,
        Type_KeyPressed,
        Type_KeyReleased,
        Type_WindowClose
    };

    enum MouseButton
    {
        Mouse_LeftButton = 0,
        Mouse_RightButton,

	Mouse_ButtonCount // Get the enum size
    };

    struct Mouse
    {
        MouseButton button;
        Uint32 x;
        Uint32 y;
    };

    enum Key
    {
        Unknown = -1,
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        Escape,
        LControl,
        LShift,
        LAlt,
        LSystem,
        RControl,
        RShift,
        RAlt,
        RSystem,
        Menu,
        LBracket,
        RBracket,
        SemiColon,
        Comma,
        Period,
        Quote,
        Slash,
        BackSlash,
        Tilde,
        Equal,
        Dash,
        Space,
        Return,
        BackSpace,
        Tab,
        PageUp,
        PageDown,
        End,
        Home,
        Insert,
        Delete,
        Add,
        Subtract,
        Multiply,
        Divide,
        Left,
        Right,
        Up,
        Down,
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        Pause,

	KeyCount // Get the enum size
    };

    struct Keyboard
    {
        Key keyCode;
        bool ctrlPressed;
        bool altPressed;
        bool shiftPressed;
    };

    union Value
    {
        Mouse mouse;
        Keyboard keyboard;
    };

    Event(){}
    Event(const Event& other)
    {
        m_type = other.m_type;
        m_value = other.m_value;
    }

    Event(Type type, Value value)
    {
        m_type = type;
        m_value = value;
    }


    Event(Type type, Keyboard value)
    {
        m_type = type;
        m_value.keyboard = value;
    }


    Event(Type type, Mouse value)
    {
        m_type = type;
        m_value.mouse = value;
    }

    virtual ~Event(){}

    const Event& operator=(const Event& other)
    {
        m_type = other.m_type;
        m_value = other.m_value;

        return *this;
    }

    Type getType() const{return m_type;}
    void setType(Type type){m_type = type;}

    Mouse getMouse() const{return m_value.mouse;}
    void setMouse(Mouse mouse){m_value.mouse = mouse;}

    Keyboard getKeyboard() const{return m_value.keyboard;}
    void setKeyboard(Keyboard keyboard){m_value.keyboard = keyboard;}

  protected:

  private:

    Type m_type;
    Value m_value;
};

class IMP_API EvnContextInterface
{
    public:
        virtual ~EvnContextInterface();

        static EvnContextInterface* getInstance();

        virtual Uint32 createWindow(Uint32 width, Uint32 height) = 0;
        virtual void destroyWindow(Uint32 windowID) = 0;

		virtual void resizeWindow(Uint32 windowID, Uint32 width, Uint32 height) = 0;
		virtual void setWindowTitle(Uint32 windowID, const char* title) = 0;

        virtual void setCursorVisible(Uint32 windowID, bool visible) = 0;
        virtual bool isOpen(Uint32 windowID) = 0;

        virtual void getEvents(Uint32 windowID) = 0;

        virtual void setCursorPosition(Uint32 windowID, Uint32 x, Uint32 y) = 0;

        virtual void display(Uint32 windowID) = 0;

        bool nextEvent(Event& event);
        void pushEvent(const Event& event);

        virtual Uint32 getWidth(Uint32 windowID) const = 0;
        virtual Uint32 getHeight(Uint32 windowID) const = 0;



    protected:

        EvnContextInterface();

    private:

        static EvnContextInterface* m_singleton;

        std::vector<Event> m_events;
};

IMPGEARS_END

#endif // EVNCONTEXTINTERFACE_H
