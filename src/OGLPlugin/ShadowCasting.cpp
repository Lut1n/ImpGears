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
uniform int u_sampleCount;

in vec2 v_texCoord;

float i_depth(vec2 uv){return texture(u_input_sampler_depth, uv).x;}
float i_shadow(vec3 uvw){return texture(u_input_cubemap_shadow, uvw).x;}

float hash(vec3 xyz)
{
    vec3 v = vec3(2.19823371,3.27653,1.746541);
    float n = 1545758.2653872;
    float c = dot( xyz,v );
    return fract( sin( c )* n );
}

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

mat3 build_tbn(vec3 n_ref)
{
    vec3 n_z = normalize(n_ref);
    vec3 n_x = vec3(1.0,0.0,0.0);
    vec3 n_y = cross(n_z,n_x);
    vec3 n_x2 = vec3(0.0,1.0,0.0);
    vec3 n_y_2 = cross(n_z,n_x2);
    if(length(n_y) < length(n_y_2)) n_y = n_y_2;

    n_y = normalize(n_y);
    n_x = cross(n_y,n_z); n_x=normalize(n_x);
    mat3 tbn = mat3(n_x,n_y,n_z);
    return tbn;
}

float random(vec3 seed, int i)
{
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    float depth = i_depth(v_texCoord);
    vec3 view_pos = unproject(v_texCoord, depth);
    vec3 world_pos = vec3(inverse(u_view_cam) * vec4(view_pos,1.0));

    float EPSILON = 0.001;
    vec3 pos_from_light = world_pos - u_light_pos;


    vec3 poissonDisk[16] = vec3[](
       vec3( -0.94201624, -0.39906216, 0.0 ),
       vec3( 0.94558609, -0.76890725, 0.0 ),
       vec3( -0.094184101, -0.92938870, 0.0 ),
       vec3( 0.34495938, 0.29387760, 0.0 ),
       vec3( -0.91588581, 0.45771432, 0.0 ),
       vec3( -0.81544232, -0.87912464, 0.0 ),
       vec3( -0.38277543, 0.27676845, 0.0 ),
       vec3( 0.97484398, 0.75648379, 0.0 ),
       vec3( 0.44323325, -0.97511554, 0.0 ),
       vec3( 0.53742981, -0.47373420, 0.0 ),
       vec3( -0.26496911, -0.41893023, 0.0 ),
       vec3( 0.79197514, 0.19090188, 0.0 ),
       vec3( -0.24188840, 0.99706507, 0.0 ),
       vec3( -0.81409955, 0.91437590, 0.0 ),
       vec3( 0.19984126, 0.78641367, 0.0 ),
       vec3( 0.14383161, -0.1410079, 0.00 )
    );

    float near = 0.1;
    float far = 128.0;
    float distance_light = (length(pos_from_light)-near) / (far-near);
    mat3 tbn = build_tbn(pos_from_light);

    int N = u_sampleCount;
    float received_light = 1.0;
    for(int i=0;i<N;++i)
    {
        int index = i;
        // int index = // int(16.0*random(floor(world_pos.xyz*1000.0), i))%16;
        // int index = int(16.0*hash(vec3(v_texCoord, i)))%16;

        float distance_occlusion = i_shadow(pos_from_light + tbn * poissonDisk[index]);
        if(distance_light < 1.0-EPSILON && distance_light > distance_occlusion+EPSILON) received_light -= 0.8/float(N);
    }

    // without multi-sampling
    // float distance_occlusion = i_shadow(pos_from_light);
    // distance_light = (length(pos_from_light)-near) / (50.0-near);
    // if(distance_light < 1.0-EPSILON && distance_light > distance_occlusion+EPSILON) received_light = 0.2;

    out_color = vec4(vec3(received_light),1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
ShadowCasting::ShadowCasting()
{
    _samples = 16;
}

//--------------------------------------------------------------
ShadowCasting::~ShadowCasting()
{

}

//--------------------------------------------------------------
void ShadowCasting::setup(const Vec4& vp)
{
    Vec4 viewport = Vec4(0.0,0.0,vp[2],vp[3]);
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
        if(_fillingQueue == nullptr) _fillingQueue = RenderQueue::create();
        _fillingQueue = renderer->applyRenderVisitor(_fillingGraph,_fillingQueue);
        renderer->drawQueue(_fillingQueue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
    else
    {
        Matrix4 view;
        if(_camera) view = _camera->getViewMatrix();
        Vec3 light_pos; if(_light) light_pos = _light->_worldPosition;

        _graph->getInitState()->setUniform("u_input_sampler_depth", _input[0], 0);
        _graph->getInitState()->setUniform("u_input_cubemap_shadow", _shadows, 1);
        _graph->getInitState()->setUniform("u_light_pos", light_pos );
        _graph->getInitState()->setUniform("u_view_cam", view );
        _graph->getInitState()->setUniform("u_sampleCount", _samples);
        if(_queue == nullptr) _queue = RenderQueue::create();
        _queue = renderer->applyRenderVisitor(_graph,_queue);
        renderer->drawQueue(_queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
}


IMPGEARS_END
