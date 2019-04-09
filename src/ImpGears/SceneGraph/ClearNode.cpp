#include <SceneGraph/ClearNode.h>
#include <SceneGraph/GraphRenderer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
ClearNode::ClearNode()
	: _color(0.0)
	, _depth(1.0)
{
}

//--------------------------------------------------------------
ClearNode::~ClearNode()
{
}

//--------------------------------------------------------------
void ClearNode::render()
{
	GraphRenderer::s_interface->apply(this);
}

//--------------------------------------------------------------
void ClearNode::update()
{
}

IMPGEARS_END
