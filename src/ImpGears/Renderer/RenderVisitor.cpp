#include <Renderer/RenderVisitor.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/ClearNode.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
RenderVisitor::RenderVisitor()
{
	u_proj = Uniform::create("u_proj",Uniform::Type_Mat4);
	// u_view = Uniform::create("u_view",Uniform::Type_Mat4);
	u_model = Uniform::create("u_model",Uniform::Type_Mat4);
	u_normal = Uniform::create("u_normal",Uniform::Type_Mat3);
}

//--------------------------------------------------------------
RenderVisitor::~RenderVisitor()
{
}

//--------------------------------------------------------------
void RenderVisitor::reset()
{
	_queue = RenderQueue::create();
	u_model->set( Matrix4() );
	u_proj->set( Matrix4() );
	// u_view->set( Matrix4() );
	u_normal->set( Matrix3() );
}

//--------------------------------------------------------------
void RenderVisitor::apply( Node* node )
{
	Camera* asCamera = dynamic_cast<Camera*>( node );
	ClearNode* asClear = dynamic_cast<ClearNode*>( node );
	
	if( asCamera ) applyCamera(asCamera);
	else if( asClear ) applyClearNode(asClear);
	else applyDefault(node);
}

//--------------------------------------------------------------
void RenderVisitor::applyDefault( Node* node )
{
	State::Ptr topState = _states.back();
	
	u_model->set( _matrices.back() );
	u_proj->set( topState->getProjectionMatrix() );
	
	/*Matrix4 mv = _matrices.back() * u_view->getMat4();
	Matrix3 normalMat = Matrix3(mv).inverse().transpose();*/
	
	Matrix3 normalMat = Matrix3(_matrices.back()).inverse().transpose();
	u_normal->set( normalMat );
	
	topState->setUniform(u_proj);
	topState->setUniform(u_model);
	// topState->setUniform(u_view);
	topState->setUniform(u_normal);
	
	
	_queue->_states.push_back(topState);
	_queue->_nodes.push_back(node);
	// _queue->_modelMats.push_back();
	
	// topState->apply();
	// node->render();
}

//--------------------------------------------------------------
void RenderVisitor::applyCamera( Camera* node )
{
	Matrix4 m = _matrices.back();
	Vec3 translation = Vec3(m(3,0),m(3,1),m(3,2));
	node->setAbsolutePosition( translation );
	node->lookAt();
	// u_view->set( node->getViewMatrix() );
	_queue->_camera = node;
}

//--------------------------------------------------------------
void RenderVisitor::applyClearNode( ClearNode* node )
{
	State::Ptr topState = _states.back();
	_queue->_states.push_back(topState);
	_queue->_nodes.push_back(node);
}

//--------------------------------------------------------------
void RenderVisitor::push( Node* node )
{
	State::Ptr state = State::create();
	if(_states.size() > 0) state->clone(_states.back(),State::CloneOpt_All);
	state->clone(node->getState(),State::CloneOpt_IfChanged);
	_states.push_back(state);
	
	Matrix4 model; if(_matrices.size() > 0) model = _matrices.back();
	_matrices.push_back(model * node->getModelMatrix());
}

//--------------------------------------------------------------
void RenderVisitor::pop()
{
	_states.pop_back();
	_matrices.pop_back();
}

IMPGEARS_END
