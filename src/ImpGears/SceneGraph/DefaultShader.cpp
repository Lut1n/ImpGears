#include <SceneGraph/DefaultShader.h>

IMPGEARS_BEGIN


DefaultShader* DefaultShader::_instance = IMP_NULL;


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
DefaultShader* DefaultShader::getInstance()
{
	return _instance;
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
uniform vec3 u_color;
uniform float u_type;

void main(){

	vec4 texColor = vec4(u_color, 1.0);
	
	if(u_type == 0.0)
		texColor *= texture2D(u_colorTexture, v_texCoord);
	
    gl_FragData[0] = texColor * gl_Color;
}

);

IMPGEARS_END
