#version 130

uniform mat4 u_view;
uniform vec3 u_lightPos;
uniform vec3 u_lightCol;
uniform vec3 u_lightAtt;
uniform vec3 u_color;

uniform sampler2D u_sampler_normal;
uniform sampler2D u_sampler_color;
// uniform sampler2D u_sampler_illu;

varying vec2 v_texCoord;
varying vec3 v_m;
varying vec3 v_n;


mat3 inverse(mat3 m)
{
	float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
	float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
	float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

	float b01 = a22 * a11 - a12 * a21;
	float b11 = -a22 * a10 + a12 * a20;
	float b21 = a21 * a10 - a11 * a20;

	float det = a00 * b01 + a01 * b11 + a02 * b21;

	return mat3(b01, (-a22 * a01 + a02 * a21), (a12 * a01 - a02 * a11),
				b11, (a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
				b21, (-a21 * a00 + a01 * a20), (a11 * a00 - a01 * a10)) / det;
}

vec3 phong()
{
	mat4 view = u_view;
	vec3 lightPos = u_lightPos;
	vec3 lightCol = u_lightCol;
	vec3 lightAtt = u_lightAtt;
	float lightPower = lightAtt[0];
	float shininess = lightAtt[1];
	
	vec2 tex = v_texCoord;
	vec3 color = u_color * gl_Color.xyz;
	vec3 frag_pos = v_m;
	vec3 normal = v_n;
	vec3 light_dir = lightPos - frag_pos;
	float dist = length(light_dir);
	dist = dist * dist;
	light_dir = normalize(light_dir);
	
	// TBN Matrix for computation in tangent space
	vec3 n_z = normal;
	vec3 n_x = vec3(1.0,0.0,0.0);
	vec3 n_y = cross(n_z,n_x); n_y=normalize(n_y);
	n_x = cross(n_y,n_z); n_x=normalize(n_x);
	mat3 tbn = inverse( mat3(n_x,n_y,n_z) );
	
	vec4 frag_n = texture2D(u_sampler_normal,tex);
	normal = vec3(frag_n[0],frag_n[1],frag_n[2]) * 2.0 - 1.0;
	
	float lambertian = max(dot(light_dir,normal),0.f);
	float specular = 0.0;
	
	if(lambertian > 0.0)
	{
		// view dir
		vec3 view_pos = tbn * -view[3].xyz;
		vec3 view_dir = view_pos - frag_pos;
		view_dir=normalize(view_dir);
		
		// blinn phong
		light_dir = tbn * light_dir; 
		vec3 halfDir = light_dir + view_dir;
		float specAngle = max( dot(halfDir,normal), 0.f );
		specular = pow(specAngle, shininess);
	}
	
	// Color model :
	// ambiantColor
	// + diffColor * lambertian * lightColor * lightPower / distance
	// + specColor * specular * lightColor * lightPower / distance
	
	vec4 frag_c = texture2D(u_sampler_color, tex);
	color *= vec3(frag_c[0],frag_c[1],frag_c[2]);
	
	vec3 colModelRes = color*0.01
	+ color*0.7 * lambertian * lightCol * lightPower / dist
	+ color*0.3 * specular * lightCol * lightPower / dist;
	
	return clamp( colModelRes,0.0,1.0 );
}

void main()
{
	vec3 c = phong();
	c *= 1.0;
	c = clamp(c,0.0,1.0);
	gl_FragData[0] = vec4(c,1.0);
}
