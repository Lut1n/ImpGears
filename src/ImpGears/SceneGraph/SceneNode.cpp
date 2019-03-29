#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/SceneNode.h>
#include <SceneGraph/OpenGL.h>
#include <SceneGraph/Camera.h>

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
    m_parentModelMatrix = Matrix4();
    m_parentNormalMatrix = Matrix4();

    m_localModelMatrix = Matrix4();
    m_localNormalMatrix = Matrix4();

    scale = Vec3(1.f, 1.f, 1.f);
}

//--------------------------------------------------------------
SceneNode::~SceneNode()
{
}

//--------------------------------------------------------------
void SceneNode::addSubNode(const SceneNode::Ptr& sceneNode)
{
    subSceneNodes.push_back( sceneNode );
}

//--------------------------------------------------------------
void SceneNode::removeSubNode(const SceneNode::Ptr& sceneNode)
{
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
            Matrix4::scale(scale.x(), scale.y(), scale.z())
            * Matrix4::rotationX(rx)
            * Matrix4::rotationY(ry)
            * Matrix4::rotationZ(rz)
            * Matrix4::translation(position.x(), position.y(), position.z());

        m_localNormalMatrix =
            Matrix4::rotationX(rx)
            * Matrix4::rotationY(ry)
            * Matrix4::rotationZ(rz)
            * Matrix4::scale(scale.x(), scale.y(), scale.z()).inverse();

        m_localMatrixHasChanged = false;
    }
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
GraphicState::Ptr SceneNode::getGraphicState()
{
	if(_state == nullptr)_state = GraphicState::create();
	return _state;
}

IMPGEARS_END
