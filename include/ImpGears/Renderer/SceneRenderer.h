#ifndef IMP_SCENE_RENDERER_H
#define IMP_SCENE_RENDERER_H

#include <Core/Object.h>

#include <Renderer/RenderVisitor.h>
#include <Renderer/RenderTarget.h>

#include <Plugins/RenderPlugin.h>

#include <SceneGraph/Graph.h>
#include <SceneGraph/ClearNode.h>
#include <SceneGraph/GeoNode.h>

IMPGEARS_BEGIN

class IMP_API SceneRenderer : public Object
{
public:

	Meta_Class(SceneRenderer)
	
	SceneRenderer();
	virtual ~SceneRenderer();

	void render(const Graph::Ptr& scene);
	
	void setDirectRendering(bool direct) {_direct = direct;}
	bool isDirectRendering() const {return _direct;}
	void setRenderTarget( RenderTarget::Ptr& targets ) {_targets = targets;}
	RenderTarget::Ptr getRenderTarget() const { return _targets; }
	
	static RenderPlugin::Ptr s_interface;

protected:
	
	bool _direct;
	RenderTarget::Ptr _targets;
	RenderVisitor::Ptr _visitor;
	
	void applyState(const State::Ptr& state);
	void applyClear(ClearNode* clearNode);
	void drawGeometry(GeoNode* geoNode);
};

IMPGEARS_END

#endif // IMP_SCENE_RENDERER_H
