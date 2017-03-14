#ifndef IMP_GUI_LINK_H
#define IMP_GUI_LINK_H

#include "Core/impBase.h"

#include <vector>
#include <Gui/GuiBox.h>
#include <Gui/GuiButton.h>
#include <Gui/GuiText.h>
#include <Gui/Line2DGeometry.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiLinkRenderer
{
	
	public:
	
	//--------------------------------------------------------------
	class Anchor
	{
		public:
		
		Anchor()
		{}
		virtual ~Anchor()
		{}
		
		virtual void getAnchorPosition(float& x, float& y) = 0;
	};
	
	//--------------------------------------------------------------
	struct Link
	{	
		Anchor* _a1;
		Anchor* _a2;
	};
	
	//--------------------------------------------------------------
	typedef std::vector<Link> LinkBuffer;
	typedef std::vector<Line2DGeometry*> LineGeomBuffer;
	
	GuiLinkRenderer();
	virtual ~GuiLinkRenderer();
	
	void addLink(Anchor* la1, Anchor* la2);
	void removeLink(Anchor* la);
	
	void updateBuffer();
	void draw();
	
	static GuiLinkRenderer* instance() { if(_singleton == NULL)_singleton = new GuiLinkRenderer(); return _singleton; }

	protected:

	LineGeomBuffer _vBuffer;
	LinkBuffer _lkBuffer;
	
	static GuiLinkRenderer* _singleton;
};

//--------------------------------------------------------------
class GuiLinkAnchor
	: public GuiComponent
	, public GuiLinkRenderer::Anchor
{

public:

	//--------------------------------------------------------------
	enum Side
	{
		Side_Left = 0,
		Side_Right
	};
	
	GuiLinkAnchor();
	virtual ~GuiLinkAnchor();
	
	void setSide(Side side);
	Side getSide() const {return _side;}
	
	virtual void renderComponent(imp::Uint32 passID, float parentX, float parentY);
	
	virtual void getAnchorPosition(float& x, float& y);
	
	void setLink(GuiLinkAnchor* other);
	void unsetLink();
	
	static	GuiLinkAnchor* _currentPressed;

protected:

	Side _side;
	GuiLinkAnchor* _linkedAnchor;
	float _absPosX;
	float _absPosY;
	GuiButton* _button;
	GuiText* _text;
};

//--------------------------------------------------------------
class GuiLinkedBox : public GuiBox
{
	public:

	GuiLinkedBox();
	virtual ~GuiLinkedBox();

	void setupAnchorCount(GuiLinkAnchor::Side side, unsigned int count);
	void setAnchorText(GuiLinkAnchor::Side side, int id, const std::string& text);

	protected:
	GuiComponent* _leftSide;
	GuiComponent* _rightSide;
	
	std::vector<GuiLinkAnchor*> _leftAnchors;
	std::vector<GuiLinkAnchor*> _rightAnchors;
};

IMPGEARS_END

#endif // IMP_GUI_LINK_H
