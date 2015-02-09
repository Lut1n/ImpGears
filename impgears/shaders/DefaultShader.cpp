#include "DefaultShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
DefaultShader::DefaultShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
DefaultShader::~DefaultShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* DefaultShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main(){

    gl_Position = u_projection * u_view * u_model * gl_Vertex;

    gl_FrontColor = vec4(1.f, 1.f, 1.f, 1.f);
    v_texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* DefaultShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;
uniform sampler2D u_colorTexture;

void main(){

    vec4 texColor = texture2D(u_colorTexture, v_texCoord);
    gl_FragData[0] = texColor * gl_Color;
}

);

IMPGEARS_END
