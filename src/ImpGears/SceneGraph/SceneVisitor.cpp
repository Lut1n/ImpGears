#include <SceneGraph/SceneVisitor.h>
#include <SceneGraph/Camera.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneVisitor::SceneVisitor()
{
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
	topState->apply();
	
	Shader::Ptr shader = topState->getShader();
	if(shader)
	{
		shader->setModel(_matrices.back());
		Camera* camera = Camera::getActiveCamera();
		if(camera) shader->setView( camera->getViewMatrix() );
		shader->setProjection( topState->getProjectionMatrix() );
	}
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
