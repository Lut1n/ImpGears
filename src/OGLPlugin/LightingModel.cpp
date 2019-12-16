#include <OGLPlugin/LightingModel.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>
#include <Descriptors/ImageIO.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_lightingModel = GLSL_CODE(
// uniform sampler2D u_input_sampler_color;
uniform sampler2D u_input_sampler_normal;
uniform sampler2D u_input_sampler_depth;
uniform sampler2D u_input_sampler_shininess;
uniform vec3 u_light_pos;

varying vec2 v_texCoord;

// vec3 i_color(vec2 uv){return texture2D(u_input_sampler_color, uv).xyz;}
vec3 i_normal(vec2 uv){return texture2D(u_input_sampler_normal, uv).xyz;}
float i_depth(vec2 uv){return texture2D(u_input_sampler_depth, uv).x;}
float i_shininess(vec2 uv){return texture2D(u_input_sampler_shininess, uv).x;}

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
    // vec3 color = i_color(v_texCoord);
    vec3 normal = i_normal(v_texCoord);
    float depth = i_depth(v_texCoord);

    vec3 n = normal *2.0 - 1.0;
    vec3 view_cam_pos = vec3(0.0);

    vec3 view_pos = unproject(v_texCoord, depth);


    // vec3 lightColor = vec3(1.0);
    float lightPower = 200.0; // u_lightAtt[0];
    float shininess = i_shininess(v_texCoord) * 10.0; // u_lightAtt[1];

    // view space
    vec3 light_dir = u_light_pos - view_pos;
    float dist = length(light_dir);
    dist = dist * dist;
    light_dir = normalize(light_dir);

    // mat3 tbn = build_tbn( normalize(v_n) );
    // mat3 inv_tbn = inverse( tbn );

    float lambertian = max(dot(light_dir,n),0.0);
    float specular = 0.0;

    // blinn phong
    if(lambertian > 0.0)
    {
        // view dir
        vec3 view_dir=normalize(-view_pos);

        // blinn phong
        vec3 halfDir = light_dir + view_dir;
        float specAngle = max( dot(halfDir,n), 0.0 );
        specular = pow(specAngle, shininess);
    }

    // Color model :
    // ambiantColor
    // + diffColor * lambertian * lightColor * lightPower / distance
    // + specColor * specular * lightColor * lightPower / distance

    float ambient = 0.01;
    float colorModel = ambient
            + 0.7 * lambertian * /*lightColor **/ lightPower / dist
            + 0.3 * specular * /*lightColor **/ lightPower / dist;

    const float EPSILON = 0.001;
    if( length(normal) < EPSILON ) colorModel = 0.0;

    out_color = vec4(vec3(colorModel),1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
LightingModel::LightingModel()
{

}

//--------------------------------------------------------------
LightingModel::~LightingModel()
{

}

//--------------------------------------------------------------
void LightingModel::setup()
{
    Vec4 viewport = Vec4(0.0,0.0,1024.0,1024);
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
        RenderQueue::Ptr queue = renderer->applyRenderVisitor(_fillingGraph);
        renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
    else
    {
        Vec4 light_pos = Vec4(_light->_worldPosition);
        if(_camera) light_pos = light_pos * _camera->getViewMatrix();

        _graph->getInitState()->setUniform("u_input_sampler_normal", _input[0], 0);
        _graph->getInitState()->setUniform("u_input_sampler_depth", _input[1], 1);
        _graph->getInitState()->setUniform("u_input_sampler_shininess", _input[2], 2);
        _graph->getInitState()->setUniform("u_light_pos", Vec3(light_pos) );
        RenderQueue::Ptr queue = renderer->applyRenderVisitor(_graph);
        renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
}


IMPGEARS_END
