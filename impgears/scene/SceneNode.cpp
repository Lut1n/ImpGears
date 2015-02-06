#include "SceneNode.h"
#include "../graphics/GLcommon.h"

#include <cmath>

IMPGEARS_BEGIN

//--------------------------------------------------------------
int SceneNode::nbDisplayed = 0;

//--------------------------------------------------------------
SceneNode::SceneNode():
    rx(0.f), ry(0.f), rz(0.f), renderActivated(true)
{
}

//--------------------------------------------------------------
SceneNode::~SceneNode(){

}

//--------------------------------------------------------------
void SceneNode::addSubNode(SceneNode* sceneNode){
    subSceneNodes.push_back(sceneNode);
}

//--------------------------------------------------------------
void SceneNode::removeSubNode(SceneNode* sceneNode){
    subSceneNodes.remove(sceneNode);
}

//--------------------------------------------------------------
void SceneNode::renderAll(imp::Uint32 passID){

    if(!renderActivated)return;

    SceneNode::nbDisplayed++;

    glPushMatrix();

    glTranslatef(position.getX(), position.getY(), position.getZ());
    glRotatef(rz, 0.f, 0.f, 1.f);
    glRotatef(ry, 0.f, 1.f, 0.f);
    glRotatef(rx, 1.f, 0.f, 0.f);

    render(passID);

    for(SceneNodeIt it = subSceneNodes.begin(); it != subSceneNodes.end(); it++){
        SceneNode* sub = *it;
        sub->renderAll(passID);
    }

    glPopMatrix();
}

//--------------------------------------------------------------
void SceneNode::calculateRotation(){

    orientation.normalize();

    float convertion = 180.f/3.14159265359f;

    float x2 = orientation.getX()*orientation.getX();
    float y2 = orientation.getY()*orientation.getY();
    float xy = sqrt(x2+y2);

    rx = 0.f; //Rotation sur axe frontal
    ry = -convertion * atan2f(orientation.getZ(), xy);
    rz = convertion * atan2f(orientation.getY(), orientation.getX());
}

IMPGEARS_END
