#ifndef IMP_STATE_H
#define IMP_STATE_H

#include "Core/impBase.h"
#include "System/EvnContextInterface.h"

#define SENSIBILITY 0.002f

IMPGEARS_BEGIN

/// \brief Defines a temporary game state. To be modified.
class IMP_API State
{
    public:
        State();
        virtual ~State();

        /// \brief Called when event is received.
        /// \param event - The received event.
        void onEvent(const imp::Event& event);

        /// \brief Gets the instance of the game state.
        /// \return The instance.
        static State* getInstance()
        {
                return m_singleton;
        }

	bool m_pressedKeys[Event::KeyCount];
	bool m_pressedMouseButtons[Event::Mouse_ButtonCount];

	float xMouse, yMouse;
	float xdep, ydep;
		
	Event::Key _lastPressedKey;
	Event::Key _lastReleasedKey;
	char _keyValue;

	void setWindowDim(unsigned int w, unsigned int h)
{
	_windowWidth = w;
	_windowHeight = h;
}

    protected:
    private:

        static State* m_singleton;
		unsigned int _windowWidth;
		unsigned int _windowHeight;
};

IMPGEARS_END

#endif // IMP_STATE_H
