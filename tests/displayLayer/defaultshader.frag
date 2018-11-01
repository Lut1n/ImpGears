
varying vec2 v_texCoord;
uniform sampler2D u_colorTexture;

void main(){

	vec4 texColor = texture2D(u_colorTexture, v_texCoord);
	
    gl_FragData[0] = texColor * gl_Color;
}
