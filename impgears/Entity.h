#ifndef IMP_ENTITY_H
#define IMP_ENTITY_H

#include "base/impBase.hpp"
#include "EvnContextInterface.h"

IMPGEARS_BEGIN

/// \brief Defines a game entity. Entities are updated by the entity manager.
class Entity
{
    public:
        Entity();
        virtual ~Entity();

        /// \brief Updates the entity. Called by EntityManager.
        virtual void update() = 0;

        /// \brief Called when event is received.
        /// \param event - the event.
        virtual void onEvent(const Event& event) = 0;

    protected:
    private:
};

IMPGEARS_END

#endif // IMP_ENTITY_H
