#ifndef IMP_QUADNODE_H
#define IMP_QUADNODE_H

#include <Renderer/RenderPlugin.h>
#include <SceneGraph/Node.h>
#include <Geometry/Geometry.h>

IMPGEARS_BEGIN

class IMP_API QuadNode : public Node
{
public:

	Meta_Class(QuadNode)

	QuadNode();
	virtual ~QuadNode();

	virtual void render();
	
protected:
	
	Geometry _geo;
	RenderPlugin::Data::Ptr _gData;
};

IMPGEARS_END

#endif // IMP_QUADNODE_H
