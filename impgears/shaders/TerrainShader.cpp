#include "TerrainShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
TerrainShader* TerrainShader::instance = IMP_NULL;

//--------------------------------------------------------------
TerrainShader::TerrainShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
    instance = this;
}

//--------------------------------------------------------------
TerrainShader::~TerrainShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* TerrainShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec4 position_;
varying vec4 vertex_;
varying vec2 texture_coordinate;
varying vec4 shadow_coordinate;
varying float tooH;
uniform float chunkZ;
uniform float mapLevel;

uniform mat4 shadow_pMat;
uniform mat4 shadow_mvMat;
uniform mat4 shadow_coordBiasMat;

void main(){

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    position_ = gl_Position;
    shadow_coordinate = shadow_coordBiasMat* shadow_pMat * shadow_mvMat * gl_Vertex;


    vec4 verColor = gl_Color;
    if(chunkZ+gl_Vertex.z > mapLevel)
        verColor *= vec4(0.0, 0.0, 0.0, 1.0);

    tooH = 0.f;
    if(chunkZ+gl_Vertex.z > mapLevel+1.f)
        tooH = 1.f;

    gl_FrontColor = verColor;
    vertex_ =  gl_Vertex;
    texture_coordinate = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* TerrainShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec4 position_;
varying vec4 vertex_;
varying vec2 texture_coordinate;
varying vec4 shadow_coordinate;
varying float tooH;
uniform sampler2D my_color_texture;
uniform sampler2D my_shadow_texture;

void main(){

    vec2 poissonDisk[4];
    poissonDisk[0] = vec2(-0.94201624, -0.39906216);
    poissonDisk[1] = vec2(0.94558609, -0.76890725);
    poissonDisk[2] = vec2(-0.094184101, -0.92938870);
    poissonDisk[3] = vec2(0.34495938, 0.29387760);

    if(tooH  == 1.f)
        discard;

    float depth =  min(1.f, abs(50.f/max(position_.w, 1.0)) );
    vec4 textcolor = texture2D(my_color_texture, texture_coordinate);

    float visibility = 1.f;
    for(int i=0; i<4; i++)
    {
         if( texture2D(my_shadow_texture, shadow_coordinate.xy + poissonDisk[i]/1500.f).z < shadow_coordinate.z - 0.005f )
                visibility -= 0.15f;
    }

    visibility = max(0.f, visibility);

    gl_FragData[0] = textcolor*vec4(depth, depth, depth, 1.f) * gl_Color * vec4(visibility, visibility, visibility, 1.f);
}

);

IMPGEARS_END
