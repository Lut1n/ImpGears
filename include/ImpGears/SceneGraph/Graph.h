#ifndef IMP_GRAPH_H
#define IMP_GRAPH_H

#include <Core/Object.h>

#include <SceneGraph/ClearNode.h>
#include <SceneGraph/Visitor.h>

IMPGEARS_BEGIN

class IMP_API Graph : public Object
{
public:

    Meta_Class(Graph)

    Graph();
    virtual ~Graph();

    State::Ptr getInitState() { return _initState; }
    const State::Ptr& getInitState() const { return _initState; }

    void accept(Visitor::Ptr& visitor);
    void setRoot(Node::Ptr& node);
    const Node::Ptr& getRoot() const {return _root;}

    void setClearColor(const Vec4& color);
    void setClearDepth(float depth);

protected:

    Node::Ptr _root;
    ClearNode::Ptr _initNode;
    State::Ptr _initState;
};

IMPGEARS_END

#endif // IMP_GRAPH_H
