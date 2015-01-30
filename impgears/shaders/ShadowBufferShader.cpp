#include "ShadowBufferShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
ShadowBufferShader::ShadowBufferShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
ShadowBufferShader::~ShadowBufferShader()
{
}


//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* ShadowBufferShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 texture_coordinate;

void main(){

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
    texture_coordinate = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* ShadowBufferShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 texture_coordinate;
uniform sampler2D my_color_texture;

void main(){
    vec4 textcolor = texture2D(my_color_texture, texture_coordinate);
    gl_FragData[0] = textcolor * gl_Color;
}

);


IMPGEARS_END
