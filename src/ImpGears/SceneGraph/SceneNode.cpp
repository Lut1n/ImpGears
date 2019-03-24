#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/SceneNode.h>
#include <SceneGraph/OpenGL.h>
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

    scale = Vec3(1.f, 1.f, 1.f);
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
void SceneNode::renderAll()
{	
    if(!renderActivated)return;

    SceneNode::nbDisplayed++;

    Matrix4 modelMat = getModelMatrix();
    Matrix4 normalMat = getNormalMatrix();
	
    for(auto node : subSceneNodes)
	{
        node->setParentModelMatrices(modelMat, normalMat);
	}
	
	GraphicRenderer* renderer = GraphicRenderer::getInstance();
	GraphicStatesManager& states = renderer->getStateManager();
	
	states.pushState( _state.get() );
	if(states.getShader())
	{
		states.getShader()->setModel(modelMat);
		if(imp::Camera::getActiveCamera())
		{
			imp::Camera::getActiveCamera()->lookAt();
			states.getShader()->setView( imp::Camera::getActiveCamera()->getViewMatrix() );
		}
		if(states.getParameters())
			states.getShader()->setProjection( states.getParameters()->getProjectionMatrix() );
	}
	
    render();

    for(auto node : subSceneNodes)
	{
		node->renderAll();
	}
	
	states.popState();
}

//--------------------------------------------------------------
void SceneNode::commitTransformation()
{
    if(m_localMatrixHasChanged)
    {
        m_localModelMatrix =
            Matrix4::getScaleMat(scale.x(), scale.y(), scale.z())
            * Matrix4::getRotationMat(rx, 0.f, 0.f)
            * Matrix4::getRotationMat(0.f, ry, 0.f)
            * Matrix4::getRotationMat(0.f, 0.f, rz)
            * Matrix4::getTranslationMat(position.x(), position.y(), position.z());

        m_localNormalMatrix =
            Matrix4::getRotationMat(rx, 0.f, 0.f)
            * Matrix4::getRotationMat(0.f, ry, 0.f)
            * Matrix4::getRotationMat(0.f, 0.f, rz)
            * Matrix4::getScaleMat(scale.x(), scale.y(), scale.z()).getInverse();

        m_localMatrixHasChanged = false;
    }
}

//--------------------------------------------------------------
void SceneNode::calculateRotation(){

    orientation.normalize();

    float convertion = 180.f/3.14159265359f;

    float x2 = orientation.x()*orientation.x();
    float y2 = orientation.y()*orientation.y();
    float xy = sqrt(x2+y2);

    rx = 0.f; //Rotation sur axe frontal
    ry = -convertion * atan2f(orientation.z(), xy);
    rz = convertion * atan2f(orientation.y(), orientation.x());

    m_localMatrixHasChanged = true;
}

//--------------------------------------------------------------
Matrix4 SceneNode::getModelMatrix()
{
    commitTransformation();

    return m_parentModelMatrix * m_localModelMatrix;
}

//--------------------------------------------------------------
Matrix4 SceneNode::getNormalMatrix()
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
