#include "Graphics/BasicShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
BasicShader::BasicShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
BasicShader::~BasicShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* BasicShader::vertexCodeSource = IMP_GLSL_SRC(

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

varying vec4 v_vertexColor;

void main(){

    gl_Position = u_projection * u_view * u_model * gl_Vertex;

    v_vertexColor = vec4(1.f, 1.f, 1.f, 1.f);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* BasicShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec4 v_vertexColor;

uniform vec3 u_color;

void main(){

	vec4 texColor = vec4(u_color, 1.0);
	
    gl_FragData[0] = texColor * v_vertexColor;
}

);

IMPGEARS_END
