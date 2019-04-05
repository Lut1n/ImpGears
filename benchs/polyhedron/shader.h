#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
std::string vertexSimple = GLSL_CODE
(

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

// varying vec3 v_vertex;
varying float v_depth;
varying vec3 v_pos;

void main()
{
	// v_vertex = gl_Vertex.xyz;
	vec4 mv_pos = u_view * u_model * gl_Vertex;
	v_pos = gl_Vertex.xyz;
	gl_Position = u_projection * mv_pos;
	v_depth = length(mv_pos.xyz);
}

);

//--------------------------------------------------------------
std::string fragSimple = GLSL_CODE
(

// varying vec3 v_vertex;
varying float v_depth;
varying vec3 v_pos;
uniform vec3 u_color;

float hash(vec3 xyz, float seed)
{
    return  fract( sin( seed + dot(xyz, vec3(12.19823371,10.27653,35.746541)) )* 45758.2653872 );
}

float noise(vec3 xyz, float freq)
{
    vec3 iv = floor(xyz * freq);
    vec3 fv = fract(xyz * freq);
    
    fv = 3.0*fv*fv - 2.*fv*fv*fv;
    
	float v[8];
	
	for(int i=0;i<8;++i) v[i] = hash( (iv + vec3(mod(float(i/4),2.0),mod(float(i/2),2.0),mod(float(i),2.0)))/freq, 0.0 );
    
    float v01 = mix(v[0],v[1],fv.z);
    float v23 = mix(v[2],v[3],fv.z);
    float v45 = mix(v[4],v[5],fv.z);
    float v67 = mix(v[6],v[7],fv.z);
    
    float v0123 = mix(v01,v23,fv.y);
    float v4567 = mix(v45,v67,fv.y);
    
    return mix(v0123,v4567,fv.x);
}

float fractal(vec3 xyz, int octave, float freq, float persist, float force)
{
    const int MAX_OCTAVE = 20;
    float total = 0.0;
    // float freq = 1.0;
    // float persist = 0.7;
    
    float result = 0.0;
    for(int i=0; i<MAX_OCTAVE;++i)
    {
        result += noise(xyz,freq) * persist;
        
        total += persist;
        persist *= 0.5;
        freq *= 2.0;
		
		if(i >= octave) break;
    }
    
    return result/total;
}

void main()
{
	float fog_max = 5.0;
	float fog = min(v_depth,fog_max) / fog_max;
	
	float c = fractal(v_pos,8,8.0,0.7,1.0);
	c *= 1.0;
	c = clamp(c,0.0,1.0);
	
	gl_FragData[0] = vec4(u_color*(1.0-fog)*c,1.0);
}

);
