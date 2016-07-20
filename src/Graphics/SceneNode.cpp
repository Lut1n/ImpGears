#include "Graphics/SceneNode.h"
#include "Graphics/GLcommon.h"

#include <cmath>

IMPGEARS_BEGIN

//--------------------------------------------------------------
int SceneNode::nbDisplayed = 0;

//--------------------------------------------------------------
SceneNode::SceneNode():
    rx(0.f),
    ry(0.f),
    rz(0.f),
    renderActivated(true),
    m_localMatrixHasChanged(true)
{
    m_parentModelMatrix = Matrix4::getIdentityMat();
    m_parentNormalMatrix = Matrix4::getIdentityMat();

    m_localModelMatrix = Matrix4::getIdentityMat();
    m_localNormalMatrix = Matrix4::getIdentityMat();

    scale = Vector3(1.f, 1.f, 1.f);
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

    Matrix4 modelMat = getModelMatrix();
    Matrix4 normalMat = getNormalMatrix();

    render(passID);

    for(SceneNodeIt it = subSceneNodes.begin(); it != subSceneNodes.end(); it++)
    {
        SceneNode* sub = *it;
        sub->setParentModelMatrices(modelMat, normalMat);
        sub->renderAll(passID);
    }
}

//--------------------------------------------------------------
void SceneNode::commitTransformation()
{
    if(m_localMatrixHasChanged)
    {
        m_localModelMatrix =
            Matrix4::getTranslationMat(position.getX(), position.getY(), position.getZ())
            * Matrix4::getRotationMat(rx, 0.f, 0.f)
            * Matrix4::getRotationMat(0.f, ry, 0.f)
            * Matrix4::getRotationMat(0.f, 0.f, rz)
            * Matrix4::getScaleMat(scale.getX(), scale.getY(), scale.getZ());

        m_localNormalMatrix =
            Matrix4::getRotationMat(rx, 0.f, 0.f)
            * Matrix4::getRotationMat(0.f, ry, 0.f)
            * Matrix4::getRotationMat(0.f, 0.f, rz)
            * Matrix4::getScaleMat(scale.getX(), scale.getY(), scale.getZ()).getInverse();

        m_localMatrixHasChanged = false;
    }
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

    m_localMatrixHasChanged = true;
}

//--------------------------------------------------------------
const Matrix4 SceneNode::getModelMatrix()
{
    commitTransformation();

    return m_parentModelMatrix * m_localModelMatrix;
}

//--------------------------------------------------------------
const Matrix4 SceneNode::getNormalMatrix()
{
    commitTransformation();

    return m_parentNormalMatrix * m_localNormalMatrix;
}

IMPGEARS_END
