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

varying vec2 v_texCoord;

uniform mat4 u_projection;

void main(){

    gl_Position = u_projection * gl_ModelViewMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
    v_texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* ShadowShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform sampler2D u_shadowBuffer;
uniform sampler2D u_depthTex;
uniform mat4 u_viewMat;
uniform mat4 u_shadowViewMat;
uniform mat4 u_shadowProjMat;

float LinearizeDepth(vec2 uv)
{
  float n = 0.1; // camera z near
  float f = 512.0; // camera z far
  float z = texture2D(u_depthTex, uv.xy).x;
  return (2.0 * n) / (f + n - z * (f - n));
}

mat4 inverse(mat4 m)
{
   float SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
   float SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
   float SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
   float SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
   float SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
   float SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
   float SubFactor06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
   float SubFactor07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
  float SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
   float SubFactor09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
   float SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
   float SubFactor11 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
   float SubFactor12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
   float SubFactor13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
   float SubFactor14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
   float SubFactor15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
   float SubFactor16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
   float SubFactor17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
   float SubFactor18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

   mat4 adj;

   adj[0][0] = + (m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02);
   adj[1][0] = - (m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04);
   adj[2][0] = + (m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05);
   adj[3][0] = - (m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05);

   adj[0][1] = - (m[0][1] * SubFactor00 - m[0][2] * SubFactor01 + m[0][3] * SubFactor02);
   adj[1][1] = + (m[0][0] * SubFactor00 - m[0][2] * SubFactor03 + m[0][3] * SubFactor04);
   adj[2][1] = - (m[0][0] * SubFactor01 - m[0][1] * SubFactor03 + m[0][3] * SubFactor05);
   adj[3][1] = + (m[0][0] * SubFactor02 - m[0][1] * SubFactor04 + m[0][2] * SubFactor05);

   adj[0][2] = + (m[0][1] * SubFactor06 - m[0][2] * SubFactor07 + m[0][3] * SubFactor08);
   adj[1][2] = - (m[0][0] * SubFactor06 - m[0][2] * SubFactor09 + m[0][3] * SubFactor10);
   adj[2][2] = + (m[0][0] * SubFactor11 - m[0][1] * SubFactor09 + m[0][3] * SubFactor12);
   adj[3][2] = - (m[0][0] * SubFactor08 - m[0][1] * SubFactor10 + m[0][2] * SubFactor12);

   adj[0][3] = - (m[0][1] * SubFactor13 - m[0][2] * SubFactor14 + m[0][3] * SubFactor15);
   adj[1][3] = + (m[0][0] * SubFactor13 - m[0][2] * SubFactor16 + m[0][3] * SubFactor17);
   adj[2][3] = - (m[0][0] * SubFactor14 - m[0][1] * SubFactor16 + m[0][3] * SubFactor18);
   adj[3][3] = + (m[0][0] * SubFactor15 - m[0][1] * SubFactor17 + m[0][2] * SubFactor18);

   float det = (+ m[0][0] * adj[0][0]
		+ m[0][1] * adj[1][0]
		+ m[0][2] * adj[2][0]
		+ m[0][3] * adj[3][0]);

   return adj / det;
}

void main(){

    // define poisson disk
    vec2 poissonDisk[4];
    poissonDisk[0] = vec2(-0.94201624, -0.39906216);
    poissonDisk[1] = vec2(0.94558609, -0.76890725);
    poissonDisk[2] = vec2(-0.094184101, -0.92938870);
    poissonDisk[3] = vec2(0.34495938, 0.29387760);

    // get view ray
    float thfov = tan(60.f/2.f);
    float aspectRatio = 8.f/6.f;
    vec2 ndc = v_texCoord * 2.f -1.f;
    vec3 viewRay = vec3(ndc.x*thfov*aspectRatio, ndc.y*thfov, 1.f);

    float depth = LinearizeDepth(v_texCoord);
    vec3 viewPosition = viewRay * depth;
    vec4 modelPosition = inverse(u_viewMat) * vec4(viewPosition, 1.f);

    vec4 shadowPosition = u_shadowProjMat * u_shadowViewMat * modelPosition;
    vec4 shadowCoord = (shadowPosition * 0.5f + 0.5f);

    float visibility = 1.f;
    for(int i=0; i<4; i++)
    {
         if( texture2D(u_shadowBuffer, shadowCoord.xy + poissonDisk[i]/1500.f).z < shadowCoord.z - 0.005f )
                visibility -= 0.15f;
    }

    visibility = max(0.f, visibility);


    gl_FragData[0] = vec4(visibility, visibility, visibility, 1.f);
    gl_FragData[0] = vec4(modelPosition.xyz, 1.f);
    //gl_FragData[0] = vec4(texture2D(u_shadowBuffer, shadowCoord.xy).zzz, 1.f);
}

);


IMPGEARS_END
