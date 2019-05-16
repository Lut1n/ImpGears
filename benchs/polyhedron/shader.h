#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
std::string fragSimple = GLSL_CODE
(

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

vec3 textureColor(vec2 uv)
{
	return vec3(fractal(vec3(uv,0.0),8,8.0,0.7,1.0));
}

vec3 textureNormal(vec2 uv){ return vec3(0.0,0.0,1.0); }
vec3 textureEmissive(vec2 uv){ return vec3(0.0,0.0,0.0); }

);
