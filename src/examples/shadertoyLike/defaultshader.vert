uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

varying vec2 v_texCoord;

void main()
{
    gl_Position = u_proj * u_view * u_model * gl_Vertex;

    gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);
    v_texCoord = vec2(gl_MultiTexCoord0);
}
