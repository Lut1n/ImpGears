#include "ShadowShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
ShadowShader::ShadowShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
ShadowShader::~ShadowShader()
{
}


//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* ShadowShader::vertexCodeSource = IMP_GLSL_SRC(

uniform mat4 u_projection;

varying vec2 v_texCoord;

void main(){
    gl_Position = u_projection * gl_Vertex;
    v_texCoord = vec2(gl_MultiTexCoord0);
    gl_FrontColor = gl_Color;
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* ShadowShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform sampler2D u_shadowBuffer;
uniform sampler2D u_depthSampler;
uniform mat4 u_shadowProjMat;
uniform mat4 u_shadowViewMat;
uniform mat4 u_iviewMat;
uniform float u_far;

vec4 unproject(vec2 txCoord)
{
    float radFov = 60.f * 3.14f / 180.f;
    float thfov = tan(radFov/2.f);

    float whRatio = 4.f/3.f;
    vec2 ndc = txCoord * 2.f - 1.f;

    vec3 viewRay = vec3(ndc.x*thfov, ndc.y*thfov/whRatio, -1.f);
    float viewDepth = texture2D(u_depthSampler, txCoord).x * u_far;
    vec4 viewVec = vec4(viewRay * viewDepth, 1.f);
    //viewVec /= viewVec.w;

    return viewVec;
}

void main(){

    // define poisson disk
    vec2 poissonDisk[4];
    poissonDisk[0] = vec2(-0.94201624, -0.39906216);
    poissonDisk[1] = vec2(0.94558609, -0.76890725);
    poissonDisk[2] = vec2(-0.094184101, -0.92938870);
    poissonDisk[3] = vec2(0.34495938, 0.29387760);

    vec4 modelPosition = u_iviewMat * unproject(v_texCoord);
    vec4 shadowViewPosition = u_shadowViewMat * modelPosition;
    vec4 shadowProjCoord = u_shadowProjMat * shadowViewPosition;
    vec2 shadowCoord = shadowProjCoord.xy / shadowProjCoord.w;
    shadowCoord = (shadowCoord + 1.f)/2.f;

    float visibility = 1.f;
    for(int i=0; i<4; i++)
    {
         if( texture2D(u_shadowBuffer, shadowCoord + poissonDisk[i]/2000.f).z * 512.f < -shadowViewPosition.z - 0.215f )
                visibility -= 0.2f;
    }
    visibility = max(0.f, visibility);

    gl_FragData[0] = vec4(visibility, visibility, visibility, 1.f);
}

);


IMPGEARS_END
