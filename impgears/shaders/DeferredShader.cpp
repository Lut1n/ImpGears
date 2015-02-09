#include "DeferredShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
DeferredShader* DeferredShader::instance = IMP_NULL;

//--------------------------------------------------------------
DeferredShader::DeferredShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
    instance = this;
}

//--------------------------------------------------------------
DeferredShader::~DeferredShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* DeferredShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec3 v_normal;
varying vec3 v_tan;
varying vec3 v_bitan;
varying vec4 v_mPosition;

varying vec4 v_position;
varying vec2 v_texCoord;
varying vec4 v_shadowCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_normal;
uniform mat4 u_shadowProjMat;
uniform mat4 u_shadowViewMat;
uniform mat4 u_shadowModelMat;

void main(){
    v_mPosition = u_model * gl_Vertex;

    gl_Position = u_projection * u_view * u_model * gl_Vertex;
    v_position = gl_Vertex;
    v_shadowCoord = u_shadowProjMat * u_shadowViewMat * u_shadowModelMat * gl_Vertex;
    v_shadowCoord = v_shadowCoord * 0.5f + 0.5f;
    v_texCoord = vec2(gl_MultiTexCoord0);
    gl_FrontColor = gl_Color;

    v_normal = vec4(u_normal * vec4(gl_Normal, 0.f)).xyz;
    //v_normal = vec4(vec4(gl_Normal, 0.f)).xyz;
    vec3 t1 = cross(v_normal, vec3(0.0, 0.0, 1.0));
	vec3 t2 = cross(v_normal, vec3(0.0, 1.0, 0.0));
	if(length(t1) > length(t2))
		v_tan = t1;
	else
		v_tan = t2;
	v_tan = normalize(v_tan);
	v_bitan = normalize(cross(v_normal, v_tan));
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* DeferredShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec3 v_normal;
varying vec3 v_tan;
varying vec3 v_bitan;
varying vec4 v_mPosition;

varying vec4 v_position;
varying vec2 v_texCoord;
varying vec4 v_shadowCoord;

uniform sampler2D u_colorTexture;
uniform sampler2D u_specTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_selfTexture;
uniform sampler2D u_shadowBuffer;

uniform vec3 u_sun;
uniform vec3 u_eye;

uniform float u_chunkLevel;
uniform float u_mapLevel;

mat3 transpose(mat3 m)
{
    mat3 result;

    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            result[i][j] = m[j][i];
        }
    }

    return result;
}

void main(){

    // define poisson disk
    vec2 poissonDisk[4];
    poissonDisk[0] = vec2(-0.94201624, -0.39906216);
    poissonDisk[1] = vec2(0.94558609, -0.76890725);
    poissonDisk[2] = vec2(-0.094184101, -0.92938870);
    poissonDisk[3] = vec2(0.34495938, 0.29387760);

    if(v_position.z+u_chunkLevel > u_mapLevel+1.f)
        discard;

    vec4 fragPosition = (v_position + 1.f)/2.f;
    gl_FragData[2] = fragPosition;

    vec3 normalMaterial = texture2D(u_normalTexture, v_texCoord).xyz * 2.f - 1.f;
    vec4 selfMaterial = texture2D(u_selfTexture, v_texCoord);
    vec4 specMaterial = texture2D(u_specTexture, v_texCoord);

    mat3 TBN = mat3(v_tan, v_bitan, v_normal);
    // TBN = transpose(TBN);
    normalMaterial = normalize(TBN * normalMaterial);

    vec3 fragNormal = (normalMaterial + 1.f) /2.f;
    gl_FragData[1] = vec4(fragNormal, 1.f);

    vec3 sunVec = normalize(u_sun - v_mPosition.xyz);
	vec3 vertexToEye = normalize(u_eye - v_mPosition.xyz);

	float diffuseFactor = max(0.f, dot(sunVec, normalMaterial));

    vec3 lightReflect = normalize(reflect(sunVec, normalMaterial));
    float specularFactor = dot(vertexToEye, lightReflect);

    if(diffuseFactor > 0.f)
        specularFactor = pow(specularFactor, 32.0);

    float visibility = 1.f;
    for(int i=0; i<4; i++)
    {
         if( texture2D(u_shadowBuffer, v_shadowCoord.xy + poissonDisk[i]/1500.f).z < v_shadowCoord.z - 0.005f )
                visibility -= 0.15f;
    }
    visibility = max(0.f, visibility);

    vec4 texColor = texture2D(u_colorTexture, v_texCoord) * vec4(visibility, visibility, visibility, 1.f);
    texColor *= vec4(diffuseFactor, diffuseFactor, diffuseFactor, 1.f);

    if(diffuseFactor > 0.f && visibility >= 0.4f)
    {
        texColor += specMaterial*vec4(specularFactor, specularFactor, specularFactor, 1.f);
    }

    //texColor += vec4(selfMaterial.xyz, 1.f);
    gl_FragData[3] = vec4(selfMaterial.xyz, 1.f);

    if(v_position.z+u_chunkLevel > u_mapLevel)
        texColor *= vec4(0.0, 0.0, 0.0, 1.0);

    gl_FragData[0] = texColor * gl_Color;
}

);

IMPGEARS_END
