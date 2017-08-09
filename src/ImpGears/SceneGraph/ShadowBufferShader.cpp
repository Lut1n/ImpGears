#include "Graphics/ShadowBufferShader.h"

IMPGEARS_BEGIN

ShadowBufferShader* ShadowBufferShader::instance = IMP_NULL;

//--------------------------------------------------------------
ShadowBufferShader::ShadowBufferShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
    instance = this;
}

//--------------------------------------------------------------
ShadowBufferShader::~ShadowBufferShader()
{
}


//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* ShadowBufferShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 texture_coordinate;
varying vec4 v_viewPosition;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main(){

    v_viewPosition = u_view * u_model * gl_Vertex;
    gl_Position = u_projection * v_viewPosition;
    gl_FrontColor = gl_Color;
    texture_coordinate = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* ShadowBufferShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 texture_coordinate;
varying vec4 v_viewPosition;

uniform sampler2D my_color_texture;

void main(){
    vec4 textcolor = texture2D(my_color_texture, texture_coordinate);
    gl_FragData[0] = textcolor * gl_Color;

    gl_FragDepth = -v_viewPosition.z/512.f;
}

);


IMPGEARS_END
