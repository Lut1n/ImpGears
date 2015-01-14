#include "DefaultShader.h"

IMPGEARS_BEGIN

DefaultShader::DefaultShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

DefaultShader::~DefaultShader()
{
}

///SHADER SOURCE
const char* DefaultShader::vertexCodeSource =
"varying vec4 position_; \
varying vec4 vertex_; \
varying vec2 texture_coordinate; \
void main(){ \
    gl_FrontColor = gl_Color; \
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \
    position_ = gl_Position; \
    vertex_ =  gl_Vertex; \
    texture_coordinate = vec2(gl_MultiTexCoord0);\
}";

const char* DefaultShader::fragmentCodeSource =
"varying vec4 position_; \
varying vec4 vertex_; \
varying vec2 texture_coordinate; \
uniform sampler2D my_color_texture; \
void main(){ \
    float depth =  min(1.f, abs(50.f/max(position_.w, 1.0)) ); \
    float height = vertex_.z/256.f; \
    vec4 textcolor = texture2D(my_color_texture, texture_coordinate); \
    gl_FragColor = textcolor*vec4(depth, depth, depth, 1.f);\
}";



IMPGEARS_END
