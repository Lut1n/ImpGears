#include <OGLPlugin/BlendAll.h>
#include <OGLPlugin/GlRenderer.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_mult = GLSL_CODE(

uniform sampler2D u_input_sampler_lighting;
uniform sampler2D u_input_sampler_bloom;
in vec2 v_texCoord;

vec4 i_lighting(vec2 uv){return texture(u_input_sampler_lighting, uv).xyzw;}
vec4 i_bloom(vec2 uv){return texture(u_input_sampler_bloom, uv).xyzw;}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = vec4(i_lighting(v_texCoord).xyz * i_bloom(v_texCoord).xyz, 1.0);
}

);

//--------------------------------------------------------------
static std::string glsl_min = GLSL_CODE(

uniform sampler2D u_input_sampler_lighting;
uniform sampler2D u_input_sampler_bloom;
in vec2 v_texCoord;

vec4 i_lighting(vec2 uv){return texture(u_input_sampler_lighting, uv).xyzw;}
vec4 i_bloom(vec2 uv){return texture(u_input_sampler_bloom, uv).xyzw;}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = vec4(min(i_lighting(v_texCoord).xyz, i_bloom(v_texCoord).xyz),1.0);
}

);

//--------------------------------------------------------------
static std::string glsl_max = GLSL_CODE(

uniform sampler2D u_input_sampler_lighting;
uniform sampler2D u_input_sampler_bloom;
in vec2 v_texCoord;

vec4 i_lighting(vec2 uv){return texture(u_input_sampler_lighting, uv).xyzw;}
vec4 i_bloom(vec2 uv){return texture(u_input_sampler_bloom, uv).xyzw;}
void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = vec4(max(i_lighting(v_texCoord).xyz, i_bloom(v_texCoord).xyz),1.0);
}

);

//--------------------------------------------------------------
static std::string glsl_mix = GLSL_CODE(

uniform sampler2D u_input_sampler_lighting;
uniform sampler2D u_input_sampler_bloom;
in vec2 v_texCoord;

vec4 i_lighting(vec2 uv){return texture(u_input_sampler_lighting, uv).xyzw;}
vec4 i_bloom(vec2 uv){return texture(u_input_sampler_bloom, uv).xyzw;}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = vec4(mix(i_lighting(v_texCoord).xyz, i_bloom(v_texCoord).xyz, 0.5), 1.0);
}

);

IMPGEARS_BEGIN

//--------------------------------------------------------------
BlendAll::BlendAll(Type t)
    : _type(t)
{

}

//--------------------------------------------------------------
BlendAll::~BlendAll()
{

}

//--------------------------------------------------------------
void BlendAll::setup(const Vec4& vp)
{
    Vec4 viewport = Vec4(0.0,0.0,vp[2],vp[3]);
    if(_type == Max)
        _graph = buildQuadGraph("glsl_max", glsl_max, viewport);
    else if(_type == Mix)
        _graph = buildQuadGraph("glsl_mix", glsl_mix, viewport);
    else if(_type == Min)
        _graph = buildQuadGraph("glsl_min", glsl_min, viewport);
    else if(_type == Mult)
        _graph = buildQuadGraph("glsl_mult", glsl_mult, viewport);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void BlendAll::apply(GlRenderer* renderer, bool /*skip*/)
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
    if(_queue == nullptr) _queue = RenderQueue::create();
    _queue = renderer->applyRenderVisitor(_graph,_queue);
    renderer->drawQueue(_queue, nullptr, SceneRenderer::RenderFrame_Bloom);
}


IMPGEARS_END
