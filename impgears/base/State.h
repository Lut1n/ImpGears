#ifndef IMP_STATE_H
#define IMP_STATE_H

#include "impBase.hpp"
#include "EvnContextInterface.h"

#define WIN_W  800
#define WIN_H 600

#define SENSIBILITY 0.002f

IMPGEARS_BEGIN

class State
{
    public:
        State();
        virtual ~State();

        void onEvent(const imp::Event& event);

        static State* getInstance()
        {
                return m_singleton;
        }

        bool key_up, key_down, key_left, key_right, key_shift;

        float xdep, ydep;

    protected:
    private:

        static State* m_singleton;
};

IMPGEARS_END

#endif // IMP_STATE_H
