#include <RendererGL/CopyFrame.h>
#include <RendererGL/GlRenderer.h>

#include <ImpGears/SceneGraph/QuadNode.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
CopyFrame::CopyFrame()
{

}

//--------------------------------------------------------------
CopyFrame::~CopyFrame()
{

}

//--------------------------------------------------------------
void CopyFrame::setup(const Vec4& vp)
{
    _graph = buildQuadGraph("glsl_copy", getCopyGlsl(), vp);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void CopyFrame::apply(GlRenderer* renderer, bool /*skip*/)
{
    if(_output.size() > 0)
    {
        renderer->_renderPlugin->init(_frame);
        renderer->_renderPlugin->bind(_frame);
        _frame->change();
    }
    else
    {
        renderer->_renderPlugin->unbind();
    }
    _graph->getInitState()->setUniform("u_input_sampler", _input[0], 0);
    if(_queue == nullptr) _queue = RenderQueue::create();
    _queue = renderer->applyRenderVisitor(_graph,_queue);
    renderer->drawQueue(_queue, nullptr, SceneRenderer::RenderFrame_Lighting);
}


IMPGEARS_END
