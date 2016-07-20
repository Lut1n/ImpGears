#include "Graphics/SSAOShader.h"

#include "Core/Vector3.h"
#include "Core/Math.h"

#include "Core/State.h"

#include <cstdio>

IMPGEARS_BEGIN

SSAOShader* SSAOShader::instance = IMP_NULL;

//--------------------------------------------------------------
SSAOShader::SSAOShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
    instance = this;
    initRandom();

    for(Uint32 i=0; i<IMP_SSAO_SAMPLE_KERNEL_SIZE; ++i)
    {
        Vector3 v(random(-1.f, 1.f), random(-1.f, 1.f), random(0.f, 1.f));

        v.normalize();
        float scale = (float)i/IMP_SSAO_SAMPLE_KERNEL_SIZE;
        scale = lerp(0.1f, 1.0f, scale*scale);
        v *= scale;

        m_sampleKernel[i*3] = v.getX();
        m_sampleKernel[i*3+1] = v.getY();
        m_sampleKernel[i*3+2] = v.getZ();
    }

    m_noise.create(4, 4);
    PixelData pixels;
    m_noise.getPixelData(&pixels);
    for(Uint32 i=0; i<4; ++i)
    {
        for(Uint32 j=0; j<4; ++j)
        {
            Vector3 v(random(-1.f, 1.f), random(-1.f, 1.f), 0.f);
            v.normalize();
            Pixel pix;
            pix.red = ((v.getX() +1.f)/2.f)*255;
            pix.green = ((v.getY()+1.f)/2.f)*255;
            pix.blue = ((v.getZ()+1.f)/2.f)*255;;
            pix.alpha = 255;
            pixels.setPixel(i,j, pix);
        }
    }
    m_noise.loadFromPixelData(&pixels);
}

//--------------------------------------------------------------
SSAOShader::~SSAOShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* SSAOShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform mat4 u_projection;

void main(){

    gl_Position = u_projection * gl_Vertex;
    gl_FrontColor = gl_Color;
    v_texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* SSAOShader::fragmentCodeSource = IMP_GLSL_SRC(

uniform sampler2D depthTex;
uniform sampler2D noiseTex;
uniform sampler2D normalTex;
uniform mat4 pMat;
uniform mat4 vMat;
uniform mat4 ivMat;
uniform float u_near;
uniform float u_far;
uniform vec3 sampleKernel[16];
uniform float u_sampleCount;

varying vec2 v_texCoord;

vec4 unproject(vec2 txCoord)
{
    float radFov = 60.f * 3.14f / 180.f;
    float thfov = tan(radFov/2.f);

    float whRatio = 4.f/3.f;
    vec2 ndc = txCoord * 2.f - 1.f;

    vec3 viewRay = vec3(ndc.x*thfov, ndc.y*thfov/whRatio, -1.f);
    float viewDepth = texture2D(depthTex, txCoord).x * u_far;
    vec4 viewVec = vec4(viewRay * viewDepth, 1.f);
    //viewVec /= viewVec.w;

    return viewVec;
}

void main(){

    // get normal
    vec3 normal = texture2D(normalTex, v_texCoord).xyz * 2.f - 1.f;
    normal = normalize(normal);

    // get depth
    float originDepth = texture2D(depthTex, v_texCoord).x;

    // get origin (in model view space)
    vec4 origin = unproject(v_texCoord);


    // build tbn matrix (tangent space)
    vec2 noiseScale = vec2(800.f/4.f, 600.f/4.f);
    vec3 rotationVec = texture2D(noiseTex, v_texCoord*noiseScale).xyz * 2.f - 1.f;
    vec3 tangent = normalize(rotationVec - normal * dot(rotationVec, normal));
    vec3 bitangent = normalize(cross(normal, tangent));
    mat3 tbn = mat3(tangent, bitangent, normal);

    float sampleRadius = 3.f;

    float occlusion = 0.0f;
    for(int i=0; i<int(u_sampleCount); ++i)
    {
        // get sample position
        vec3 sample = tbn * sampleKernel[i];
        vec4 sample4 = vMat * vec4(sample, 0.f); //vMat for only rotation
        sample = sample4.xyz * sampleRadius + origin.xyz;

        // project sample position
        vec4 offset = vec4(sample, 1.f);
        offset = pMat * offset;
        offset.xy /= offset.w;
        offset.xy = (offset.xy + 1.f) / 2.f;

        // get sample depth
        float sampleDepth = texture2D(depthTex, offset.xy).x;

        // range check and acc
        float rangeCheck = abs(originDepth - sampleDepth)*u_far < sampleRadius ? 1.f : 0.f;
        occlusion += (sampleDepth*u_far <= -sample.z ? 1.f : 0.f) * rangeCheck;

    }

    occlusion = 1.f - (occlusion/u_sampleCount);

    gl_FragData[0] = vec4(occlusion, occlusion, occlusion, 1.f);
}

);

/*
source :
http://www.geeks3d.com/20091216/geexlab-how-to-visualize-the-depth-buffer-in-glsl/

In a perspective projection, the z buffer value is non-linear in eye space: in short,
depth values are proportional to the reciprocal of the z value is eye space
(see http://www.sjbaker.org/steve/omniv/love_your_z_buffer.html).

There is more precision close to the camera (or eye) and less precision far from the eye.

But z values are linear in screen space (see http://www.humus.name/index.php?ID=255).
*/


IMPGEARS_END
