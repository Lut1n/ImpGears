#include <SceneGraph/SceneVisitor.h>
#include <SceneGraph/Camera.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneVisitor::SceneVisitor()
{
}

//--------------------------------------------------------------
SceneVisitor::SceneVisitor(GraphicStatesManager* mngr)
{
	setStateManager(mngr);
}

//--------------------------------------------------------------
SceneVisitor::~SceneVisitor()
{
}

//--------------------------------------------------------------
void SceneVisitor::apply( SceneNode* node )
{
	synchronizeStack();
	
	_stateMngr->pushState( node->getGraphicState().get() );
	
	Matrix4 model;
	for(auto n : _stack) model *= n->getModelMatrix();
	
	model *= node->getModelMatrix();
	
	if(_stateMngr->getShader())
	{
		_stateMngr->getShader()->setModel(model);
		if(imp::Camera::getActiveCamera())
		{
			imp::Camera::getActiveCamera()->lookAt();
			_stateMngr->getShader()->setView( imp::Camera::getActiveCamera()->getViewMatrix() );
		}
		if(_stateMngr->getParameters())
			_stateMngr->getShader()->setProjection( _stateMngr->getParameters()->getProjectionMatrix() );
	}
	
	
	node->render();
	
	_stateMngr->popState();
}

void SceneVisitor::synchronizeStack()
{	
	bool needSyncStacks = ( _stateMngr->size() != (int)_stack.size()+1 );
	if(needSyncStacks)
	{
		if(_stateMngr->size() < (int)_stack.size()+1) _stateMngr->pushState(_stack.back()->getGraphicState().get());
		while(_stateMngr->size() > (int)_stack.size()+1) _stateMngr->popState();
	}
}

IMPGEARS_END
