#include <OGLPlugin/BlendAll.h>
#include <OGLPlugin/GlRenderer.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_blend = GLSL_CODE(

uniform sampler2D u_input_sampler_lighting;
uniform sampler2D u_input_sampler_bloom;
varying vec2 v_texCoord;

vec4 i_lighting(vec2 uv){return texture2D(u_input_sampler_lighting, uv).rgbw;}
vec4 i_bloom(vec2 uv){return texture2D(u_input_sampler_bloom, uv).rgbw;}

void lighting(out vec4 out_lighting,
              out vec4 out_emissive,
              out vec3 out_position,
              out vec3 out_normal,
              out float out_metalness,
              out float out_depth)
{
    out_lighting = max(i_lighting(v_texCoord), i_bloom(v_texCoord));
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
BlendAll::BlendAll()
{

}

//--------------------------------------------------------------
BlendAll::~BlendAll()
{

}

//--------------------------------------------------------------
void BlendAll::setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output)
{
    _input = input;
    _output = output;

    Vec4 viewport = Vec4(0.0,0.0,512.0,512);
    _graph = buildQuadGraph(glsl_blend, viewport);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void BlendAll::apply(GlRenderer* renderer)
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
    _graph->getInitState()->setUniform("u_input_sampler_lighting", _input[0], 0);
    _graph->getInitState()->setUniform("u_input_sampler_bloom", _input[1], 1);
    renderer->applyRenderVisitor(_graph, nullptr, SceneRenderer::RenderFrame_Bloom);
}


IMPGEARS_END
