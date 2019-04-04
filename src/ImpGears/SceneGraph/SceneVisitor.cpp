#include <SceneGraph/SceneVisitor.h>
#include <SceneGraph/Camera.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneVisitor::SceneVisitor()
{
	_gStates = GraphicStatesManager::create();
}

//--------------------------------------------------------------
SceneVisitor::~SceneVisitor()
{
}

//--------------------------------------------------------------
void SceneVisitor::apply( SceneNode* node )
{
	Matrix4 model;
	for(auto mat : _matrixStack) model *= mat;
	
	if(_gStates->getShader())
	{
		_gStates->getShader()->setModel(model);
		if(imp::Camera::getActiveCamera())
		{
			imp::Camera::getActiveCamera()->lookAt();
			_gStates->getShader()->setView( imp::Camera::getActiveCamera()->getViewMatrix() );
		}
		if(_gStates->getParameters())
			_gStates->getShader()->setProjection( _gStates->getParameters()->getProjectionMatrix() );
	}
	
	node->render();
}
//--------------------------------------------------------------
void SceneVisitor::push( SceneNode* node )
{
	_gStates->pushState(node->getGraphicState().get());
	_matrixStack.push_back(node->getModelMatrix());
}

//--------------------------------------------------------------
void SceneVisitor::pop()
{
	_gStates->popState();
	_matrixStack.pop_back();
}

IMPGEARS_END
