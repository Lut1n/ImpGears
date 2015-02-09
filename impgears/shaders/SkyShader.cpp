#include "SkyShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
SkyShader::SkyShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
SkyShader::~SkyShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* SkyShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 texCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main(){

    gl_Position = u_projection * u_view * u_model * gl_Vertex;
    gl_FrontColor = gl_Color;
    texCoords = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* SkyShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 texCoords;
uniform sampler2D skyTexture;

void main(){

    vec4 txColor = texture2D(skyTexture, texCoords);

    gl_FragData[0] = txColor;
}

);

IMPGEARS_END
