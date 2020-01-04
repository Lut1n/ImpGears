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

class IMP_API RenderVisitor : public Visitor
{
public:

    Meta_Class(RenderVisitor)

    using MatrixStack = std::vector<Matrix4>;
    using StateStack = std::vector<State::Ptr>;

    RenderVisitor();
    virtual ~RenderVisitor();

    virtual void reset();

    // return true if the node has to be traversal
    virtual bool apply( Node* node );
    virtual void push( Node* node );
    virtual void pop();

    RenderQueue::Ptr getQueue() { return _queue; }

    // initial config
    Node* _toSkip;

protected:

    virtual void applyDefault( Node* node );
    virtual void applyCamera( Camera* node );
    virtual void applyLightNode( LightNode* node );
    virtual void applyClearNode( ClearNode* node );

    MatrixStack _matrices;
    StateStack _states;
    Uniform::Ptr u_proj;
    // Uniform::Ptr u_view;
    Uniform::Ptr u_model;
    Uniform::Ptr u_normal;

    RenderQueue::Ptr _queue;
};

IMPGEARS_END

#endif // IMP_SCENEVISITOR_H
