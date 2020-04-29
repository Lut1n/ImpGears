#include <ImpGears/SceneGraph/Node.h>
#include <ImpGears/SceneGraph/Visitor.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Node::Node()
    : _state(nullptr)
    , _position(0.0)
    , _rotation(0.0)
    , _scale(1.0)
{
    _transformChanged = true;
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
void Node::clearNode()
{
    _children.clear();
}

//--------------------------------------------------------------
void Node::accept( Visitor& visitor )
{
    for(auto node : _children)
    {
        node->update();
        node->computeMatrices();
        visitor.push(node);
        visitor.apply(node);
        node->accept(visitor);
        visitor.pop();
    }
}

//--------------------------------------------------------------
void Node::computeMatrices()
{
    if(_transformChanged)
    {
        _modelMatrix = _scaleMat * _rotationMat * _positionMat;
        _transformChanged = false;
    }
}



void Node::setPosition(const Vec3& position)
{
    _position=position;
    _positionMat = Matrix4::translation(_position.x(), _position.y(), _position.z());
    _transformChanged = true;
}
void Node::setRotation(const Vec3& rotation)
{
    _rotation=rotation;
    _rotationMat = Matrix4::rotationX(_rotation.x())
            * Matrix4::rotationY(_rotation.y())
            * Matrix4::rotationZ(_rotation.z());
    _transformChanged = true;
}
void Node::setScale(const Vec3& scale)
{
    _scale=scale;
    _scaleMat = Matrix4::scale(_scale.x(), _scale.y(), _scale.z());
    _transformChanged = true;
}

State::Ptr Node::getState(bool createIfNotExist)
{
    if(createIfNotExist && _state==nullptr)
        _state = State::create();
    return _state;
}

IMPGEARS_END
