#ifndef IMP_SCENEVISITOR_H
#define IMP_SCENEVISITOR_H

#include <ImpGears/SceneGraph/Visitor.h>
#include <ImpGears/SceneGraph/Node.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/State.h>
#include <ImpGears/SceneGraph/ClearNode.h>
#include <ImpGears/Core/Matrix4.h>
#include <ImpGears/Renderer/Export.h>

#include <ImpGears/Renderer/RenderQueue.h>

#include <vector>

IMPGEARS_BEGIN

struct IG_RENDERER_API StackItem
{
    State::Ptr state;
    State::Ptr overrideState;
    Matrix4 matrix;
    
    
    static State::Ptr s_defaultState;
    
    StackItem();
    
    void reset();
};

struct IG_RENDERER_API StateStack
{
    std::vector<StackItem> items;
    int position;
    
    StateStack();
    
    void push(const State::Ptr state, const Matrix4& mat);
    void pop();
    
    int size();
    void reset();
    
    State::Ptr topState() const;
    Matrix4 topMatrix() const;
};

class IG_RENDERER_API RenderVisitor : public Visitor
{
public:

    Meta_Class(RenderVisitor)

    RenderVisitor();
    virtual ~RenderVisitor();

    virtual void reset(RenderQueue::Ptr initQueue = nullptr);

    // return true if the node has to be traversal
    virtual bool apply( Node::Ptr node );
    virtual void push( Node::Ptr node );
    virtual void pop();

    RenderQueue::Ptr getQueue() { return _queue; }

    Node* _toSkip;

protected:

    virtual void applyDefault( Node::Ptr node );
    virtual void applyCamera( Camera::Ptr node );
    virtual void applyLightNode( LightNode::Ptr node );
    virtual void applyClearNode( ClearNode::Ptr node );
    
    StateStack stack;
    RenderQueue::Ptr _queue;
};

IMPGEARS_END

#endif // IMP_SCENEVISITOR_H
