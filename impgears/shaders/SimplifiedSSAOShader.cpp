#include "SimplifiedSSAOShader.h"

IMPGEARS_BEGIN

SimplifiedSSAOShader::SimplifiedSSAOShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

SimplifiedSSAOShader::~SimplifiedSSAOShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* SimplifiedSSAOShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform mat4 u_projection;

void main(){

    gl_Position = u_projection * gl_Vertex;
    gl_FrontColor = gl_Color;
    // get texture UV
    v_texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* SimplifiedSSAOShader::fragmentCodeSource = IMP_GLSL_SRC(

uniform sampler2D u_depthTex;
uniform float u_ssaoPixelRange;
uniform float u_ssaoPower;

varying vec2 v_texCoord;

float LinearizeDepth(vec2 uv)
{
  float n = 0.1; // camera z near
  float f = 512.0; // camera z far
  float z = texture2D(u_depthTex, uv.xy).x;
  return (2.0 * n) / (f + n - z * (f - n));
}

void main(){

    // get depth
    float originDepth = LinearizeDepth(v_texCoord);

    vec2 r = vec2(u_ssaoPixelRange, u_ssaoPixelRange);

    r /= vec2(800.f, 600.f);

    float leftDepth = LinearizeDepth(v_texCoord+vec2(-r.x, 0.f));
    float rightDepth = LinearizeDepth(v_texCoord+vec2(r.x, 0.f));
    float topDepth = LinearizeDepth(v_texCoord+vec2(0.f, r.y));
    float bottomDepth = LinearizeDepth(v_texCoord+vec2(0.f, -r.y));
    float lefttopDepth = LinearizeDepth(v_texCoord+vec2(-r.x, r.y));
    float leftbottomDepth = LinearizeDepth(v_texCoord+vec2(-r.x, -r.y));
    float righttopDepth = LinearizeDepth(v_texCoord+vec2(r.x, r.y));
    float rightbottomDepth = LinearizeDepth(v_texCoord+vec2(r.x, -r.y));

    float occlusion = 0.f;

    if(leftDepth <= originDepth)occlusion += u_ssaoPower;
    if(rightDepth <= originDepth)occlusion += u_ssaoPower;
    if(topDepth <= originDepth)occlusion += u_ssaoPower;
    if(bottomDepth <= originDepth)occlusion += u_ssaoPower;
    if(lefttopDepth <= originDepth)occlusion += u_ssaoPower;
    if(leftbottomDepth <= originDepth)occlusion += u_ssaoPower;
    if(righttopDepth <= originDepth)occlusion += u_ssaoPower;
    if(rightbottomDepth <= originDepth)occlusion += u_ssaoPower;

    occlusion = 1.f - (occlusion/8.f);

    gl_FragData[0] = vec4(occlusion, occlusion, occlusion, 1.f);
}

);

IMPGEARS_END
