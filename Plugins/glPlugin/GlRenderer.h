#ifndef IMP_GL_RENDERER_H
#define IMP_GL_RENDERER_H

#include <Renderer/RenderTarget.h>

#include <Plugins/RenderPlugin.h>

#include <SceneGraph/ClearNode.h>
#include <SceneGraph/GeoNode.h>

IMPGEARS_BEGIN

class IMP_API GlRenderer : public SceneRenderer
{
public:

    Meta_Class(GlRenderer)
	
	GlRenderer();
	virtual ~GlRenderer();

    void applyRenderVisitor(const Graph::Ptr& scene);

	virtual void render(const Graph::Ptr& scene);

    virtual Image::Ptr getTarget(bool dlFromGPU = false, int id = 0);
	
	void loadRenderPlugin(const std::string& renderPlugin);
	void setRenderPlugin(RenderPlugin* plugin);
	
	RenderPlugin* _renderPlugin;

protected:

    RenderTarget::Ptr _renderTargets;
	
	void applyState(const State::Ptr& state);
	void applyClear(ClearNode* clearNode);
    void drawGeometry(GeoNode* geoNode);
};

IMPGEARS_END

#endif // IMP_GL_RENDERER_H
