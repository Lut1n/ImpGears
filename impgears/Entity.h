#ifndef IMP_ENTITY_H
#define IMP_ENTITY_H

#include "base/impBase.hpp"
#include "EvnContextInterface.h"

IMPGEARS_BEGIN

class Entity
{
    public:
        Entity();
        virtual ~Entity();

        virtual void update() = 0;
        virtual void onEvent(const Event& event) = 0;

    protected:
    private:
};

IMPGEARS_END

#endif // IMP_ENTITY_H
