#define GLSL_CODE( code ) #code

/// =========== VERTEX SHADER SOURCE =====================
static std::string basicVert = GLSL_CODE(
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat3 u_normal;

varying vec2 v_texCoord;
varying vec3 v_m;
varying vec3 v_mv;
varying vec3 v_n;
varying vec3 a_n;
varying vec3 v_vertex;

void main()
{
    vec4 mv_pos = u_view * u_model * gl_Vertex;
    gl_Position = u_proj * mv_pos;
    v_n = normalize(u_normal * gl_Normal);
    a_n = gl_Normal;
    gl_FrontColor = gl_Color;

    v_texCoord = gl_MultiTexCoord0.xy;
    v_m = (u_model * gl_Vertex).xyz;
    v_mv = mv_pos.xyz;
    v_vertex = gl_Vertex.xyz;
}

);

/// =========== FRAGMENT SHADER SOURCE =====================
static std::string basicFrag = GLSL_CODE(

uniform vec3 u_color;

varying vec2 v_texCoord;
varying vec3 v_m;
varying vec3 v_mv;
varying vec3 v_vertex;

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    vec4 color = vec4(u_color,1.0) * gl_Color;
    color *= textureColor(v_texCoord);
    vec4 emi = textureEmissive(v_texCoord);

    out_color = max(emi,color);
    out_emissive = emi;
    out_normal = vec3(0.0,0.0,1.0);

    out_reflectivity = textureReflectivity(v_texCoord);
    out_shininess = 0.0;
    float near = 0.1; float far = 128.0;
    out_depth = (length(v_mv.xyz) - near) / (far-near);
}

);



// glsl 1.30 (OpenGL 3.0) compatibility : inverse matrix is not available before glsl 1.40
/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_inverse_mat = GLSL_CODE(

mat3 inverse(mat3 m)
{
    float a00 = m[0][0];float a01 = m[0][1];float a02 = m[0][2];
    float a10 = m[1][0];float a11 = m[1][1];float a12 = m[1][2];
    float a20 = m[2][0];float a21 = m[2][1];float a22 = m[2][2];

    float b01 = a22 * a11 - a12 * a21;
    float b11 = -a22 * a10 + a12 * a20;
    float b21 = a21 * a10 - a11 * a20;

    float det = a00 * b01 + a01 * b11 + a02 * b21;

    return mat3(b01, (-a22 * a01 + a02 * a21), (a12 * a01 - a02 * a11),
    b11, (a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
    b21, (-a21 * a00 + a01 * a20), (a11 * a00 - a01 * a10)) / det;
}

mat4 inverse(mat4 m)
{
    float a00 = m[0][0]; float a01 = m[0][1]; float a02 = m[0][2]; float a03 = m[0][3];
    float a10 = m[1][0]; float a11 = m[1][1]; float a12 = m[1][2]; float a13 = m[1][3];
    float a20 = m[2][0]; float a21 = m[2][1]; float a22 = m[2][2]; float a23 = m[2][3];
    float a30 = m[3][0]; float a31 = m[3][1]; float a32 = m[3][2]; float a33 = m[3][3];

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

    return mat4(
        a11 * b11 - a12 * b10 + a13 * b09,
        a02 * b10 - a01 * b11 - a03 * b09,
        a31 * b05 - a32 * b04 + a33 * b03,
        a22 * b04 - a21 * b05 - a23 * b03,
        a12 * b08 - a10 * b11 - a13 * b07,
        a00 * b11 - a02 * b08 + a03 * b07,
        a32 * b02 - a30 * b05 - a33 * b01,
        a20 * b05 - a22 * b02 + a23 * b01,
        a10 * b10 - a11 * b08 + a13 * b06,
        a01 * b08 - a00 * b10 - a03 * b06,
        a30 * b04 - a31 * b02 + a33 * b00,
        a21 * b02 - a20 * b04 - a23 * b00,
        a11 * b07 - a10 * b09 - a12 * b06,
        a00 * b09 - a01 * b07 + a02 * b06,
        a31 * b01 - a30 * b03 - a32 * b00,
        a20 * b03 - a21 * b01 + a22 * b00) / det;
}

);


/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_samplerCN = GLSL_CODE(

uniform sampler2D u_sampler_color;
uniform sampler2D u_sampler_normal;

vec4 textureColor(vec2 uv)
{
    return texture2D(u_sampler_color, uv).xyzw;
}

vec3 textureNormal(vec2 uv)
{
    vec3 n = texture2D(u_sampler_normal,uv).xyz;
    return n * 2.0 - 1.0;
}

vec4 textureEmissive(vec2 uv){ return vec4(0.0,0.0,0.0,0.0); }

float textureReflectivity(vec2 uv){ return 0.0; }

);

static std::string glsl_samplerNone = GLSL_CODE(

vec4 textureColor(vec2 uv){ return vec4(1.0); }
vec3 textureNormal(vec2 uv){ return vec3(0.0,0.0,1.0); }
vec4 textureEmissive(vec2 uv){ return vec4(0.0,0.0,0.0,0.0); }
float textureReflectivity(vec2 uv){ return 0.0; }

);


/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_samplerCNE = GLSL_CODE(

uniform sampler2D u_sampler_color;
uniform sampler2D u_sampler_normal;
uniform sampler2D u_sampler_emissive;
uniform sampler2D u_sampler_reflectivity;

vec4 textureColor(vec2 uv)
{
    return texture2D(u_sampler_color, uv).xyzw;
}

vec3 textureNormal(vec2 uv)
{
    vec3 n = texture2D(u_sampler_normal,uv).xyz;
    return n * 2.0 - 1.0;
}

vec4 textureEmissive(vec2 uv)
{
    return texture2D(u_sampler_emissive,uv).xyzw;
}

float textureReflectivity(vec2 uv)
{
    return texture2D(u_sampler_reflectivity,uv).x;
}

);


static std::string glsl_phong = GLSL_CODE(

uniform mat4 u_model;
uniform mat4 u_view;
uniform vec3 u_color;
uniform float u_shininess;

varying vec2 v_texCoord;
varying vec3 v_m;
varying vec3 v_mv;
varying vec3 v_n;
varying vec3 a_n;
varying vec3 v_vertex;

mat3 build_tbn(vec3 n_ref)
{
    vec3 n_z = n_ref; // normalize();
    vec3 n_x = vec3(1.0,0.0,0.0);
    vec3 n_y = cross(n_z,n_x);
    vec3 n_x2 = vec3(0.0,1.0,0.0);
    vec3 n_y_2 = cross(n_z,n_x2);
    if(length(n_y) < length(n_y_2)) n_y = n_y_2;

    n_y = normalize(n_y);
    n_x = cross(n_y,n_z); n_x=normalize(n_x);
    mat3 tbn = mat3(n_x,n_y,n_z);
    return tbn;
}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    vec3 color = u_color.xyz * gl_Color.xyz;
    color *= textureColor(v_texCoord).xyz;
    vec4 emi = textureEmissive(v_texCoord);

    out_color = vec4(clamp( max(color,emi.xyz),0.0,1.0 ), 1.0);
    out_emissive = clamp(emi,0.0,1.0);

    vec3 normal = textureNormal(v_texCoord);
    mat3 normal_mat = transpose( inverse( mat3(u_view*u_model) ) );

    mat3 tbn = build_tbn( normalize(v_n) );
    // mat3 tbn_a = build_tbn( normalize(a_n) );
    out_normal = (normal_mat * tbn * normal) * 0.5 + 0.5;

    out_reflectivity = textureReflectivity(v_texCoord);
    out_shininess = u_shininess * 0.01;
    float near = 0.1; float far = 128.0;
    out_depth = (length(v_mv.xyz) - near) / (far-near);
}

);

static std::string glsl_mrt1 = GLSL_CODE(

void main()
{
    vec4 out_color;
    vec4 out_emissive;
    vec3 out_normal;
    float out_reflectivity;
    float out_shininess;
    float out_depth;

    lighting(out_color,out_emissive,out_normal,out_reflectivity,out_shininess,out_depth);

    gl_FragData[0] = out_color;
}

);

static std::string glsl_mrt2 = GLSL_CODE(

void main()
{
    vec4 out_color;
    vec4 out_emissive;
    vec3 out_normal;
    float out_reflectivity;
    float out_shininess;
    float out_depth;

    lighting(out_color,out_emissive,out_normal,out_reflectivity,out_shininess,out_depth);

    gl_FragData[0] = out_color;
    gl_FragData[1] = out_emissive;
}

);

static std::string glsl_mrt_default = GLSL_CODE(

void main()
{
    vec4 out_color;
    vec4 out_emissive;
    vec3 out_normal;
    float out_reflectivity;
    float out_shininess;
    float out_depth;

    lighting(out_color,out_emissive,out_normal,out_reflectivity,out_shininess,out_depth);

    gl_FragData[0] = out_color;
    gl_FragData[1] = out_emissive;
    gl_FragData[2] = vec4(out_normal,1.0);
    gl_FragData[3] = vec4(vec3(out_reflectivity),1.0);
    gl_FragData[4] = vec4(vec3(out_shininess),1.0);
    gl_FragData[5] = vec4(vec3(out_depth),1.0);
}

);
