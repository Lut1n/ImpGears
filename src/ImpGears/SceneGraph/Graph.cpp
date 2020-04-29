#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/Visitor.h>

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
    _initState->setPerspectiveProjection(90.0, 1.0, 0.1, 1024.0);
    _initState->setBlendMode(State::BlendMode_SrcAlphaBased);
    _initState->setFaceCullingMode(State::FaceCullingMode_None);
    _initState->setViewport(0.0,0.0,512.0,512.0);
    _initState->setLineWidth(1.0);
    _initState->setDepthTest( true );
    _initState->setTransparent( false );
    
    ReflexionModel::Ptr reflexion = ReflexionModel::create();
    RenderPass::Ptr pass_info = RenderPass::create();
    _initState->setReflexion(reflexion);
    _initState->setRenderPass(pass_info);
}

//--------------------------------------------------------------
Graph::~Graph()
{
}

//---------------------------------------------------------------
void Graph::accept(Visitor& visitor)
{
    visitor.push(_initNode);
    visitor.apply(_initNode);

    _root->update();
    _root->computeMatrices();
    visitor.push(_root);
    visitor.apply(_root);
    if(_root) _root->accept(visitor);
    visitor.pop();


    visitor.pop();
}

//---------------------------------------------------------------
void Graph::setRoot(const Node::Ptr& node)
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
