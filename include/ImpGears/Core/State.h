#ifndef IMP_STATE_H
#define IMP_STATE_H

#include "Core/impBase.h"


#define SENSIBILITY 0.002f

IMPGEARS_BEGIN

/// \brief Defines a temporary game state. To be modified.
class IMP_API State
{
    public:
        State();
        virtual ~State();

        /// \brief Gets the instance of the game state.
        /// \return The instance.
        static State* getInstance()
        {
                return m_singleton;
        }
		
		enum ActionState
		{
			ActionState_False = 0,
			ActionState_Pressed,
			ActionState_True,
			ActionState_Released
		};

	float xMouse, yMouse;
	float xdep, ydep;
	
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
		bool _mouseOverWindows;
};

IMPGEARS_END

#endif // IMP_STATE_H
