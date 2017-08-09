#ifndef IMP_MESHNODE_H
#define IMP_MESHNODE_H

#include "Core/impBase.h"
#include "Graphics/SceneNode.h"
#include "Data/MeshModel.h"

IMPGEARS_BEGIN

class IMP_API MeshNode : public SceneNode
{
    public:
        MeshNode(MeshModel* model);
        virtual ~MeshNode();

        virtual void render(imp::Uint32 passID);

		const MeshModel* getModel() const {return m_model;}

    protected:
    private:

    MeshModel* m_model;
};

IMPGEARS_END

#endif // IMP_MESHNODE_H
