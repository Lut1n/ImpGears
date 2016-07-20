#ifndef IMP_SCENENODE_H
#define IMP_SCENENODE_H

#include "Core/impBase.h"
#include <list>
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "System/EvnContextInterface.h"

IMPGEARS_BEGIN

class SceneNode;

typedef std::list<SceneNode*> SceneNodeList;
typedef SceneNodeList::iterator SceneNodeIt;

/// \brief Defines a scene node. Has to be added to a scene for rendering.
class IMP_API SceneNode
{
    public:
        SceneNode();
        virtual ~SceneNode();

        virtual void render(imp::Uint32 passID) = 0;

        void setRotation(float rx, float ry, float rz){
            this->rx = rx;
            this->ry = ry;
            this->rz = rz;
            m_localMatrixHasChanged = true;
        }

        void setPosition(imp::Vector3 position){
            this->position = position;
            m_localMatrixHasChanged = true;
        }

        void setOrientation(imp::Vector3 orientation){
            this->orientation = orientation;
        }

        float getRx() const{return rx;}
        float getRy() const{return ry;}
        float getRz() const{return rz;}

        void renderAll(imp::Uint32 passID);

        const imp::Vector3 getPosition() const{return position;}
        const imp::Vector3 getOrientation() const{return orientation;}

        bool renderIsActivated() const{return renderActivated;}
        void setRenderActivated(bool activated){renderActivated = activated;}

        const Matrix4 getModelMatrix();
        const Matrix4 getNormalMatrix();
        const void setParentModelMatrices(const Matrix4& pModelMat, const Matrix4& pNormalMat)
        {
            m_parentModelMatrix = pModelMat;
            m_parentNormalMatrix = pNormalMat;
        }

        void setScale(const Vector3& _scale)
        {
            scale = _scale;
            m_localMatrixHasChanged = true;
        }

        void commitTransformation();

        static int nbDisplayed;

    protected:

        void addSubNode(SceneNode* sceneNode);
        void removeSubNode(SceneNode* sceneNode);
        void calculateRotation();

    private:
        SceneNodeList subSceneNodes;

        float rx, ry, rz;

        imp::Vector3 position;
        imp::Vector3 orientation;

        Vector3 scale;

        bool renderActivated;

        Matrix4 m_parentModelMatrix;
        Matrix4 m_parentNormalMatrix;

        Matrix4 m_localModelMatrix;
        Matrix4 m_localNormalMatrix;
        bool m_localMatrixHasChanged;
};

IMPGEARS_END

#endif // IMP_SCENENODE_H
