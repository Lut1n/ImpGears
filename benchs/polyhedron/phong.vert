#version 130

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
