#ifndef IMP_RT_SCENE_RENDERER_H
#define IMP_RT_SCENE_RENDERER_H

#include <Renderer/SceneRenderer.h>
#include <Renderer/RenderTarget.h>

#include <SceneGraph/Graph.h>
#include <SceneGraph/ClearNode.h>
#include <SceneGraph/GeoNode.h>

IMPGEARS_BEGIN

class IMP_API RTSceneRenderer : public SceneRenderer
{
public:

	Meta_Class(RTSceneRenderer)
	
	RTSceneRenderer();
	virtual ~RTSceneRenderer();

	virtual void render(const Graph::Ptr& scene);

protected:
	
	// RenderTarget::Ptr _targets;
	
	void applyState(const State::Ptr& state);
	void applyClear(ClearNode* clearNode);
	void drawGeometry(GeoNode* geoNode);
	
	Matrix4 _proj;
	Matrix4 _view;
	Vec3 _camPos;
};

IMPGEARS_END

#endif // IMP_RT_SCENE_RENDERER_H
