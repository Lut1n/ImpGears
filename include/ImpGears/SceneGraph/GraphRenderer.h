#ifndef IMP_GRAPHRENDERER_H
#define IMP_GRAPHRENDERER_H

#include <Core/Object.h>

#include <SceneGraph/ClearNode.h>
#include <SceneGraph/SceneVisitor.h>
#include <SceneGraph/RefactoInterface.h>

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
	
	void setClearColor(const Vec4& color);
	void setClearDepth(float depth);
	
	static RefactoInterface* s_interface;

protected:
	
	ClearNode::Ptr _initNode;
	State::Ptr _initState;
	SceneVisitor::Ptr _visitor;
};

IMPGEARS_END

#endif // IMP_GRAPHRENDERER_H
