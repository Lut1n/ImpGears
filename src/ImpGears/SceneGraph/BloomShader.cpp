#include "Graphics/BloomShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
BloomShader::BloomShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
BloomShader::~BloomShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* BloomShader::vertexCodeSource = IMP_GLSL_SRC(

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
const char* BloomShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;
uniform sampler2D u_inTextureOrigin;
uniform sampler2D u_inTexture16;
uniform sampler2D u_inTexture32;
uniform sampler2D u_inTexture64;
uniform sampler2D u_inTexture128;

void main(){

    float texel16Size = 1.f/16.f;
    float texel32Size = 1.f/32.f;
    float texel64Size = 1.f/64.f;
    float texel128Size = 1.f/128.f;

	float gaussKernel[7];
	gaussKernel[0] = 0.006f;
	gaussKernel[1] = 0.061f;
	gaussKernel[2] = 0.242f;
	gaussKernel[3] = 0.383f;
	gaussKernel[4] = 0.242f;
	gaussKernel[5] = 0.061f;
	gaussKernel[6] = 0.006f;

	vec2 offset;
    vec4 result = vec4(0.f, 0.f, 0.f, 0.f);
    for(int i=-3; i<4; ++i)
    {
        for(int j=-3; j<4; ++j)
        {
			// 16
            offset = vec2(float(i), float(j))*texel16Size;
            result += texture2D(u_inTexture16, v_texCoord+offset) * gaussKernel[i+3] * gaussKernel[j+3];
			// 32
            offset = vec2(float(i), float(j))*texel32Size;
            result += texture2D(u_inTexture32, v_texCoord+offset) * gaussKernel[i+3] * gaussKernel[j+3];
			// 64
            offset = vec2(float(i), float(j))*texel64Size;
            result += texture2D(u_inTexture64, v_texCoord+offset) * gaussKernel[i+3] * gaussKernel[j+3];
			// 128
            offset = vec2(float(i), float(j))*texel128Size;
            result += texture2D(u_inTexture128, v_texCoord+offset) * gaussKernel[i+3] * gaussKernel[j+3];
        }
    }

	vec4 originColor = texture2D(u_inTextureOrigin, v_texCoord);

    gl_FragData[0] = originColor + result * gl_Color;
}

);

IMPGEARS_END

