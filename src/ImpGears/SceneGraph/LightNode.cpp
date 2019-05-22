#include <SceneGraph/LightNode.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
LightNode::LightNode()
	: _color(1.0)
	, _power(30.0)
{
}

//--------------------------------------------------------------
LightNode::LightNode(const Vec3& color, float power)
	: _color(color)
	, _power(power)
{
}

//--------------------------------------------------------------
LightNode::~LightNode()
{
}

IMPGEARS_END
