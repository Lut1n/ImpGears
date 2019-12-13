#include <OGLPlugin/ShadowCasting.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>
#include <Descriptors/ImageIO.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_shadowCasting = GLSL_CODE(
uniform sampler2D u_input_sampler_depth;
uniform samplerCube u_input_cubemap_shadow;
uniform vec3 u_light_pos;
uniform mat4 u_view_cam;

varying vec2 v_texCoord;

float i_depth(vec2 uv){return texture2D(u_input_sampler_depth, uv).x;}
float i_shadow(vec3 uvw){return texture(u_input_cubemap_shadow, uvw).x;}

vec3 unproject(vec2 txCoord, float depth)
{
    float near = 0.1;
    float far = 128.0;

    float rad_fov = 3.141592 * 0.5;
    float th_fov = tan(rad_fov * 0.5);

    // float ratio = 1.0; // 4.f/3.f;
    vec2 ndc = txCoord * 2.0 - 1.0;

    vec3 ray = vec3(ndc.x*th_fov, ndc.y*th_fov/*/ratio*/, -1.0);
    float view_depth = near + depth * (far-near);
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
    float depth = i_depth(v_texCoord);
    //
    vec3 view_pos = unproject(v_texCoord, depth);
    vec3 world_pos = vec3(inverse(u_view_cam) * vec4(view_pos,1.0));

    //
    float EPSILON = 0.001;
    vec3 pos_from_light = world_pos - u_light_pos;
    // vec3 pos_from_light = view_pos - u_light_pos;

    float near = 0.1;
    float far = 128.0;
    float distance_light = (length(pos_from_light)-near) / (far-near);

    float distance_occlusion = i_shadow(pos_from_light);
    //
    float received_light = 1.0;
    if(distance_light > distance_occlusion+EPSILON) received_light = 0.1;

    out_color = vec4(vec3(received_light),1.0);

    // --- for debug ---
    // out_color = vec4(vec3(distance_light),1.0);
    // out_color = vec4(vec3(distance_occlusion),1.0);
    // out_color = texture(u_input_cubemap_shadow, view_pos);
    // out_color = texture(u_input_cubemap_shadow, pos_from_light);
    // out_color = vec4(1.0,0.0,0.0,1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
ShadowCasting::ShadowCasting()
{

}

//--------------------------------------------------------------
ShadowCasting::~ShadowCasting()
{

}

//--------------------------------------------------------------
void ShadowCasting::setup()
{
    Vec4 viewport = Vec4(0.0,0.0,1024.0,1024);
    _graph = buildQuadGraph("glsl_shadowCasting", glsl_shadowCasting, viewport);
    _fillingGraph = buildQuadGraph("glsl_fill", s_glsl_fill, viewport);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void ShadowCasting::apply(GlRenderer* renderer, bool skip)
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
        Matrix4 view;
        if(_camera) view = _camera->getViewMatrix();

        _graph->getInitState()->setUniform("u_input_sampler_depth", _input[0], 0);
        _graph->getInitState()->setUniform("u_input_cubemap_shadow", _shadows, 1);
        _graph->getInitState()->setUniform("u_light_pos", _light->_worldPosition );
        _graph->getInitState()->setUniform("u_view_cam", view );
        RenderQueue::Ptr queue = renderer->applyRenderVisitor(_graph);
        renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
}


IMPGEARS_END
