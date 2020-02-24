#ifndef IMP_SCENEVISITOR_H
#define IMP_SCENEVISITOR_H

#include <SceneGraph/Visitor.h>
#include <SceneGraph/Node.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/State.h>
#include <SceneGraph/ClearNode.h>
#include <Core/Matrix4.h>

#include <Renderer/RenderQueue.h>

#include <vector>

IMPGEARS_BEGIN

struct IMP_API StackItem
{
    State::Ptr state;
    Matrix4 matrix;
    
    
    static State::Ptr s_defaultState;
    
    StackItem();
    
    void reset();
};

struct IMP_API StateStack
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

class IMP_API RenderVisitor : public Visitor
{
public:

    Meta_Class(RenderVisitor)

    RenderVisitor();
    virtual ~RenderVisitor();

    virtual void reset(RenderQueue::Ptr initQueue = nullptr);

    // return true if the node has to be traversal
    virtual bool apply( Node* node );
    virtual void push( Node* node );
    virtual void pop();

    RenderQueue::Ptr getQueue() { return _queue; }

    Node* _toSkip;

protected:

    virtual void applyDefault( Node* node );
    virtual void applyCamera( Camera* node );
    virtual void applyLightNode( LightNode* node );
    virtual void applyClearNode( ClearNode* node );
    
    StateStack stack;
    RenderQueue::Ptr _queue;
};

IMPGEARS_END

#endif // IMP_SCENEVISITOR_H
