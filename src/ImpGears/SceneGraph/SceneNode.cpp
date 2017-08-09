#include "Graphics/GraphicRenderer.h"
#include "Graphics/SceneNode.h"
#include "Graphics/GLcommon.h"
#include <iostream>

#include <cmath>

IMPGEARS_BEGIN

//--------------------------------------------------------------
int SceneNode::nbDisplayed = 0;

//--------------------------------------------------------------
SceneNode::SceneNode():
	_state(nullptr),
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
void SceneNode::addSubNode(const std::shared_ptr<SceneNode>& sceneNode){
    subSceneNodes.push_back( sceneNode );
}

//--------------------------------------------------------------
void SceneNode::removeSubNode(const std::shared_ptr<SceneNode>& sceneNode){
    subSceneNodes.remove( sceneNode );
}

//--------------------------------------------------------------
void SceneNode::renderAll(imp::Uint32 passID)
{	
    if(!renderActivated)return;

    SceneNode::nbDisplayed++;

    Matrix4 modelMat = getModelMatrix();
    Matrix4 normalMat = getNormalMatrix();
	
	GraphicRenderer* renderer = GraphicRenderer::getInstance();
	GraphicStatesManager& states = renderer->getStateManager();
	
	states.pushState( _state.get() );
	if(states.getShader())
	{
		states.getShader()->setModel(modelMat);
		/*if(imp::Camera::getActiveCamera())
			states.getShader()->setView( imp::Camera::getActiveCamera()->getViewMatrix() );*/
		if(states.getParameters())
			states.getShader()->setProjection( states.getParameters()->getProjectionMatrix() );
	}
	
    render(passID);

    for(SceneNodeIt it = subSceneNodes.begin(); it != subSceneNodes.end(); it++)
    {
        SceneNode* sub = it->get();
        sub->setParentModelMatrices(modelMat, normalMat);
        sub->renderAll(passID);
    }
	
	states.popState();
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

//--------------------------------------------------------------
std::shared_ptr<GraphicState> SceneNode::getGraphicState()
{
	if(_state == nullptr)_state = std::shared_ptr<GraphicState>(new GraphicState());
	return _state;
}

IMPGEARS_END
