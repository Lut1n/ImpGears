#include <SceneGraph/ClearNode.h>
#include <SceneGraph/OpenGL.h>

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
	GLbitfield bitfield = 0;
	
	if(_clearColor)
	{
		glClearColor(_color.x(), _color.y(), _color.z(), _color.w());
		bitfield = bitfield | GL_COLOR_BUFFER_BIT;
	}
	
	if(_clearDepth)
	{
		glClearDepth(_depth);
		bitfield = bitfield | GL_DEPTH_BUFFER_BIT;
	}
	
	glClear(bitfield);
}

//--------------------------------------------------------------
void ClearNode::update()
{
}

IMPGEARS_END
