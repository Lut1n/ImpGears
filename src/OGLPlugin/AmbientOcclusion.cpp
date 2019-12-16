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

varying vec2 v_texCoord;

vec3 i_normal(vec2 uv){return texture2D(u_input_sampler_normal, uv).xyz;}
float i_depth(vec2 uv){return texture2D(u_input_sampler_depth, uv).x;}

float hash(vec3 xyz)
{
        vec3 v = vec3(2.19823371,3.27653,1.746541);
        float n = 1545758.2653872;
        float c = dot( xyz,v );
        return fract( sin( c )* n );
}

vec3 noise(vec2 txCoord)
{
    vec3 x_seed = vec3(txCoord, 0.274523);
    vec3 y_seed = vec3(txCoord, 0.336841);
    vec3 z_seed = vec3(txCoord, 0.497234);

    return vec3(hash(x_seed),hash(y_seed),hash(z_seed) * 0.5 + 0.5);
}

vec3 kernel_simu(vec2 txCoord, int index)
{
    const float KERNEL_SIZE = 16.0;

    // vec2 seed = vec2(0.27584, 0.384854) * vec2(index);
    vec2 seed = txCoord * vec2(index);
    vec3 sample = noise(seed);
    sample = normalize(sample);

    float scale = float(index)/KERNEL_SIZE;
    scale = mix(0.1f, 1.0f, scale*scale);
    sample *= scale;

    return sample;
}

vec4 unproject(vec2 txCoord, float depth)
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
    // world_pos /= world_pos.w;

    return view_pos;
}

mat3 build_tbn(vec3 n_ref)
{
    vec3 n_z = n_ref; // normalize();
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



    // build tbn matrix (tangent space)
    vec3 rotationVec = noise(v_texCoord); rotationVec.z = 0.0;
    rotationVec = normalize(rotationVec);
    rotationVec = rotationVec * 2.0 - 1.0;

    // vec3 tangent = normalize(rotationVec - normal * dot(rotationVec, normal));
    // vec3 bitangent = normalize(cross(normal, tangent));
    // mat3 tbn = mat3(tangent, bitangent, normal);

    mat3 tbn = build_tbn(normal);

    float sampleRadius = 3.0;

    float occlusion = 0.0f;
    const float KERNEL_SIZE = 16.0;
    for(int i=0; i<KERNEL_SIZE; ++i)
    {
        // get sample position
        vec3 sample = tbn * kernel_simu(v_texCoord, i);
        // vec4 sample4 = u_cam_view * vec4(sample, 0.0); //u_cam_view for only rotation
        // sample = sample4.xyz * sampleRadius + view_origin.xyz;
        sample = sample*sampleRadius + view_origin.xyz;

        // project sample position
        vec4 offset = vec4(sample, 1.0);
        offset = u_scene_proj * offset;
        // offset /= -offset.z;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        // get sample depth
        float depth_sample = i_depth(offset.xy).x;

        // range check and acc
        float rangeCheck = abs(depth_origin - depth_sample)*(far) < sampleRadius ? 1.0 : 0.0;
        occlusion += (depth_sample*far <= -sample.z ? 1.0 : 0.0) * rangeCheck;

    }

    occlusion = 1.0 - (occlusion/16.0);

    out_color = vec4(occlusion, occlusion, occlusion, 1.0);

    // --- for debug ---
    // out_color = vec4(vec3(distance_light),1.0);
    // out_color = vec4(vec3(distance_occlusion),1.0);
    // out_color = texture(u_input_cubemap_shadow, view_pos);
    // out_color = texture(u_input_cubemap_shadow, pos_from_light);
    // out_color = vec4(1.0,0.0,0.0,1.0);

    // out_color = vec4(noise(v_texCoord),1.0);
    // out_color = vec4(kernel_simu(15),1.0);
    // out_color = vec4(vec3(depth_origin),1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
AmbientOcclusion::AmbientOcclusion()
{

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
        RenderQueue::Ptr queue = renderer->applyRenderVisitor(_fillingGraph);
        renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
    else
    {
        Matrix4 view;
        if(_camera) view = _camera->getViewMatrix();

        _graph->getInitState()->setUniform("u_input_sampler_normal", _input[0], 0);
        _graph->getInitState()->setUniform("u_input_sampler_depth", _input[1], 1);
        _graph->getInitState()->setUniform("u_cam_view", view);
        _graph->getInitState()->setUniform("u_scene_proj", _proj);
        RenderQueue::Ptr queue = renderer->applyRenderVisitor(_graph);
        renderer->drawQueue(queue, nullptr, SceneRenderer::RenderFrame_Lighting);
    }
}


IMPGEARS_END
