varying vec2 v_texCoord;
uniform sampler2D u_sampler;

void main()
{
    vec4 uvw = texture2D(u_sampler, v_texCoord);
    gl_FragData[0] = vec4(uvw.xyz, 1.0);
}

