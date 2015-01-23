#include "DepthNormalShader.h"

#include "base/frustumParams.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
DepthNormalShader::DepthNormalShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
DepthNormalShader::~DepthNormalShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* DepthNormalShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec3 normal;

void main(){

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    normal = gl_NormalMatrix * gl_Normal;
    gl_FrontColor = gl_Color;
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* DepthNormalShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec3 normal;

void main(){

    gl_FragData[0] = vec4(normal.xyz, 1.f);
}

);

IMPGEARS_END
