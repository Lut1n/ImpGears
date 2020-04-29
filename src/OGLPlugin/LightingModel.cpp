#include <OGLPlugin/LightingModel.h>
#include <OGLPlugin/GlRenderer.h>

#include <ImpGears/SceneGraph/QuadNode.h>
#include <ImpGears/Descriptors/ImageIO.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_lightingModel = GLSL_CODE(
uniform sampler2D u_input_sampler_normal;
uniform sampler2D u_input_sampler_depth;
uniform sampler2D u_input_sampler_shininess;
uniform vec3 u_light_pos;
uniform float u_light_power;
uniform float u_ambient;

in vec2 v_texCoord;

vec3 i_normal(vec2 uv){return texture(u_input_sampler_normal, uv).xyz;}
float i_depth(vec2 uv){return texture(u_input_sampler_depth, uv).x;}
float i_shininess(vec2 uv){return texture(u_input_sampler_shininess, uv).x;}

vec3 unproject(vec2 txCoord, float depth)
{
    float near = 0.1;
    float far = 128.0;

    float rad_fov = 3.141592 * 0.5;
    float th_fov = tan(rad_fov * 0.5);

    // float ratio = 1.0; // 4.f/3.f;
    vec2 ndc = txCoord * 2.0 - 1.0;

    vec3 ray = vec3(ndc.x*th_fov, ndc.y*th_fov/*/ratio*/, -1.0);
    ray = normalize(ray);
    float view_depth = near + depth * (far-near);
    vec4 view_pos = vec4(ray * view_depth, 1.0);

    return view_pos.xyz;
}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    vec3 normal = i_normal(v_texCoord) *2.0 - 1.0;
    float depth = i_depth(v_texCoord);

    vec3 view_pos = unproject(v_texCoord, depth);

    float lightPower = u_light_power; // 200.0;
    float shininess = i_shininess(v_texCoord) * 100.0;

    // view space
    vec3 light_dir = u_light_pos - view_pos;
    float dist = length(light_dir);
    dist = dist * dist;
    light_dir = normalize(light_dir);

    float lambertian = max(dot(light_dir,normal),0.0);
    float specular = 0.0;

    // blinn phong
    if(lambertian > 0.0)
    {
        // view dir
        vec3 view_dir=normalize(-view_pos);

        // blinn phong
        vec3 halfDir = normalize(light_dir + view_dir);
        float specAngle = max( dot(halfDir,normal), 0.0 );
        specular = pow(specAngle, shininess);
    }

    float intensity = clamp(lightPower/dist,0.0,1.0);
    float ambient = u_ambient; // 0.3;
    float lighting = (lambertian+specular)*intensity;
    lighting = mix(ambient,1.0,clamp(lighting,0.0,1.0));

    const float EPSILON = 0.001;
    if( length(i_normal(v_texCoord)) < EPSILON ) lighting = 0.0;

    out_color = vec4(vec3(lighting),1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
LightingModel::LightingModel()
{
    _lightpower = 200.0;
    _ambient = 0.1;
}

//--------------------------------------------------------------
LightingModel::~LightingModel()
{

}

//--------------------------------------------------------------
void LightingModel::setup(const Vec4& vp)
{
    Vec4 viewport = Vec4(0.0,0.0,vp[2],vp[3]);
    _graph = buildQuadGraph("glsl_lightingModel", glsl_lightingModel, viewport);
    _fillingGraph = buildQuadGraph("glsl_fill", s_glsl_fill, viewport);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void LightingModel::apply(GlRenderer* renderer, bool skip)
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
        if(_fillingQueue == nullptr) _fillingQueue = RenderQueue::create();
        _fillingQueue = renderer->applyRenderVisitor(_fillingGraph,_fillingQueue);
        renderer->drawQueue(_fillingQueue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
    else
    {
        Vec4 light_pos; if(_light) light_pos = Vec4(_light->_worldPosition,1.0);
        if(_camera) light_pos = light_pos * _camera->getViewMatrix();

        _graph->getInitState()->setUniform("u_input_sampler_normal", _input[0], 0);
        _graph->getInitState()->setUniform("u_input_sampler_depth", _input[1], 1);
        _graph->getInitState()->setUniform("u_input_sampler_shininess", _input[2], 2);
        _graph->getInitState()->setUniform("u_light_pos", Vec3(light_pos) );
        _graph->getInitState()->setUniform("u_light_power", _lightpower );
        _graph->getInitState()->setUniform("u_ambient", _ambient );
        if(_queue == nullptr) _queue = RenderQueue::create();
        _queue = renderer->applyRenderVisitor(_graph,_queue);
        renderer->drawQueue(_queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
}


IMPGEARS_END
