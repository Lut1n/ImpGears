#include <SceneGraph/Graph.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Graph::Graph()
{
    _initNode = ClearNode::create();
    _initNode->setDepth(1);
    _initNode->setColor( Vec4(0.0) );
    _initNode->enableDepth(true);
    _initNode->enableColor(true);

    // default parameters values
    _initState = _initNode->getState();
    _initState->setPerspectiveProjection(90.0, 1.0, 0.1, 128.0);
    _initState->setBlendMode(State::BlendMode_SrcAlphaBased);
    _initState->setFaceCullingMode(State::FaceCullingMode_None);
    _initState->setViewport(0.0,0.0,500.0,500.0);
    _initState->setLineWidth(1.0);
    _initState->setDepthTest( true );
}

//--------------------------------------------------------------
Graph::~Graph()
{
}

//---------------------------------------------------------------
void Graph::accept(Visitor::Ptr& visitor)
{
    visitor->push(_initNode.get());
    visitor->apply(_initNode.get());
    if(_root) _root->accept(visitor);
    visitor->pop();
}

//---------------------------------------------------------------
void Graph::setRoot(Node::Ptr& node)
{
    _root = node;
}

//---------------------------------------------------------------
void Graph::setClearColor(const Vec4& color)
{
    _initNode->setColor(color);
}

//---------------------------------------------------------------
void Graph::setClearDepth(float depth)
{
    _initNode->setDepth(depth);
}

IMPGEARS_END
