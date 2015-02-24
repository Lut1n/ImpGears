#include "shaders/BlurShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
BlurShader::BlurShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
BlurShader::~BlurShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* BlurShader::vertexCodeSource = IMP_GLSL_SRC(

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
const char* BlurShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;
uniform sampler2D u_colorTexture;
uniform float u_blurSize_x;
uniform float u_blurSize_y;

void main(){

    vec2 texelSize = 1.f/vec2(800.f, 600.f);

    vec4 result = vec4(0.f, 0.f, 0.f, 0.f);
    for(int i=int(-u_blurSize_x/2.f); i<int(u_blurSize_x/2.f); ++i)
    {
        for(int j=int(-u_blurSize_y/2.f); j<int(u_blurSize_y/2.f); ++j)
        {
            vec2 offset = vec2(float(i), float(j))*texelSize;
            result += texture2D(u_colorTexture, v_texCoord+offset);
        }
    }

    result /= (u_blurSize_x*u_blurSize_y);

    gl_FragData[0] = result * gl_Color;
}

);

IMPGEARS_END

