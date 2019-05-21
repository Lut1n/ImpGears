#ifndef IMP_GRAPH_H
#define IMP_GRAPH_H

#include <Core/Object.h>

#include <SceneGraph/ClearNode.h>
#include <Renderer/SceneVisitor.h>
#include <Renderer/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API Graph : public Object
{
public:

	Meta_Class(Graph)
	
	Graph();
	virtual ~Graph();

	void renderScene(const Node::Ptr& scene);
	
	State::Ptr getInitState() { return _initState; }
	
	void setClearColor(const Vec4& color);
	void setClearDepth(float depth);
	void setTarget(const Target::Ptr& target);
	void setDefaultTarget();
	
	static RenderPlugin::Ptr s_interface;

protected:
	
	ClearNode::Ptr _initNode;
	State::Ptr _initState;
	SceneVisitor::Ptr _visitor;
};

IMPGEARS_END

#endif // IMP_GRAPH_H
