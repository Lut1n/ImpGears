#ifndef IMP_QUADNODE_H
#define IMP_QUADNODE_H

#include <ImpGears/SceneGraph/GeoNode.h>
#include <ImpGears/SceneGraph/Export.h>

IMPGEARS_BEGIN

class IG_SCENE_API QuadNode : public GeoNode
{
public:

    Meta_Class(QuadNode)

    QuadNode();
    virtual ~QuadNode();
};

IMPGEARS_END

#endif // IMP_QUADNODE_H
