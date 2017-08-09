#include <Utils/EvnContextInterface.h>
#include <iostream>
#include <cstdio>

IMPGEARS_BEGIN

EvnContextInterface* EvnContextInterface::m_singleton = IMP_NULL;

EvnContextInterface::EvnContextInterface()
{
    if(m_singleton != IMP_NULL)
        delete m_singleton;

    m_singleton = this;
}

EvnContextInterface::~EvnContextInterface()
{
    m_singleton = IMP_NULL;
}

EvnContextInterface* EvnContextInterface::getInstance()
{
    if(m_singleton == IMP_NULL)
        fprintf(stderr, "impError : No instance of Context\n");

    return m_singleton;
}

bool EvnContextInterface::nextEvent(Event& event)
{
    if(m_events.size() == 0)
        return false;

    event = m_events.back();
    m_events.pop_back();

    return true;
}

void EvnContextInterface::pushEvent(const Event& event)
{
    m_events.insert(m_events.begin(), event);
}

IMPGEARS_END
