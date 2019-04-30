#include <SceneGraph/SceneVisitor.h>
#include <SceneGraph/Camera.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneVisitor::SceneVisitor()
{
	u_proj = Uniform::create("u_proj",Uniform::Type_Mat4);
	u_view = Uniform::create("u_view",Uniform::Type_Mat4);
	u_model = Uniform::create("u_model",Uniform::Type_Mat4);
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
	State::Ptr topState = _states.back();
	
	u_model->set( _matrices.back() );
	u_proj->set( topState->getProjectionMatrix() );
	Camera* camera = Camera::getActiveCamera();
	if(camera)
		u_view->set( camera->getViewMatrix() );
	
	topState->setUniform(u_proj);
	topState->setUniform(u_model);
	topState->setUniform(u_view);
	topState->apply();
	
	node->render();
}

//--------------------------------------------------------------
void SceneVisitor::applyCamera( Camera* node )
{
	Matrix4 m = _matrices.back();
	Vec3 translation = Vec3(m(3,0),m(3,1),m(3,2));
	node->setAbsolutePosition( translation );
	node->lookAt();
}

//--------------------------------------------------------------
void SceneVisitor::push( SceneNode* node )
{
	State::Ptr state = State::create();
	if(_states.size() > 0) state->clone(_states.back(),State::CloneOpt_All);
	state->clone(node->getState(),State::CloneOpt_IfChanged);
	_states.push_back(state);
	
	Matrix4 model; if(_matrices.size() > 0) model = _matrices.back();
	_matrices.push_back(model * node->getModelMatrix());
}

//--------------------------------------------------------------
void SceneVisitor::pop()
{
	_states.pop_back();
	_matrices.pop_back();
}

IMPGEARS_END
