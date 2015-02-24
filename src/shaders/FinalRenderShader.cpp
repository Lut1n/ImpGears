#include "shaders/FinalRenderShader.h"

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

uniform mat4 u_projection;

void main(){

    gl_Position = u_projection * gl_Vertex;
    gl_FrontColor = vec4(1.f, 1.f, 1.f, 1.f);
    texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* FinalRenderShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 texCoord;
uniform sampler2D u_colorTexture;
uniform sampler2D u_backgroundTexture;
uniform sampler2D u_selfTexture;
uniform sampler2D u_shadows;

void main(){

    vec4 texColor = texture2D(u_colorTexture, texCoord);
    vec4 backColor = texture2D(u_backgroundTexture, texCoord);
    vec4 selfColor = texture2D(u_selfTexture, texCoord);
    vec4 shadow = texture2D(u_shadows, texCoord);

    vec4 foregroundColor = texColor*shadow + vec4(selfColor.xyz, 0.f);

    vec3 resultColor = backColor.xyz*(1.f-foregroundColor.w) + (foregroundColor.xyz)*foregroundColor.w;

    gl_FragData[0] = gl_Color * vec4(resultColor, 1.f);
}

);

IMPGEARS_END

