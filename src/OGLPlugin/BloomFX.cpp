#include <OGLPlugin/BloomFX.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_blur = GLSL_CODE(

uniform float u_horizontal_blur;
uniform sampler2D u_input_sampler;
varying vec2 v_texCoord;

vec4 i_emi(vec2 uv){return texture2D(u_input_sampler, uv).rgbw;}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    vec2 tex_offset = 1.0 / 1024.0;
    vec4 result = i_emi(v_texCoord) * weight[0];

    vec2 dir;
    if(u_horizontal_blur > 0.5) dir = vec2(tex_offset.x, 0.0);
    else dir = vec2(0.0, tex_offset.y);

    for(int i = 1; i < 5; ++i)
    {
        result += i_emi(v_texCoord + dir*i) * weight[i];
        result += i_emi(v_texCoord - dir*i) * weight[i];
    }

    result = clamp(result,0.0,1.0);
    out_color = result;
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
BloomFX::BloomFX()
{

}

//--------------------------------------------------------------
BloomFX::~BloomFX()
{

}

//--------------------------------------------------------------
void BloomFX::setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output)
{
    _input = input;
    _output = output;

    _subpassCount = 10;
    Vec4 viewport = Vec4(0.0,0.0,1024.0,1024);
    _graph = buildQuadGraph("glsl_blur", glsl_blur, viewport);

    _frames[0] = RenderTarget::create();
    _frames[0]->build(viewport[2],viewport[3],1, true);
    _frames[1] = RenderTarget::create();
    _frames[1]->build(_output, true);
}

//--------------------------------------------------------------
void BloomFX::bind(GlRenderer* renderer, int subpassID)
{
    int frameID = (subpassID%2);
    renderer->_renderPlugin->init(_frames[frameID]);
    renderer->_renderPlugin->bind(_frames[frameID]);
    _frames[frameID]->change();
}

//--------------------------------------------------------------
void BloomFX::process(GlRenderer* renderer, int subpassID)
{
    int prevFrame =(subpassID-1)%2;
    ImageSampler::Ptr sampler = _frames[prevFrame]->get(0);
    float h = subpassID%2==0?0.0:1.0;

    _graph->getInitState()->setUniform("u_input_sampler", sampler, 0);
    _graph->getInitState()->setUniform("u_horizontal_blur", h);

    RenderQueue::Ptr queue = renderer->applyRenderVisitor(_graph);
    renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Bloom);
}

//--------------------------------------------------------------
void BloomFX::apply(GlRenderer* renderer)
{
    bind(renderer, 0);
    _graph->getInitState()->setUniform("u_input_sampler", _input[0], 0);
    _graph->getInitState()->setUniform("u_horizontal_blur", float(0.0));
    RenderQueue::Ptr queue = renderer->applyRenderVisitor(_graph);
    renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Bloom);

    for(int i=1;i<_subpassCount;++i)
    {
        bind(renderer, i);
        process(renderer, i);
    }
}


IMPGEARS_END
