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
    if(s_defaultState == nullptr) s_defaultState = State::create();
}

//--------------------------------------------------------------
void StackItem::reset()
{
    state->clone(s_defaultState,State::CloneOpt_All);
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
    
    Matrix4 prev_mat;
    if(position>0)
    {
        items[position].state->clone(items[position-1].state, State::CloneOpt_All);
        prev_mat = items[position-1].matrix;
    }
    else
        items[position].reset();
    
    items[position].state->clone(src_state, State::CloneOpt_IfChanged);
    items[position].matrix = mat * prev_mat;
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
    if(position>=0) return items[position].state;
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
    // stack = StateStack();
    stack.reset();

    _queue = initQueue;
    if( !_queue ) _queue = RenderQueue::create();
    
    _queue->_lights.clear();
    _queue->_renderBin.reset();
}

//--------------------------------------------------------------
bool RenderVisitor::apply( Node* node )
{
    if(_toSkip == node) return false;

    Camera* asCamera = dynamic_cast<Camera*>( node );
    ClearNode* asClear = dynamic_cast<ClearNode*>( node );
    LightNode* asLight = dynamic_cast<LightNode*>( node );
    GeoNode* asGeode = dynamic_cast<GeoNode*>( node );
    RenderToSamplerNode* r2s = dynamic_cast<RenderToSamplerNode*>( node );

    if( asCamera ) applyCamera(asCamera);
    else if( asClear ) applyClearNode(asClear);
    else if( asLight ) applyLightNode(asLight);
    else if(asGeode) applyDefault(node);
    else if(r2s) _queue->_renderBin.push(node->getState(),node);

    return true;
}

//--------------------------------------------------------------
void RenderVisitor::applyDefault( Node* node )
{
    Matrix4 topMat = stack.topMatrix();
    State::Ptr topState = stack.topState();

    Matrix3 normalMat = Matrix3(topMat).inverse().transpose();

    topState->setUniform("u_proj", topState->getProjectionMatrix());
    topState->setUniform("u_model", topMat);
    topState->setUniform("u_normal", normalMat);
    
    _queue->_renderBin.push(topState,node);
}

//--------------------------------------------------------------
void RenderVisitor::applyCamera( Camera* node )
{
    Matrix4 m = stack.topMatrix();
    
    Vec3 translation = Vec3(m(3,0),m(3,1),m(3,2));
    node->setAbsolutePosition( translation );
    node->lookAt();
    _queue->_camera = node;
}

//--------------------------------------------------------------
void RenderVisitor::applyLightNode( LightNode* node )
{
    Matrix4 m = stack.topMatrix();
    
    node->_worldPosition = Vec3(m(3,0),m(3,1),m(3,2));
    _queue->_lights.push_back(node);
}

//--------------------------------------------------------------
void RenderVisitor::applyClearNode( ClearNode* node )
{
    State::Ptr topState = stack.topState();
    
    _queue->_renderBin.push(topState,node);
}

//--------------------------------------------------------------
void RenderVisitor::push( Node* node )
{
    stack.push(node->getState(), node->getModelMatrix() );
    
}

//--------------------------------------------------------------
void RenderVisitor::pop()
{    
    stack.pop();
}

IMPGEARS_END
