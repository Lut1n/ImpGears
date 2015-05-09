#include "shaders/PreBloomShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
PreBloomShader::PreBloomShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
PreBloomShader::~PreBloomShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* PreBloomShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform mat4 u_projection;

void main(){

    gl_Position = u_projection * gl_Vertex;

    gl_FrontColor = vec4(1.f, 1.f, 1.f, 1.f);
    v_texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* PreBloomShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;
uniform sampler2D u_inTexture;

void main(){

    vec4 texColor = texture2D(u_inTexture, v_texCoord);

    gl_FragData[0] = texColor * gl_Color; // 16
    // gl_FragData[1] = texColor * gl_Color; // 32
    // gl_FragData[2] = texColor * gl_Color; // 64
    // gl_FragData[3] = texColor * gl_Color; // 128
}

);

IMPGEARS_END
