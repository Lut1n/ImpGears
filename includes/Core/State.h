#ifndef IMP_STATE_H
#define IMP_STATE_H

#include "Core/impBase.h"
#include "System/EvnContextInterface.h"

#define WIN_W  800
#define WIN_H 600

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

	float xdep, ydep;

    protected:
    private:

        static State* m_singleton;
};

IMPGEARS_END

#endif // IMP_STATE_H
