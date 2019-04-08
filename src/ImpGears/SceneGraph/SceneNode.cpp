#include <SceneGraph/SceneNode.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneNode::SceneNode()
	: _state(nullptr)
	, _position(0.0)
    , _rotation(0.0)
	, _scale(1.0)
{
	_state = State::create();
}

//--------------------------------------------------------------
SceneNode::~SceneNode()
{
}

//--------------------------------------------------------------
void SceneNode::addNode(const SceneNode::Ptr& node)
{
    _children.push_back( node );
}

//--------------------------------------------------------------
void SceneNode::remNode(const SceneNode::Ptr& node)
{
    _children.remove( node );
}

//--------------------------------------------------------------
void SceneNode::accept( Visitor<SceneNode*>::Ptr& visitor )
{
    update();
	computeMatrices();
	
	visitor->push(this);
	visitor->apply(this);
    for(auto node:_children) node->accept(visitor);
	visitor->pop();
}

//--------------------------------------------------------------
void SceneNode::computeMatrices()
{
	_modelMatrix =
		Matrix4::scale(_scale.x(), _scale.y(), _scale.z())
		* Matrix4::rotationX(_rotation.x())
		* Matrix4::rotationY(_rotation.y())
		* Matrix4::rotationZ(_rotation.z())
		* Matrix4::translation(_position.x(), _position.y(), _position.z());

	_normalMatrix =
		Matrix4::rotationX(_rotation.x())
		* Matrix4::rotationY(_rotation.y())
		* Matrix4::rotationZ(_rotation.z())
		* Matrix4::scale(_scale.x(), _scale.y(), _scale.z()).inverse();
}

IMPGEARS_END
