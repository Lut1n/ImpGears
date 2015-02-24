#ifndef IMP_MESHNODE_H
#define IMP_MESHNODE_H

#include "base/impBase.h"
#include "scene/SceneNode.h"
#include "mesh/MeshModel.h"

IMPGEARS_BEGIN

class IMP_API MeshNode : public SceneNode
{
    public:
        MeshNode(MeshModel* model);
        virtual ~MeshNode();

        virtual void render(imp::Uint32 passID);
    protected:
    private:

    MeshModel* m_model;
};

IMPGEARS_END

#endif // IMP_MESHNODE_H
