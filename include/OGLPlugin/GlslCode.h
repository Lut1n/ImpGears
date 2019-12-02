#define GLSL_CODE( code ) #code

/// =========== VERTEX SHADER SOURCE =====================
static std::string basicVert = GLSL_CODE(
// #version 130

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

    v_texCoord = vec2(gl_MultiTexCoord0);
    v_m = (u_model * gl_Vertex).xyz;
    v_mv = mv_pos.xyz;
    v_vertex = gl_Vertex;
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
              out vec3 out_position,
              out vec3 out_normal,
              out float out_metalness,
              out float out_depth)
{
    vec4 color = vec4(u_color,1.0) * gl_Color;
    color *= textureColor(v_texCoord);
    vec4 emi = textureEmissive(v_texCoord);

    out_color = max(emi,color);
    out_emissive = emi;
    out_position = normalize(v_vertex) * 0.5 + 0.5;
    out_normal = vec3(0.0,0.0,1.0);
    out_metalness = 0.0;
    float near = 0.1; float far = 128.0;
    out_depth = (length(v_mv.xyz) - near) / far;
}

);


/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_invMat3 = GLSL_CODE(

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

);

static std::string glsl_samplerNone = GLSL_CODE(

vec4 textureColor(vec2 uv){ return vec4(1.0); }
vec3 textureNormal(vec2 uv){ return vec3(0.0,0.0,1.0); }
vec4 textureEmissive(vec2 uv){ return vec4(0.0,0.0,0.0,0.0); }

);


/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_samplerCNE = GLSL_CODE(

uniform sampler2D u_sampler_color;
uniform sampler2D u_sampler_normal;
uniform sampler2D u_sampler_emissive;

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

);


static std::string glsl_phong = GLSL_CODE(

uniform mat4 u_model;
uniform mat4 u_view;
uniform vec3 u_lightPos;
uniform vec3 u_lightCol;
uniform vec3 u_lightAtt;
uniform vec3 u_color;

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
              out vec3 out_position,
              out vec3 out_normal,
              out float out_metalness,
              out float out_depth)
{
    float shininess = u_lightAtt[1];

    vec3 color = u_color.xyz * gl_Color.xyz;
    color *= textureColor(v_texCoord).xyz;
    vec4 emi = textureEmissive(v_texCoord);

    out_color = vec4(clamp( color,0.0,1.0 ), 1.0);
    out_emissive = clamp(emi,0.0,1.0);
    out_position = normalize(v_vertex) * 0.5 + 0.5;

    vec3 normal = textureNormal(v_texCoord);
    mat3 normal_mat = transpose( inverse( mat3(u_view*u_model) ) );

    mat3 tbn = build_tbn( normalize(v_n) );
    mat3 tbn_a = build_tbn( normalize(a_n) );
    out_normal = (normal_mat * tbn_a * normal) * 0.5 + 0.5;
    out_metalness = shininess;
    float near = 0.1; float far = 128.0;
    out_depth = (length(v_mv.xyz) - near) / far;
}

);

static std::string glsl_mrt1 = GLSL_CODE(

void main()
{
    vec4 out_color;
    vec4 out_emissive;
    vec3 out_position;
    vec3 out_normal;
    float out_metalness;
    float out_depth;

    lighting(out_color,out_emissive,out_position,out_normal,out_metalness,out_depth);

    gl_FragData[0] = out_color;
}

);

static std::string glsl_mrt2 = GLSL_CODE(

void main()
{
    vec4 out_color;
    vec4 out_emissive;
    vec3 out_position;
    vec3 out_normal;
    float out_metalness;
    float out_depth;

    lighting(out_color,out_emissive,out_position,out_normal,out_metalness,out_depth);

    gl_FragData[0] = out_color;
    gl_FragData[1] = out_emissive;
}

);

static std::string glsl_mrt_default = GLSL_CODE(

void main()
{
    vec4 out_color;
    vec4 out_emissive;
    vec3 out_position;
    vec3 out_normal;
    float out_metalness;
    float out_depth;

    lighting(out_color,out_emissive,out_position,out_normal,out_metalness,out_depth);

    gl_FragData[0] = out_color;
    gl_FragData[1] = out_emissive;
    gl_FragData[2] = vec4(out_position,1.0);
    gl_FragData[3] = vec4(out_normal,1.0);
    gl_FragData[4] = vec4(vec3(out_metalness),1.0);
    gl_FragData[5] = vec4(vec3(out_depth),1.0);
}

);
