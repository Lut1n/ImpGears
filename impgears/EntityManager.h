#ifndef IMP_ENTITYMANAGER_H
#define IMP_ENTITYMANAGER_H

#include "base/impBase.hpp"
#include "Entity.h"

#include <vector>

IMPGEARS_BEGIN

class EntityManager
{
    public:
        EntityManager();
        virtual ~EntityManager();

        void addEntity(Entity* entity);
        void removeEntity(Entity* entity);

        void updateAll();
        void onEvent(const imp::Event& event);

        static EntityManager* getInstance(){return m_instance;}


    protected:
    private:

        static EntityManager* m_instance;

        std::vector<Entity*> m_entities;
};

IMPGEARS_END

#endif // IMP_ENTITYMANAGER_H
