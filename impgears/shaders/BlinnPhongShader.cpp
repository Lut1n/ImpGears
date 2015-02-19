#include "BlinnPhongShader.h"

IMPGEARS_BEGIN

BlinnPhongShader::BlinnPhongShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

BlinnPhongShader::~BlinnPhongShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* BlinnPhongShader::vertexCodeSource = IMP_GLSL_SRC(

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
const char* BlinnPhongShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform sampler2D u_ambientIntensity;
uniform sampler2D u_diffuseColor;
uniform sampler2D u_specularColor;
uniform sampler2D u_normalSampler;

uniform sampler2D u_depthSampler;
uniform float u_far;

uniform vec3 u_viewSunPosition;
uniform vec3 u_viewEyePosition;

uniform float u_intensity;
uniform float u_shininess;

vec4 unproject(vec2 txCoord)
{
    float radFov = 60.f * 3.14f / 180.f;
    float thfov = tan(radFov/2.f);

    float whRatio = 4.f/3.f;
    vec2 ndc = txCoord * 2.f - 1.f;

    vec3 viewRay = vec3(ndc.x*thfov, ndc.y*thfov/whRatio, -1.f);
    float viewDepth = texture2D(u_depthSampler, txCoord.xy).x * u_far;
    vec4 viewVec = vec4(viewRay * viewDepth, 1.f);
    //viewVec /= viewVec.w;

    return viewVec;
}

void main(){

    float ambientIntensity = texture2D(u_ambientIntensity, v_texCoord).x;

    vec3 normalMaterial = texture2D(u_normalSampler, v_texCoord).xyz * 2.f - 1.f;

    vec4 diffuseColor = texture2D(u_diffuseColor, v_texCoord);
    vec3 specularColor = texture2D(u_specularColor, v_texCoord).xyz;

    vec4 viewPosition = unproject(v_texCoord);

    vec3 vertexToSun = normalize(u_viewSunPosition - viewPosition.xyz);
	vec3 vertexToEye = normalize(u_viewEyePosition - viewPosition.xyz);

	float diffuseFactor = max(dot(vertexToSun, normalMaterial), 0.f);

    // Phong
    //vec3 lightReflect = normalize(reflect(-vertexToSun, normalMaterial));
    //float specularFactor = max(dot(vertexToEye, lightReflect), 0.f);

    // Blinn-Phong
    vec3 halfVector = normalize(vertexToEye + vertexToSun);
    float specularFactor = max(dot(halfVector, normalMaterial), 0.f);

    specularFactor = diffuseFactor > 0.f ? specularFactor : 0.f;
    // specularFactor = length(diffuseColor.xyz) > 0.4f ? specularFactor : 0.f; //shadows ?
    specularFactor = pow(specularFactor, u_shininess);

    vec4 fragColor = vec4(diffuseColor.xyz * u_intensity * ambientIntensity, diffuseColor.w) +
                     vec4(diffuseColor.xyz * diffuseFactor, 0.f) +
                     vec4(specularColor * specularFactor * u_intensity, 0.f);

    gl_FragData[0] = fragColor;
}

);

IMPGEARS_END
