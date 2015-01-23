#ifndef IMP_ENTITYMANAGER_H
#define IMP_ENTITYMANAGER_H

#include "base/impBase.hpp"
#include "Entity.h"

#include <vector>

IMPGEARS_BEGIN

/// \brief Defines the game entity manager. Updates and send event to entities.
class EntityManager
{
    public:
        EntityManager();
        virtual ~EntityManager();

        /// \brief Adds an entity to the entity manager.
        /// \param entity - the entity to add.
        void addEntity(Entity* entity);

        /// \brief Removes an entity from the manager.
        /// \param entity - the entity to remove.
        void removeEntity(Entity* entity);

        /// \brief Updates all the entities.
        void updateAll();

        /// \brief Sends an event to all the entities.
        void onEvent(const imp::Event& event);

        /// \brief Gets the instance of the manager.
        /// \return The instance.
        static EntityManager* getInstance(){return m_instance;}


    protected:
    private:

        static EntityManager* m_instance;

        std::vector<Entity*> m_entities;
};

IMPGEARS_END

#endif // IMP_ENTITYMANAGER_H
