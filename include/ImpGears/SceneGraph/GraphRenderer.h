#ifndef IMP_GRAPHRENDERER_H
#define IMP_GRAPHRENDERER_H

#include <Core/Object.h>

#include <SceneGraph/ClearNode.h>
#include <SceneGraph/SceneVisitor.h>

IMPGEARS_BEGIN

/// \brief Defines The graphic renderer.
class IMP_API GraphRenderer : public Object
{
public:

	Meta_Class(GraphRenderer)
	
	GraphRenderer();
	virtual ~GraphRenderer();

	void renderScene(SceneNode::Ptr& scene);
	
	State::Ptr getInitState() { return _initState; }

protected:
	
	ClearNode::Ptr _initNode;
	State::Ptr _initState;
	SceneVisitor::Ptr _visitor;
};

IMPGEARS_END

#endif // IMP_GRAPHRENDERER_H
