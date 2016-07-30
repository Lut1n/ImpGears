#ifndef SFMLCONTEXTINTERFACE_H
#define SFMLCONTEXTINTERFACE_H

#include "System/EvnContextInterface.h"

#include <vector>

#include <SFML/Graphics.hpp>

IMPGEARS_BEGIN

class IMP_API SFMLContextInterface : public EvnContextInterface
{
    public:
        SFMLContextInterface();
        virtual ~SFMLContextInterface();

        virtual Uint32 createWindow(Uint32 width, Uint32 height);
        virtual void destroyWindow(Uint32 windowID);

        virtual void setCursorVisible(Uint32 windowID, bool visible);
        virtual bool isOpen(Uint32 windowID);

        virtual void setCursorPosition(Uint32 windowID, Uint32 x, Uint32 y);

        virtual void display(Uint32 windowID);

        virtual void getEvents(Uint32 windowID);
        void pushSFMLEvent(const sf::Event& event);

        virtual Uint32 getWidth(Uint32 windowID) const;
        virtual Uint32 getHeight(Uint32 windowID) const;

    protected:
    private:

        std::vector<sf::RenderWindow*> m_windows;
};

IMPGEARS_END

#endif // SFMLCONTEXTINTERFACE_H
