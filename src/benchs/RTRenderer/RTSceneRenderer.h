#ifndef IMP_RT_SCENE_RENDERER_H
#define IMP_RT_SCENE_RENDERER_H

#include <ImpGears/Renderer/SceneRenderer.h>
#include <ImpGears/Renderer/RenderTarget.h>

#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/ClearNode.h>
#include <ImpGears/SceneGraph/GeoNode.h>

IMPGEARS_BEGIN

class RTSceneRenderer : public SceneRenderer
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
