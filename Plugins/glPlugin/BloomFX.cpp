#include "BloomFX.h"
#include "GlRenderer.h"

#include <SceneGraph/QuadNode.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_blur = GLSL_CODE(

uniform float u_horizontal_blur;
uniform sampler2D u_input_sampler1;
uniform sampler2D u_input_sampler2;
varying vec2 v_texCoord;

vec3 i_col(vec2 uv){return texture2D(u_input_sampler1, uv).rgb;}
vec3 i_emi(vec2 uv){return texture2D(u_input_sampler2, uv).rgb;}

void lighting(out vec3 outColor, out vec3 outEmi)
{
    const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    vec2 tex_offset = 1.0 / 512.0;
    vec3 result = i_emi(v_texCoord) * weight[0];

    vec2 dir;
    if(u_horizontal_blur > 0.5) dir = vec2(tex_offset.x, 0.0);
    else dir = vec2(0.0, tex_offset.y);

    for(int i = 1; i < 5; ++i)
    {
        result += i_emi(v_texCoord + dir*i) * weight[i];
        result += i_emi(v_texCoord - dir*i) * weight[i];
    }

    outEmi = clamp(result,0.0,1.0);
    outColor = max(i_col(v_texCoord), clamp(result,0.0,1.0));
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
void BloomFX::setup(int subpassCount, Vec4 viewport)
{
    _subpassCount = subpassCount;

    _shader = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi);
    _shader->_fragCode_lighting = glsl_blur;

    QuadNode::Ptr quad = QuadNode::create();
    quad->setReflexion(_shader);
    Node::Ptr root = quad;

    _graph = Graph::create();
    _graph->setRoot(root);
    _graph->getInitState()->setViewport( viewport );
    _graph->getInitState()->setOrthographicProjection(-1.0,1.0,-1.0,1.0,0.0,1.0);

    Image::Ptr rgb = Image::create(viewport[2],viewport[3],4);
    Image::Ptr rgb2 = Image::create(viewport[2],viewport[3],4);
    ImageSampler::Ptr sampler = ImageSampler::create(rgb);
    ImageSampler::Ptr sampler2 = ImageSampler::create(rgb2);
    _frames[0] = RenderTarget::create();
    _frames[0]->build({ sampler,sampler2 }, true);
    rgb = Image::create(viewport[2],viewport[3],4);
    sampler = ImageSampler::create(rgb);
    sampler2 = ImageSampler::create(rgb2);
    _frames[1] = RenderTarget::create();
    _frames[1]->build({ sampler,sampler2 }, true);
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
    RenderTarget::Ptr src = _frames[prevFrame];
    float h = subpassID%2==0?0.0:1.0;

    _graph->getInitState()->setUniform("u_input_sampler1", src->get(0), 0);
    _graph->getInitState()->setUniform("u_input_sampler2", src->get(1), 1);
    _graph->getInitState()->setUniform("u_horizontal_blur", h);

    renderer->applyRenderVisitor(_graph);
}

//--------------------------------------------------------------
void BloomFX::apply(GlRenderer* renderer, const Graph::Ptr& g)
{
    bind(renderer, 0);
    renderer->applyRenderVisitor(g);

    for(int i=1;i<_subpassCount;++i)
    {
        bind(renderer, i);
        process(renderer, i);
    }

    renderer->_renderPlugin->unbind();
    process(renderer, _subpassCount);
}

IMPGEARS_END
