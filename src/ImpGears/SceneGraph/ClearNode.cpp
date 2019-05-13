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
	if(GraphRenderer::s_interface != nullptr)
		GraphRenderer::s_interface->apply(this);
}

//--------------------------------------------------------------
void ClearNode::update()
{
}

//--------------------------------------------------------------
void ClearNode::setTarget(const Target::Ptr& targets)
{
	_state->setTarget(targets);
}

//--------------------------------------------------------------
void ClearNode::setDefaultTarget()
{
	_state->setTarget(nullptr);
}

//--------------------------------------------------------------
Target::Ptr ClearNode::getTarget() const
{
	return _state->getTarget();
}

IMPGEARS_END
