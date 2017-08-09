#include "Gui/GuiShader.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiShader::GuiShader():
    Shader::Shader(vertexCodeSource, fragmentCodeSource)
{
}

//--------------------------------------------------------------
GuiShader::~GuiShader()
{
}

//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
const char* GuiShader::vertexCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main(){

    gl_Position = u_projection * u_view * u_model * gl_Vertex;

    gl_FrontColor = vec4(1.f, 1.f, 1.f, 1.0);
    v_texCoord = vec2(gl_MultiTexCoord0);
}

);

//--------------------------------------------------------------
/// =========== FRAGMENT SHADER SOURCE =====================
const char* GuiShader::fragmentCodeSource = IMP_GLSL_SRC(

varying vec2 v_texCoord;
uniform sampler2D u_texture;
uniform vec3 u_color;
uniform float u_type;
uniform float u_sizeX;
uniform float u_sizeY;
uniform float u_outlineWidth;
uniform vec3 u_outlineColor;
uniform float u_opacity;
uniform float u_cornerRadius;

vec4 makeCorner()
{
	float outline = 0.0;
	float alpha = 1.0;
	vec2 pxCoord = v_texCoord.xy*vec2(u_sizeX,u_sizeY);
	
	// top left
	if( pxCoord.x < u_cornerRadius && pxCoord.y < u_cornerRadius )
	{
		float d = distance(pxCoord, vec2(u_cornerRadius) );
		if(d > u_cornerRadius)
		{
			alpha = 0.0;
		}
		else if( d > u_cornerRadius-u_outlineWidth )
		{
			outline = 1.0;
		}
	}
	// top right
	else if( pxCoord.x > u_sizeX-u_cornerRadius && pxCoord.y < u_cornerRadius )
	{
		float d = distance(pxCoord, vec2(u_sizeX-u_cornerRadius, u_cornerRadius) );
		if(d > u_cornerRadius)
		{
			alpha = 0.0;
		}
		else if( d > u_cornerRadius-u_outlineWidth )
		{
			outline = 1.0;
		}
	}
	// bottom left
	if( pxCoord.x < u_cornerRadius && pxCoord.y > u_sizeY-u_cornerRadius )
	{
		float d = distance(pxCoord, vec2(u_cornerRadius, u_sizeY-u_cornerRadius) );
		if(d > u_cornerRadius)
		{
			alpha = 0.0;
		}
		else if( d > u_cornerRadius-u_outlineWidth )
		{
			outline = 1.0;
		}
	}
	// bottom right
	else if( pxCoord.x > u_sizeX-u_cornerRadius && pxCoord.y > u_sizeY-u_cornerRadius )
	{
		float d = distance(pxCoord, vec2(u_sizeX-u_cornerRadius, u_sizeY-u_cornerRadius) );
		if(d > u_cornerRadius)
		{
			alpha = 0.0;
		}
		else if( d > u_cornerRadius-u_outlineWidth )
		{
			outline = 1.0;
		}
	}
	
	return vec4(outline, outline, outline, alpha);
}

void main(){

	vec4 texColor = vec4( u_color, u_opacity );
	vec4 corner = makeCorner();
	
	if(u_type == 0.0)
	{
		texColor *= texture2D(u_texture, v_texCoord);
	}
	else
	{
		if(v_texCoord.x < u_outlineWidth/u_sizeX || v_texCoord.y < u_outlineWidth/u_sizeY
			|| v_texCoord.x > 1.0-u_outlineWidth/u_sizeX || v_texCoord.y > 1.0-u_outlineWidth/u_sizeY
			|| corner.x == 1.0)
			texColor.xyz = u_outlineColor;
	}
	
	texColor.w *= corner.w;
	
    gl_FragData[0] = texColor * gl_Color;
}

);

IMPGEARS_END
