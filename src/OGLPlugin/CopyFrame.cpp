#include <OGLPlugin/CopyFrame.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>

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
void CopyFrame::setup()
{
    Vec4 viewport = Vec4(0.0,0.0,512.0,512);
    _graph = buildQuadGraph("glsl_copy", s_glsl_copy, viewport);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void CopyFrame::apply(GlRenderer* renderer, bool skip)
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
    RenderQueue::Ptr queue = renderer->applyRenderVisitor(_graph);
    renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
}


IMPGEARS_END
