#ifndef IMP_SCENE_RENDERER_H
#define IMP_SCENE_RENDERER_H

#include <Core/Object.h>

#include <Renderer/RenderVisitor.h>
#include <Renderer/RenderPlugin.h>

#include <SceneGraph/Graph.h>

IMPGEARS_BEGIN

class IMP_API SceneRenderer : public Object
{
public:

	Meta_Class(SceneRenderer)
	
	SceneRenderer();
	virtual ~SceneRenderer();

	void render(const Graph::Ptr& scene);
	
	static RenderPlugin::Ptr s_interface;

protected:
	
	RenderVisitor::Ptr _visitor;
};

IMPGEARS_END

#endif // IMP_SCENE_RENDERER_H
