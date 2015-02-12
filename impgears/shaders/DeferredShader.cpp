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
varying vec4 v_viewPosition;
varying vec2 v_texCoord;
varying vec4 v_shadowProjCoord;
varying vec4 v_shadowViewPosition;

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
    v_viewPosition = u_view * u_model * gl_Vertex;
    v_shadowViewPosition = u_shadowViewMat * u_shadowModelMat * gl_Vertex;
    v_shadowProjCoord = u_shadowProjMat * v_shadowViewPosition;
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
varying vec4 v_viewPosition;
varying vec2 v_texCoord;
varying vec4 v_shadowProjCoord;
varying vec4 v_shadowViewPosition;

uniform sampler2D u_colorTexture;
uniform sampler2D u_specTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_selfTexture;
uniform sampler2D u_shadowBuffer;

uniform vec3 u_sun;
uniform vec3 u_eye;

uniform float u_mapLevel;

uniform float u_far;

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

    if(v_mPosition.z > u_mapLevel+1.f)
    {
        gl_FragDepth = gl_FragCoord.z;
        discard;
    }

    vec3 normalMaterial = texture2D(u_normalTexture, v_texCoord).xyz * 2.f - 1.f;
    vec4 selfMaterial = texture2D(u_selfTexture, v_texCoord);
    vec4 specMaterial = texture2D(u_specTexture, v_texCoord);

    mat3 TBN = mat3(v_tan, v_bitan, v_normal);
    // TBN = transpose(TBN);
    normalMaterial = normalize(TBN * normalMaterial);

    vec3 fragNormal = (normalMaterial + 1.f) /2.f;
    gl_FragData[1] = vec4(fragNormal, 1.f);

    vec3 vertexToSun = normalize(u_sun - v_mPosition.xyz);
	vec3 vertexToEye = normalize(u_eye - v_mPosition.xyz);

	float diffuseFactor = max(0.f, dot(vertexToSun, normalMaterial));

    vec3 lightReflect = normalize(reflect(vertexToSun, normalMaterial));
    float specularFactor = dot(vertexToEye, lightReflect);

    if(diffuseFactor > 0.f)
        specularFactor = pow(specularFactor, 32.0);

    vec2 shadowCoord = v_shadowProjCoord.xy / v_shadowProjCoord.w;
    shadowCoord = (shadowCoord + 1.f)/2.f;

    float visibility = 1.f;
    for(int i=0; i<4; i++)
    {
         if( texture2D(u_shadowBuffer, shadowCoord + poissonDisk[i]/2000.f).z * 512.f < -v_shadowViewPosition.z - 0.005f )
                visibility -= 0.2f;
    }
    visibility = max(0.f, visibility);

    vec4 texColor = texture2D(u_colorTexture, v_texCoord);

    vec4 fragColor = texColor * 0.2f; // simul base for ambiant;

    fragColor += texColor * diffuseFactor; // diffuse

    if(diffuseFactor > 0.f && visibility > 0.4f)
    {
        fragColor += specMaterial * specularFactor; // specular
    }

    float linearDepth = -v_viewPosition.z / u_far;
    gl_FragData[2] = vec4(linearDepth, 0.f, 0.f, 1.f);
    gl_FragDepth = linearDepth;

    gl_FragData[3] = vec4(selfMaterial.xyz, 1.f);

    if(v_mPosition.z > u_mapLevel)
        fragColor *= 0.f;

    fragColor *= visibility;
    fragColor.w = 1.f;
    gl_FragData[0] = fragColor * gl_Color;
}

);

IMPGEARS_END
