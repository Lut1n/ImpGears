#include <OGLPlugin/CopyFrame.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_copy = GLSL_CODE(

uniform sampler2D u_input_sampler;
varying vec2 v_texCoord;

vec4 i_col(vec2 uv){return texture2D(u_input_sampler, uv).rgbw;}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = i_col(v_texCoord);
}

);


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
void CopyFrame::setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output)
{
    _input = input;
    _output = output;

    Vec4 viewport = Vec4(0.0,0.0,512.0,512);
    _graph = buildQuadGraph("glsl_copy", glsl_copy, viewport);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void CopyFrame::apply(GlRenderer* renderer)
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
    renderer->applyRenderVisitor(_graph, nullptr, SceneRenderer::RenderFrame_Lighting);
}


IMPGEARS_END
