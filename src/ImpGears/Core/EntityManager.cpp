#include <Core/EntityManager.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
EntityManager* EntityManager::m_instance = IMP_NULL;

//--------------------------------------------------------------
EntityManager::EntityManager()
{
    m_instance = this;
}

//--------------------------------------------------------------
EntityManager::~EntityManager()
{
}

//--------------------------------------------------------------
void EntityManager::addEntity(Entity* entity)
{
    m_entities.push_back(entity);
}

//--------------------------------------------------------------
void EntityManager::removeEntity(Entity* entity)
{
    Uint32 index = 0;
    for(Uint32 i = 0; i<m_entities.size(); ++i)
        if(m_entities[i] == entity) index = i;

    for(Uint32 i = index+1; i<m_entities.size(); ++i)
        m_entities[i-1] = m_entities[i];

    m_entities.resize(m_entities.size()-1);
}

//--------------------------------------------------------------
void EntityManager::updateAll()
{
    for(Uint32 i=0; i<m_entities.size(); ++i)
        m_entities[i]->update();
}

//--------------------------------------------------------------
void EntityManager::onEvent(const imp::Event& event)
{
    for(Uint32 i=0; i<m_entities.size(); ++i)
        m_entities[i]->onEvent(event);
}

IMPGEARS_END
