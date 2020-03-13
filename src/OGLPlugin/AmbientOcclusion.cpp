#include <OGLPlugin/AmbientOcclusion.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>
#include <Descriptors/ImageIO.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_ssao = GLSL_CODE(
uniform sampler2D u_input_sampler_normal;
uniform sampler2D u_input_sampler_depth;
uniform mat4 u_cam_view;
uniform mat4 u_scene_proj;
uniform int u_sampleCount;

in vec2 v_texCoord;

vec3 i_normal(vec2 uv){return texture(u_input_sampler_normal, uv).xyz;}
float i_depth(vec2 uv){return texture(u_input_sampler_depth, uv).x;}

float hash(vec3 xyz)
{
    vec3 v = vec3(2.19823371,3.27653,1.746541);
    float n = 1545758.2653872;
    float c = dot( xyz,v );
    return fract( sin( c )* n );
}

float random(vec3 seed, int i)
{
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

vec3 unproject(vec2 txCoord, float depth)
{
    float near = 0.1;
    float far = 128.0;

    float rad_fov = 3.141592 * 0.5;
    float th_fov = tan(rad_fov * 0.5);

    // float ratio = 1.0; // 4.0/3.0;
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

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    float near = 0.1;
    float far = 128.0;

    vec3 normal = i_normal(v_texCoord) * 2.0 - 1.0;
    float depth_origin = i_depth(v_texCoord);
    vec3 view_origin = unproject(v_texCoord, depth_origin);
    depth_origin = near + depth_origin * (far-near);


    mat3 tbn = build_tbn(normal);

    float sampleRadius = 3.0;

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
       vec3( 0.14383161, -0.1410079, 0.0 )
    );

    float occlusion = 0.0f;
    int KERNEL_SIZE = u_sampleCount; // 16;
    for(int i=0; i<KERNEL_SIZE; ++i)
    {
        int index = i;
        // int index = int(16.0*random(view_origin, i))%16;
        // int index = int(16.0*hash(vec3(v_texCoord, i)))%16;

        // get sample position
        vec3 sample3 = poissonDisk[index];
        sample3 = normal*2.0 + tbn * sample3;
        sample3 = sampleRadius * normalize(sample3) + view_origin;

        // project sample position
        vec4 offset = vec4(sample3, 1.0);
        offset = u_scene_proj * offset;
        offset /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        // get sample depth
        float depth_sample = near + i_depth(offset.xy) * (far-near);

        // range check and acc
        float rangeCheck = abs(depth_origin - depth_sample) < sampleRadius ? 1.0 : 0.0;
        occlusion += (depth_sample <= length(sample3) ? 1.0 : 0.0) * rangeCheck;

    }

    occlusion = 1.0 - (occlusion/float(KERNEL_SIZE));

    out_color = vec4(occlusion, occlusion, occlusion, 1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
AmbientOcclusion::AmbientOcclusion()
{
    _samples = 16;
}

//--------------------------------------------------------------
AmbientOcclusion::~AmbientOcclusion()
{

}

//--------------------------------------------------------------
void AmbientOcclusion::setup(const Vec4& vp)
{
    Vec4 viewport = Vec4(0.0,0.0,vp[2],vp[3]);
    _graph = buildQuadGraph("glsl_ssao", glsl_ssao, viewport);
    _fillingGraph = buildQuadGraph("glsl_fill", s_glsl_fill, viewport);

    _proj = Matrix4::perspectiveProj(90.0, 1.0, 0.1, 128.0);

    if(_output.size() > 0)
    {
        _frame = RenderTarget::create();
        _frame->build(_output, true);
    }
}

//--------------------------------------------------------------
void AmbientOcclusion::apply(GlRenderer* renderer, bool skip)
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
        if(_fillingQueue==nullptr)_fillingQueue = RenderQueue::create();
        _fillingQueue = renderer->applyRenderVisitor(_fillingGraph,_fillingQueue);
        renderer->drawQueue(_fillingQueue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
    else
    {
        Matrix4 view;
        if(_camera) view = _camera->getViewMatrix();

        _graph->getInitState()->setUniform("u_input_sampler_normal", _input[0], 0);
        _graph->getInitState()->setUniform("u_input_sampler_depth", _input[1], 1);
        _graph->getInitState()->setUniform("u_cam_view", view);
        _graph->getInitState()->setUniform("u_scene_proj", _proj);
        _graph->getInitState()->setUniform("u_sampleCount", _samples);
        if(_queue==nullptr)_queue  =RenderQueue::create();
        _queue= renderer->applyRenderVisitor(_graph,_queue);
        renderer->drawQueue(_queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
}


IMPGEARS_END
