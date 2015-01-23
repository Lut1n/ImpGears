#ifndef SceneNode_H
#define SceneNode_H

#include <list>
#include "base/Vector3.h"
#include "EvnContextInterface.h"

class SceneNode;

typedef std::list<SceneNode*> SceneNodeList;
typedef SceneNodeList::iterator SceneNodeIt;

/// \brief Defines a scene node. Has to be added to a scene for rendering.
class SceneNode
{
    public:
        SceneNode();
        virtual ~SceneNode();

        virtual void render(imp::Uint32 passID) = 0;

        void setRotation(float rx, float ry, float rz){
            this->rx = rx;
            this->ry = ry;
            this->rz = rz;
        }

        void setPosition(imp::Vector3 position){
            this->position = position;
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

        bool renderActivated;
};

#endif // SceneNode_H
