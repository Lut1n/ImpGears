#ifndef IMP_GUI_COMPONENT_H
#define IMP_GUI_COMPONENT_H

#include "Core/impBase.h"
#include "Core/State.h"
#include <Utils/EvnContextInterface.h>

#include <SceneGraph/SceneNode.h>
#include <SceneGraph/Shader.h>

#include <Core/Rectangle.h>

#include <Gui/ComponentGeometry.h>
#include <Gui/Layout.h>
#include <Gui/GuiEventHandler.h>

IMPGEARS_BEGIN

class IMP_API GuiComponent
	: public SceneNode
	, public GuiEventSource
{
	public:

	struct EventState
	{
		EventState()
			: _pressed(false)
			, _mouseOver(false)
			, _button(0)
			, _mouseX(0.0)
			, _mouseY(0.0)
		{}

		bool	_pressed;
		bool	_mouseOver;
		int		_button;
		float	_mouseX;
		float	_mouseY;
	};

	GuiComponent(float x, float y, float w, float h);
	
	virtual ~GuiComponent();

	
	virtual void setSize(float w, float h);

	void setPosition(float x, float y);
	
	void setAbsPosition(float x, float y);
	void getAbsPosition(float& x, float& y);
	
	float getPositionX() const;
	float getPositionY() const;
	
	virtual float getSizeX() const;
	virtual float getSizeY() const;
	
	void setClippingRect(const Rectangle& clippingRect);
	const Rectangle& getClippingRect() const;
	
	void setVisible(bool visible) {_visible=visible;}
	bool isVisible() const {return _visible;}

	void setLayoutParameters(const Layout::Parameters& param) {_param = param;}
	const Layout::Parameters& getLayoutParameters() const {return _param;}
	
	void addEventHandler(GuiEventHandler* handler);
	void removeEventHandler(GuiEventHandler* handler);

	virtual void render(imp::Uint32 passID);
	virtual void renderComponent(imp::Uint32 passID, float parentX = 0.0, float parentY = 0.0);
	virtual bool event(imp::State* state, float x = 0.0, float y = 0.0);
	virtual void computeBounds(Rectangle& bounds) const;

	void compose(GuiComponent* comp);
	void decompose(GuiComponent** comp);

	protected:

	EventState _previousEventState;
	std::vector<GuiEventHandler*> _eventHandlers;

	std::vector<GuiComponent*> _components;

	Layout _layout;
	Layout::Parameters _param;
	ComponentGeometry _quad;
	bool _visible;

	float _positionX;
	float _positionY;
	float _absolutePositionX;
	float _absolutePositionY;

	public:

	static Shader* _guiComponentShader;

};


IMPGEARS_END

#endif // IMP_GUI_COMPONENT_H
