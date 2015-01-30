#include "FinalRenderShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
FinalRenderShader::FinalRenderShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
FinalRenderShader::~FinalRenderShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* FinalRenderShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 texCoord;

void main(){

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = vec4(1.f, 1.f, 1.f, 1.f);
    texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* FinalRenderShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 texCoord;
uniform sampler2D u_colorTexture;
uniform sampler2D u_ssaoTexture;
uniform sampler2D u_backgroundTexture;

void main(){

    vec4 texColor = texture2D(u_colorTexture, texCoord);
    vec4 ssaoColor = texture2D(u_ssaoTexture, texCoord);
    vec4 backColor = texture2D(u_backgroundTexture, texCoord);

    vec3 resultColor = backColor.xyz*(1.f-texColor.w) + (texColor.xyz * ssaoColor.xyz)*texColor.w;

    gl_FragData[0] = gl_Color * vec4(resultColor, 1.f);
}

);

IMPGEARS_END

