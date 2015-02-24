#ifndef IMP_STATE_H
#define IMP_STATE_H

#include "impBase.h"
#include "EvnContextInterface.h"

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

        bool key_up, key_down, key_left, key_right, key_shift, key_ctrl;

        float xdep, ydep;

        bool mapUp_down, mapDown_down;
        bool zoomUp_down, zoomDown_down;
        bool save_down;
        Uint32 debugMode;

        void setDebugMode(Uint32 _debugMode) {debugMode = _debugMode;}

    protected:
    private:

        static State* m_singleton;
};

IMPGEARS_END

#endif // IMP_STATE_H
