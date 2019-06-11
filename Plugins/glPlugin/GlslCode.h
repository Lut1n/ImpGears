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
varying vec3 v_n;

void main()
{
	vec4 mv_pos = u_view * u_model * gl_Vertex;
	gl_Position = u_proj * mv_pos;
	v_n = normalize(u_normal * gl_Normal);
	gl_FrontColor = gl_Color;
	
	v_texCoord = vec2(gl_MultiTexCoord0);
	v_m = (u_model * gl_Vertex).xyz;
}

);

/// =========== FRAGMENT SHADER SOURCE =====================
static std::string basicFrag = GLSL_CODE(

uniform vec3 u_color;

varying vec2 v_texCoord;

void lighting(out vec3 outColor, out vec3 outEmi)
{
	vec3 color = u_color * gl_Color.xyz;
	color *= textureColor(v_texCoord);
	vec3 emi = textureEmissive(v_texCoord);
	
	outColor = max(emi,color);
	outEmi = emi;
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

vec3 textureColor(vec2 uv)
{
	return texture2D(u_sampler_color, uv).xyz;
}

vec3 textureNormal(vec2 uv)
{
	vec3 n = texture2D(u_sampler_normal,uv).xyz;
	return n * 2.0 - 1.0;
}

vec3 textureEmissive(vec2 uv){ return vec3(0.0,0.0,0.0); }

);

static std::string glsl_samplerNone = GLSL_CODE(

vec3 textureColor(vec2 uv){ return vec3(1.0); }
vec3 textureNormal(vec2 uv){ return vec3(0.0,0.0,1.0); }
vec3 textureEmissive(vec2 uv){ return vec3(0.0,0.0,0.0); }

);


/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_samplerCNE = GLSL_CODE(

uniform sampler2D u_sampler_color;
uniform sampler2D u_sampler_normal;
uniform sampler2D u_sampler_emissive;

vec3 textureColor(vec2 uv)
{
	return texture2D(u_sampler_color, uv).xyz;
}

vec3 textureNormal(vec2 uv)
{
	vec3 n = texture2D(u_sampler_normal,uv).xyz;
	return n * 2.0 - 1.0;
}

vec3 textureEmissive(vec2 uv)
{
	return texture2D(u_sampler_emissive,uv).xyz;
}

);


static std::string glsl_phong = GLSL_CODE(

uniform mat4 u_view;
uniform vec3 u_lightPos;
uniform vec3 u_lightCol;
uniform vec3 u_lightAtt;
uniform vec3 u_color;

varying vec2 v_texCoord;
varying vec3 v_m;
varying vec3 v_n;

void lighting(out vec3 outColor, out vec3 outEmi)
{
	float lightPower = u_lightAtt[0];
	float shininess = u_lightAtt[1];
	
	vec3 color = u_color * gl_Color.xyz;
	vec3 light_dir = u_lightPos - v_m;
	float dist = length(light_dir);
	dist = dist * dist;
	light_dir = normalize(light_dir);
	
	// TBN Matrix for computation in tangent space
	vec3 n_z = v_n;
	vec3 n_x = vec3(1.0,0.0,0.0);
	vec3 n_y = cross(n_z,n_x); n_y=normalize(n_y);
	n_x = cross(n_y,n_z); n_x=normalize(n_x);
	mat3 tbn = inverse( mat3(n_x,n_y,n_z) );
	
	vec3 normal = textureNormal(v_texCoord);
	
	float lambertian = max(dot(light_dir,normal),0.0);
	float specular = 0.0;
	
	if(lambertian > 0.0)
	{
		// view dir
		vec3 view_pos = tbn * -u_view[3].xyz;
		vec3 view_dir = view_pos - v_m;
		view_dir=normalize(view_dir);
		
		// blinn phong
		light_dir = tbn * light_dir; 
		vec3 halfDir = light_dir + view_dir;
		float specAngle = max( dot(halfDir,normal), 0.0 );
		specular = pow(specAngle, shininess);
	}
	
	// Color model :
	// ambiantColor
	// + diffColor * lambertian * lightColor * lightPower / distance
	// + specColor * specular * lightColor * lightPower / distance
	
	color *= textureColor(v_texCoord);
	
	vec3 colModelRes = color*0.01
	+ color*0.7 * lambertian * u_lightCol * lightPower / dist
	+ color*0.3 * specular * u_lightCol * lightPower / dist;
	
	vec3 emi = textureEmissive(v_texCoord);
	colModelRes = max(emi,colModelRes);
	
	outColor = clamp( colModelRes,0.0,1.0 );
	outEmi = clamp(emi,0.0,1.0);
}

);


static std::string glsl_mrt1 = GLSL_CODE(

void main()
{
	vec3 outColor;
	vec3 outEmi;
	
	lighting(outColor,outEmi);
	
	gl_FragData[0] = vec4(outColor,1.0);
}

);

static std::string glsl_mrt2 = GLSL_CODE(

void main()
{
	vec3 outColor;
	vec3 outEmi;
	
	lighting(outColor,outEmi);
	
	gl_FragData[0] = vec4(outColor,1.0);
	gl_FragData[1] = vec4(outEmi,1.0);
}

);
