#ifndef SceneNode_H
#define SceneNode_H

#include <list>
#include "base/Vector3.h"
#include "EvnContextInterface.h"

class SceneNode;

typedef std::list<SceneNode*> SceneNodeList;
typedef SceneNodeList::iterator SceneNodeIt;

class SceneNode
{
    public:
        SceneNode();
        virtual ~SceneNode();

        virtual void render() = 0;
        virtual void update() = 0;
        virtual void onEvent(imp::Event evn) = 0;

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

        void renderAll();
        void updateAll();
        void onEventAll(imp::Event evn);

        const imp::Vector3 getPosition() const{return position;}
        const imp::Vector3 getOrientation() const{return orientation;}

        bool renderIsActivated() const{return renderActivated;}
        void setRenderActivated(bool activated){renderActivated = activated;}


        static int nbDisplayed;

    protected:

        void addSubSceneNode(SceneNode* SceneNode);
        void removeSubSceneNode(SceneNode* SceneNode);

        /**
         * Prevoir un parametre supplementaire pour la rotation sur axe frontal (x)
         */
        void calculateRotation();

    private:
        SceneNodeList subSceneNodes;

        float rx, ry, rz;

        imp::Vector3 position;
        imp::Vector3 orientation;

        bool renderActivated;
};

#endif // SceneNode_H
