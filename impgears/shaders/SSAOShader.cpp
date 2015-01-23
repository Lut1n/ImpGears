#include "SSAOShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
SSAOShader::SSAOShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
SSAOShader::~SSAOShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* SSAOShader::vertexCodeSource = IMP_GLSL_SRC(

uniform mat4 biasMat;
varying vec4 depthUV;

void main(){

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    depthUV = gl_Position;
    depthUV.xy /= depthUV.w;
    depthUV.xy = depthUV.xy *0.5f + 0.5f;
    gl_FrontColor = gl_Color;
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* SSAOShader::fragmentCodeSource = IMP_GLSL_SRC(

uniform sampler2D depthTex;
varying vec4 depthUV;

void main(){

    vec4 depthColor = texture2D(depthTex, depthUV.xy);
    gl_FragData[0] = vec4(depthColor.xyz, 1.f);
}

);

IMPGEARS_END
