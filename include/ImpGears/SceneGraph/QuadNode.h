#ifndef IMP_QUADNODE_H
#define IMP_QUADNODE_H

#include <SceneGraph/RenderPlugin.h>
#include <SceneGraph/SceneNode.h>

IMPGEARS_BEGIN

class IMP_API QuadNode : public SceneNode
{
public:

	Meta_Class(QuadNode)

	QuadNode();
	virtual ~QuadNode();

	virtual void render();
	
protected:
	
	RenderPlugin::Data* _gData;
};

IMPGEARS_END

#endif // IMP_QUADNODE_H
