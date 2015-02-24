#include "SFMLContextInterface.h"

IMPGEARS_BEGIN

SFMLContextInterface::SFMLContextInterface()
{
    //ctor
}

SFMLContextInterface::~SFMLContextInterface()
{
    //dtor
}

Uint32 SFMLContextInterface::createWindow(Uint32 width, Uint32 height)
{
    Uint32 id = m_windows.size();

    sf::RenderWindow* window =
        new sf::RenderWindow(sf::VideoMode(width, height), "SFML Window", sf::Style::Default, sf::ContextSettings(24));

    window->setFramerateLimit(60);
    m_windows.push_back(window);

    return id;
}

void SFMLContextInterface::destroyWindow(Uint32 windowID)
{
    delete m_windows[windowID];
    m_windows[windowID] = IMP_NULL;
}

void SFMLContextInterface::setCursorVisible(Uint32 windowID, bool visible)
{
    m_windows[windowID]->setMouseCursorVisible(visible);
}

bool SFMLContextInterface::isOpen(Uint32 windowID)
{
    return m_windows[windowID]->isOpen();
}

void SFMLContextInterface::setCursorPosition(Uint32 windowID, Uint32 x, Uint32 y)
{
    sf::Mouse::setPosition(sf::Vector2i(x, y), *m_windows[windowID]);
}

void SFMLContextInterface::getEvents(Uint32 windowID)
{
    sf::RenderWindow* window = m_windows[windowID];

    sf::Event event;
    while(window->pollEvent(event))
        pushSFMLEvent(event);
}

void SFMLContextInterface::display(Uint32 windowID)
{
    m_windows[windowID]->display();
}

void SFMLContextInterface::pushSFMLEvent(const sf::Event& event)
{
    switch(event.type)
    {
        case sf::Event::MouseMoved:
        {
            imp::Event::Mouse mouse;
            mouse.button = imp::Event::Mouse_LeftButton;
            mouse.x = event.mouseMove.x;
            mouse.y = event.mouseMove.y;

            pushEvent(imp::Event(imp::Event::Type_MouseMoved, mouse));
            break;
        }
        case sf::Event::MouseButtonPressed:
        {
            imp::Event::Mouse mouse;
            mouse.button = imp::Event::Mouse_LeftButton;
            mouse.x = event.mouseButton.x;
            mouse.y = event.mouseButton.y;

            pushEvent(imp::Event(imp::Event::Type_MousePressed, mouse));
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            imp::Event::Mouse mouse;
            mouse.button = imp::Event::Mouse_LeftButton;
            mouse.x = event.mouseButton.x;
            mouse.y = event.mouseButton.y;

            pushEvent(imp::Event(imp::Event::Type_MouseReleased, mouse));
            break;
        }
        case sf::Event::MouseEntered:
        {
            imp::Event::Mouse mouse;
            mouse.button = imp::Event::Mouse_LeftButton;
            mouse.x = event.mouseMove.x;
            mouse.y = event.mouseMove.y;

            pushEvent(imp::Event(imp::Event::Type_MouseEntered, mouse));
            break;
        }
        case sf::Event::MouseLeft:
        {
            imp::Event::Mouse mouse;
            mouse.button = imp::Event::Mouse_LeftButton;
            mouse.x = event.mouseMove.x;
            mouse.y = event.mouseMove.y;

            pushEvent(imp::Event(imp::Event::Type_MouseLeft, mouse));
            break;
        }
        case sf::Event::KeyPressed:
        {
            imp::Event::Keyboard keyboard;
            keyboard.keyCode = static_cast<imp::Event::Key>(event.key.code); /// same key code
            keyboard.ctrlPressed = event.key.control;
            keyboard.altPressed = event.key.alt;
            keyboard.shiftPressed = event.key.shift;

            pushEvent(imp::Event(imp::Event::Type_KeyPressed, keyboard));
            break;
        }
        case sf::Event::KeyReleased:
        {
            imp::Event::Keyboard keyboard;
            keyboard.keyCode = static_cast<imp::Event::Key>(event.key.code);  /// same key code
            keyboard.ctrlPressed = event.key.control;
            keyboard.altPressed = event.key.alt;
            keyboard.shiftPressed = event.key.shift;

            pushEvent(imp::Event(imp::Event::Type_KeyReleased, keyboard));
            break;
        }
        default:
            // fprintf(stdout, "impError : SFML Event not supported\n");
        break;
    }
}

Uint32 SFMLContextInterface::getWidth(Uint32 windowID) const
{
    return m_windows[windowID]->getSize().x;
}

Uint32 SFMLContextInterface::getHeight(Uint32 windowID) const
{
    return m_windows[windowID]->getSize().y;
}

IMPGEARS_END
