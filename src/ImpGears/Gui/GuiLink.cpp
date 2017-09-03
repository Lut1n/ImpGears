#include <Gui/GuiLink.h>
#include <iostream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiLinkRenderer* GuiLinkRenderer::_singleton = NULL;

//--------------------------------------------------------------
GuiLinkRenderer::GuiLinkRenderer()
{
}

//--------------------------------------------------------------
GuiLinkRenderer::~GuiLinkRenderer()
{
}

//--------------------------------------------------------------
void GuiLinkRenderer::addLink(Anchor* la1, Anchor* la2)
{
	Link lk;
	lk._a1 = la1;
	lk._a2 = la2;
	_lkBuffer.push_back( lk );
	Line2DGeometry* geom = new Line2DGeometry();
	_vBuffer.push_back( geom );
}

//--------------------------------------------------------------
void GuiLinkRenderer::removeLink(Anchor* la)
{
	bool found = false;
	unsigned int count = _lkBuffer.size();
	for(unsigned int i=0; i<count; ++i)
	{
		if(_lkBuffer[i]._a1 == la || _lkBuffer[i]._a2 == la)
		{
			delete _vBuffer[i];
			found = true;
		}
		
		if(found && i+1 < count )
		{
			_lkBuffer[i] = _lkBuffer[i+1];
			_vBuffer[i] = _vBuffer[i+1];
		}
	}
	
	if(found)
	{
		_lkBuffer.resize(count-1);
		_vBuffer.resize(count-1);
	}
}

//--------------------------------------------------------------
void GuiLinkRenderer::updateBuffer()
{
	for(unsigned int i=0; i<_lkBuffer.size(); ++i)
	{
		Line2DGeometry::Point2d p1, p2;
		_lkBuffer[i]._a1->getAnchorPosition(p1._x, p1._y);
		_lkBuffer[i]._a2->getAnchorPosition(p2._x, p2._y);
		_vBuffer[i]->setLine(p1, p2);

		// _vBuffer[i]->update();
	}
}

//--------------------------------------------------------------
void GuiLinkRenderer::draw()
{
	for(unsigned int i=0; i<_lkBuffer.size(); ++i)
	{
		_vBuffer[i]->draw();
	}	
}

//--------------------------------------------------------------
GuiLinkAnchor* GuiLinkAnchor::_currentPressed = NULL;

//--------------------------------------------------------------
GuiLinkAnchor::GuiLinkAnchor(GuiComponent* parent, int id)
	: GuiComponent(0.0,0.0,20.0,20.0)
	, _side(Side_Left)
	, _parentComponent(parent)
	, _id(id)
{
	//--------------------------------------------------------------
	class LinkAnchorAction : public GuiEventHandler
	{
	public:

		LinkAnchorAction(GuiLinkAnchor* anchor, GuiLinkedBox* parent)
			: _anchor(anchor)
			, _parent(parent)
		{
		}
	
		//--------------------------------------------------------------
		bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over)
			{
				//std::cout << "[debug] anchor pressed" << std::endl;
				GuiLinkAnchor::_currentPressed = _anchor;
				return true;
			}
			return false;
		}
		//--------------------------------------------------------------
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			//std::cout << "[debug] mouse released called" << std::endl;
			if(GuiLinkAnchor::_currentPressed)
			{
				//std::cout << "[debug] link no null !" << std::endl;
				if(over)
				{
					
					//std::cout << "[debug] release over !" << std::endl;
					if(_anchor != GuiLinkAnchor::_currentPressed)
					{
						//std::cout << "[debug] anchor released" << std::endl;
						GuiLinkRenderer::instance()->removeLink( _anchor );
						GuiLinkRenderer::instance()->removeLink( GuiLinkAnchor::_currentPressed );
						GuiLinkRenderer::instance()->addLink(_anchor, GuiLinkAnchor::_currentPressed);
						_parent->onLinkChanged(true, _anchor, GuiLinkAnchor::_currentPressed);
						return true;
					}
					else
					{
						//std::cout << "[debug] anchor destroyed" << std::endl;
						GuiLinkRenderer::instance()->removeLink(GuiLinkAnchor::_currentPressed);
						_parent->onLinkChanged(false, GuiLinkAnchor::_currentPressed, IMP_NULL);
					}
					GuiLinkAnchor::_currentPressed = NULL;
				}
				// GuiLinkAnchor::_currentPressed = NULL;
			}
			return false;
		}
		
		protected:
		
		GuiLinkAnchor* _anchor;
		GuiLinkedBox* _parent;
	};
	
	Layout::Parameters bparam;
	bparam.setResizingX(Layout::Resizing_Fill);
	bparam.setResizingY(Layout::Resizing_Fill);
	bparam.setPositionningX(Layout::Positionning_Relative, -0.5);
	bparam.setPositionningY(Layout::Positionning_Relative, -0.5);
	
	_button = new GuiButton();
	_button->setLayoutParameters(bparam);
	
	_button->addEventHandler( new LinkAnchorAction(this, static_cast<GuiLinkedBox*>(parent) ) );
	
	_text = new GuiText();
	_text->setText("in/out");
	compose(_text);
	
	compose(_button);
}

//--------------------------------------------------------------
GuiLinkAnchor::~GuiLinkAnchor()
{
	
}

//--------------------------------------------------------------
void GuiLinkAnchor::setSide(Side side)
{
	_side = side;
	
}

//--------------------------------------------------------------
void GuiLinkAnchor::getAnchorPosition(float& x, float& y)
{
	x = _absPosX;
	y = _absPosY;
}
//--------------------------------------------------------------
void GuiLinkAnchor::setLink(GuiLinkAnchor* other)
{
	_linkedAnchor = other;
}
//--------------------------------------------------------------
void GuiLinkAnchor::unsetLink()
{
	_linkedAnchor  = NULL;
	//GuiLinkRenderer::instance()->removeLink(this);
}

//--------------------------------------------------------------
void GuiLinkAnchor::renderComponent(float parentX, float parentY)
{
	if(_side == Side_Left)
	{
		_text->setPosition(-(20.0+_text->getSizeX()), 0.0);
	}
	else if(_side == Side_Right)
	{
		_text->setPosition(+(20.0), 0.0);
	}
	
	GuiComponent::renderComponent(parentX, parentY);
	
	_absPosX = parentX + getPositionX();
	_absPosY = parentY + getPositionY();
}

//--------------------------------------------------------------
void GuiLinkAnchor::setParent(GuiComponent* parent, int id)
{
	_parentComponent = parent;
	_id = id;
}

//--------------------------------------------------------------
void GuiLinkAnchor::setText(const std::string& text)
{
	_text->setText(text);
}

//--------------------------------------------------------------
GuiLinkedBox::GuiLinkedBox()
	: GuiBox("Node 1")
	, _leftSide(IMP_NULL)
	, _rightSide(IMP_NULL)
{
	_leftSide = new GuiComponent(0.0,0.0,0.0,0.0);
	Layout::Parameters leftParam;
	leftParam.setPositionningX(Layout::Positionning_Relative, -0.1);
	leftParam.setResizingY(Layout::Resizing_Fill);
	_leftSide->setLayoutParameters(leftParam);
	compose(_leftSide);
	
	_rightSide = new GuiComponent(0.0,0.0,0.0,0.0);
	Layout::Parameters rightParam;
	rightParam.setPositionningX(Layout::Positionning_Relative, 1.1);
	rightParam.setResizingY(Layout::Resizing_Fill);
	_rightSide->setLayoutParameters(rightParam);
	compose(_rightSide);
}

//--------------------------------------------------------------
GuiLinkedBox::~GuiLinkedBox()
{
	
}

//--------------------------------------------------------------
void GuiLinkedBox::setupAnchorCount(GuiLinkAnchor::Side side, unsigned int count)
{
	//decompose
	// to implement
	
	// re-compose
	for(unsigned int i=0; i<count; ++i)
	{
		int id = i+1;
		if(side == GuiLinkAnchor::Side_Left)id = -id;
		
		GuiLinkAnchor* anchor = new GuiLinkAnchor(this, id);
		anchor->setSide(side);
		Layout::Parameters param;
		param.setPositionningY(Layout::Positionning_Auto);
		anchor->setLayoutParameters(param);
		
		if(side == GuiLinkAnchor::Side_Left)
		{
			_leftAnchors.push_back(anchor);
			_leftSide->compose(anchor);
		}
		else if(side == GuiLinkAnchor::Side_Right)
		{
			_rightAnchors.push_back(anchor);
			_rightSide->compose(anchor);
		}
	}
}

//--------------------------------------------------------------
void GuiLinkedBox::setAnchorText(GuiLinkAnchor::Side side, int id, const std::string& text)
{
	if(side == GuiLinkAnchor::Side_Left)
	{
		_leftAnchors[id-1]->setText(text);
	}
	else if(side == GuiLinkAnchor::Side_Right)
	{
		_rightAnchors[id-1]->setText(text);
	}
}

//--------------------------------------------------------------
void GuiLinkedBox::onLinkChanged(bool created, GuiLinkAnchor* anchor1, GuiLinkAnchor* anchor2)
{
	if(_linkHandler)
	{
		if(created)
			_linkHandler->onLinkCreate(anchor1->getParent(), anchor1->getID(), anchor2->getParent(), anchor2->getID());
		else
			_linkHandler->onLinkDelete(anchor1->getParent(), anchor1->getID());
	}
}


IMPGEARS_END
