#include <Renderer/SceneRenderer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneRenderer::SceneRenderer()
{
	_visitor = RenderVisitor::create();
	_direct = true;
}

//--------------------------------------------------------------
SceneRenderer::~SceneRenderer()
{
}

IMPGEARS_END
