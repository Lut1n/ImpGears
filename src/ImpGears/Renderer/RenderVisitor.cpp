#include <Renderer/RenderVisitor.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/ClearNode.h>

#include <SceneGraph/GeoNode.h>
#include <SceneGraph/ReflexionModel.h>
#include <SceneGraph/RenderPass.h>
#include <SceneGraph/RenderToSamplerNode.h>

IMPGEARS_BEGIN

State::Ptr StackItem::s_defaultState = nullptr;

//--------------------------------------------------------------
StackItem::StackItem()
{
    state = State::create();
    overrideState = nullptr;
    if(s_defaultState == nullptr) s_defaultState = State::create();
}

//--------------------------------------------------------------
void StackItem::reset()
{
    state->clone(s_defaultState,State::CloneOpt_OverrideRef);
    overrideState = nullptr;
    matrix = Matrix4();
}

//--------------------------------------------------------------
StateStack::StateStack()
    : position(-1)
{
}

//--------------------------------------------------------------
void StateStack::push(const State::Ptr src_state, const Matrix4& mat)
{
    position++;
    if( size()>(int)items.size() ) items.resize( size() );
    
    StackItem& curr = items[position];
    curr.overrideState = nullptr;
    
    if(position == 0)
    {
        if(src_state)
            curr.state->clone(src_state, State::CloneOpt_OverrideRef);
        else 
            curr.reset();
        curr.matrix = mat;
    }
    else
    {
        StackItem& prev = items[position-1];
        State::Ptr& baseSt = prev.overrideState ? prev.overrideState : prev.state;
        curr.matrix = mat * prev.matrix;
        if(src_state)
        {
            curr.state->clone(baseSt, State::CloneOpt_OverrideRef);
            curr.state->clone(src_state, State::CloneOpt_OverrideChangedRef);
        }
        else
        {
            curr.overrideState = baseSt;
        }
        
    }
}

//--------------------------------------------------------------
void StateStack::pop()
{
    if(position>=0) position--;
}

//--------------------------------------------------------------
int StateStack::size()
{
    return position+1;
}

//--------------------------------------------------------------
void StateStack::reset()
{
    position = -1;
}

//--------------------------------------------------------------
State::Ptr StateStack::topState() const
{
    if(position>=0)
    {
        return items[position].overrideState ? items[position].overrideState : items[position].state;
    }
    return nullptr;
}

//--------------------------------------------------------------
Matrix4 StateStack::topMatrix() const
{
    if(position>=0) return items[position].matrix;
    return Matrix4();
}



//--------------------------------------------------------------
RenderVisitor::RenderVisitor()
    : _toSkip(nullptr)
{
}

//--------------------------------------------------------------
RenderVisitor::~RenderVisitor()
{
}

//--------------------------------------------------------------
void RenderVisitor::reset( RenderQueue::Ptr initQueue )
{
    if( stack.size()>0 )
    {
        std::cout << "Warning : matrices or states stacks are not consistent" << std::endl;
    }
    stack.reset();

    _queue = initQueue;
    if( !_queue ) _queue = RenderQueue::create();
    
    _queue->_camera = nullptr;
    _queue->_lights.clear();
    _queue->_renderBin.reset();
    _queue->_transparentRenderBin.reset();
}

//--------------------------------------------------------------
bool RenderVisitor::apply( Node::Ptr node )
{
    Camera::Ptr asCamera = std::dynamic_pointer_cast<Camera>( node );
    ClearNode::Ptr asClear = std::dynamic_pointer_cast<ClearNode>( node );
    LightNode::Ptr asLight = std::dynamic_pointer_cast<LightNode>( node );
    GeoNode::Ptr asGeode = std::dynamic_pointer_cast<GeoNode>( node );
    RenderToSamplerNode::Ptr r2s = std::dynamic_pointer_cast<RenderToSamplerNode>( node );

    if( asCamera ) applyCamera(asCamera);
    else if( asClear ) applyClearNode(asClear);
    else if( asLight ) applyLightNode(asLight);
    else if(asGeode) applyDefault(node);
    else if(r2s) _queue->_renderBin.push(node->getState(),node,Matrix4());

    return true;
}

//--------------------------------------------------------------
void RenderVisitor::applyDefault( Node::Ptr node )
{
    Matrix4 topMat = stack.topMatrix();
    State::Ptr topState = stack.topState();
    
    if(topState->getTransparent())
        _queue->_transparentRenderBin.push(topState,node,topMat);
    else
        _queue->_renderBin.push(topState,node,topMat);
}

//--------------------------------------------------------------
void RenderVisitor::applyCamera( Camera::Ptr node )
{
    Matrix4 m = stack.topMatrix();
    
    Vec3 translation = Vec3(m(3,0),m(3,1),m(3,2));
    node->setAbsolutePosition( translation );
    node->lookAt();
    _queue->_camera = node.get();
}

//--------------------------------------------------------------
void RenderVisitor::applyLightNode( LightNode::Ptr node )
{
    Matrix4 m = stack.topMatrix();
    
    node->_worldPosition = Vec3(m(3,0),m(3,1),m(3,2));
    _queue->_lights.push_back(node.get());
}

//--------------------------------------------------------------
void RenderVisitor::applyClearNode( ClearNode::Ptr node )
{
    State::Ptr topState = stack.topState();
    
    _queue->_renderBin.push(topState,node, Matrix4() );
}

//--------------------------------------------------------------
void RenderVisitor::push( Node::Ptr node )
{
    stack.push(node->getState(false), node->getModelMatrix() );
    
}

//--------------------------------------------------------------
void RenderVisitor::pop()
{    
    stack.pop();
}

IMPGEARS_END
