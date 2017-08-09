#include <SceneGraph/DeferredShader.h>

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

varying vec4 v_viewPosition;
varying vec2 v_texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_normal;

void main(){
    v_mPosition = u_model * gl_Vertex;

    gl_Position = u_projection * u_view * u_model * gl_Vertex;
    v_viewPosition = u_view * u_model * gl_Vertex;
    v_texCoord = vec2(gl_MultiTexCoord0);
    gl_FrontColor = gl_Color;

    v_normal = vec4(u_normal * vec4(gl_Normal, 0.f)).xyz;
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

varying vec4 v_viewPosition;
varying vec2 v_texCoord;

uniform sampler2D u_colorTexture;
uniform sampler2D u_specTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_selfTexture;

uniform float u_mapLevel;
uniform float u_far;

void main(){

    if(v_mPosition.z > u_mapLevel+1.f)
    {
        gl_FragDepth = gl_FragCoord.z;
        discard;
    }

    vec3 normalMaterial = texture2D(u_normalTexture, v_texCoord).xyz * 2.f - 1.f;
    vec4 selfMaterial = texture2D(u_selfTexture, v_texCoord);
    vec4 specMaterial = texture2D(u_specTexture, v_texCoord);

    mat3 TBN = mat3(v_tan, v_bitan, v_normal);
    normalMaterial = normalize(TBN * normalMaterial);

    vec3 fragNormal = (normalMaterial + 1.f) /2.f;

    vec4 texColor = texture2D(u_colorTexture, v_texCoord);

    float linearDepth = -v_viewPosition.z / u_far;
    gl_FragDepth = linearDepth;


    if(v_mPosition.z > u_mapLevel)
        texColor *= 0.f;

    gl_FragData[0] = vec4(texColor.xyz, 1.f);
    gl_FragData[1] = vec4(fragNormal, 1.f);
    gl_FragData[2] = specMaterial;
    gl_FragData[3] = vec4(selfMaterial.xyz, 1.f);
}

);

IMPGEARS_END
