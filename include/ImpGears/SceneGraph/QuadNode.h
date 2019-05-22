#ifndef IMP_QUADNODE_H
#define IMP_QUADNODE_H

#include <SceneGraph/GeoNode.h>

IMPGEARS_BEGIN

class IMP_API QuadNode : public GeoNode
{
public:

	Meta_Class(QuadNode)

	QuadNode();
	virtual ~QuadNode();

	virtual void update();
};

IMPGEARS_END

#endif // IMP_QUADNODE_H
