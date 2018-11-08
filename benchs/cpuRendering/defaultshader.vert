
varying vec2 v_texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

float t3(mat4 m, float f) { float r = 0.5; r *= f; m *= r; vec4 v = vec4(r); v = v * m; return length(v) * f; }

void main(){

	mat4 t = u_projection * u_model; float t2 = 0.0; t2 = t3(t,t2);
	
    gl_Position = gl_Vertex - vec4(0.5,0.5,0.0,0.0);
    gl_Position.xy *= 2.0 + t2;

    gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);
    v_texCoord = vec2(gl_MultiTexCoord0);
}
