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
	Camera* asCamera = dynamic_cast<Camera*>( node );
	if( asCamera ) applyCamera(asCamera);
	else applyDefault(node);
}

//--------------------------------------------------------------
void SceneVisitor::applyDefault( SceneNode* node )
{
	if(_gStates->getShader())
	{
		_gStates->getShader()->setModel(_matrixStack.back());
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
void SceneVisitor::applyCamera( Camera* node )
{
	Matrix4 m = _matrixStack.back();
	Vec3 translation = Vec3(m(3,0),m(3,1),m(3,2));
	node->setAbsolutePosition( translation );
	node->lookAt();
}

//--------------------------------------------------------------
void SceneVisitor::push( SceneNode* node )
{
	_gStates->pushState(node->getGraphicState().get());
	
	Matrix4 model; if(_matrixStack.size() > 0) model = _matrixStack.back();
	_matrixStack.push_back(model * node->getModelMatrix());
}

//--------------------------------------------------------------
void SceneVisitor::pop()
{
	_gStates->popState();
	_matrixStack.pop_back();
}

IMPGEARS_END
