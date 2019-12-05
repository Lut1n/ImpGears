#include <SceneGraph/Node.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Node::Node()
    : _state(nullptr)
    , _position(0.0)
    , _rotation(0.0)
    , _scale(1.0)
{
    _state = State::create();
}

//--------------------------------------------------------------
Node::~Node()
{
}

//--------------------------------------------------------------
void Node::addNode(const Node::Ptr& node)
{
    _children.push_back( node );
}

//--------------------------------------------------------------
void Node::remNode(const Node::Ptr& node)
{
    _children.remove( node );
}

//--------------------------------------------------------------
void Node::accept( Visitor::Ptr& visitor )
{
    update();
    computeMatrices();

    visitor->push(this);
    visitor->apply(this);
    for(auto node:_children) node->accept(visitor);
    visitor->pop();
}

//--------------------------------------------------------------
void Node::computeMatrices()
{
    _modelMatrix =
            Matrix4::scale(_scale.x(), _scale.y(), _scale.z())
            * Matrix4::rotationX(_rotation.x())
            * Matrix4::rotationY(_rotation.y())
            * Matrix4::rotationZ(_rotation.z())
            * Matrix4::translation(_position.x(), _position.y(), _position.z());
}

IMPGEARS_END
