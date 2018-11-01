
varying vec2 v_texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main(){

    gl_Position = gl_Vertex - vec4(0.5,0.5,0.0,0.0);
    gl_Position.xy *= 2.0;

    gl_FrontColor = vec4(1.f, 1.f, 1.f, 1.f);
    v_texCoord = vec2(gl_MultiTexCoord0);
}
