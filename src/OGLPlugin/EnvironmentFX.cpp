#include <OGLPlugin/EnvironmentFX.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>
#include <Descriptors/ImageIO.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_env = GLSL_CODE(
uniform sampler2D u_input_sampler_normal;
uniform sampler2D u_input_sampler_depth;
uniform sampler2D u_input_sampler_reflectivity;
uniform samplerCube u_input_cubemap_environment;
uniform vec3 u_light_pos;

varying vec2 v_texCoord;

float i_reflectivity(vec2 uv){return texture2D(u_input_sampler_reflectivity, uv).x;}
vec3 i_normal(vec2 uv){return texture2D(u_input_sampler_normal, uv).xyz;}
float i_depth(vec2 uv){return texture2D(u_input_sampler_depth, uv).x;}
vec3 i_env(vec3 uvw){return texture(u_input_cubemap_environment, uvw).xyz;}

vec3 unproject(vec2 txCoord, float depth)
{
    float near = 0.1;
    float far = 128.0;

    float rad_fov = 3.141592 * 0.5;
    float th_fov = tan(rad_fov * 0.5);

    // float ratio = 1.0; // 4.f/3.f;
    vec2 ndc = txCoord * 2.0 - 1.0;

    vec3 ray = vec3(ndc.x*th_fov, ndc.y*th_fov/*/ratio*/, -1.0);
    float view_depth = near + depth * far;
    vec4 view_pos = vec4(ray * view_depth, 1.0);
    // world_pos /= world_pos.w;

    return view_pos.xyz;
}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    vec3 normal = i_normal(v_texCoord);
    float depth = i_depth(v_texCoord);
    //
    vec3 view_pos = unproject(v_texCoord, depth);

    vec3 n = normal *2.0 - 1.0;
    vec3 view_cam_pos = vec3(0.0);
    vec3 incident = normalize(view_pos-view_cam_pos);

    vec3 r = reflect(incident,n);
    out_color = vec4(i_env(r),1.0);
    // if(length(normal) < 0.01) out_lighting = vec4(vec3(0.0),1.0);
    if(i_reflectivity(v_texCoord) < 0.5) out_color = vec4(vec3(1.0),1.0);

    // --- for debug ---
    // out_color = texture(u_input_cubemap_environment, view_pos);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
EnvironmentFX::EnvironmentFX()
{

}

//--------------------------------------------------------------
EnvironmentFX::~EnvironmentFX()
{

}

//--------------------------------------------------------------
void EnvironmentFX::setup()
{
    Vec4 viewport = Vec4(0.0,0.0,1024.0,1024);
    _graph = buildQuadGraph("glsl_env", glsl_env, viewport);
    _fillingGraph = buildQuadGraph("glsl_fill", s_glsl_fill, viewport);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void EnvironmentFX::apply(GlRenderer* renderer, bool skip)
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

    if(skip)
    {

        _fillingGraph->getInitState()->setUniform("u_fill_color", Vec4(1.0));
        RenderQueue::Ptr queue = renderer->applyRenderVisitor(_fillingGraph);
        renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
    else
    {
        _graph->getInitState()->setUniform("u_input_sampler_normal", _input[0], 0);
        _graph->getInitState()->setUniform("u_input_sampler_depth", _input[1], 1);
        _graph->getInitState()->setUniform("u_input_sampler_reflectivity", _input[2], 2);
        _graph->getInitState()->setUniform("u_input_cubemap_environment", _environment, 3);
        RenderQueue::Ptr queue = renderer->applyRenderVisitor(_graph);
        renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
}


IMPGEARS_END
