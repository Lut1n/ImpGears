#include "DebugTextureShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
DebugTextureShader::DebugTextureShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
DebugTextureShader::~DebugTextureShader()
{
}


//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* DebugTextureShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 texCoord;

void main(){

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = vec4(1.f, 1.f, 1.f, 1.f);
    texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* DebugTextureShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 texCoord;
uniform sampler2D u_colorTexture;

void main(){

    vec4 texColor = texture2D(u_colorTexture, texCoord);
    gl_FragData[0] = gl_Color;
}

);

IMPGEARS_END
