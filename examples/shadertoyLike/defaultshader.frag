uniform float u_timer;
uniform sampler2D u_sampler_color;

float hash(vec3 uvw, float seed){
    return fract(sin(seed + dot(uvw ,vec3(12.9898,8.233936,3.672893))) * 43758.5453);
}

float noise(vec3 uvw, float freq)
{

	vec3 i = floor(uvw * freq);
	vec3 f = fract(uvw * freq);
	
	float v0 = hash((i+vec3(0.0,0.0,0.0)), 0.0);
	float v1 = hash((i+vec3(1.0,0.0,0.0)), 0.0);
	float v2 = hash((i+vec3(0.0,1.0,0.0)), 0.0);
	float v3 = hash((i+vec3(1.0,1.0,0.0)), 0.0);
	float v4 = hash((i+vec3(0.0,0.0,1.0)), 0.0);
	float v5 = hash((i+vec3(1.0,0.0,1.0)), 0.0);
	float v6 = hash((i+vec3(0.0,1.0,1.0)), 0.0);
	float v7 = hash((i+vec3(1.0,1.0,1.0)), 0.0);
	
	f = 3.0*f*f - 2.0*f*f*f;
	
	float v01 = mix(v0,v1,f.x);
	float v23 = mix(v2,v3,f.x);
	float v45 = mix(v4,v5,f.x);
	float v67 = mix(v6,v7,f.x);
	
	float v0123 = mix(v01,v23,f.y);
	float v4567 = mix(v45,v67,f.y);
	
    return mix(v0123,v4567,f.z);
}

float valuenoise(vec3 uvw, int octave, float persist)
{
	float r = 0.0;
	float intensity = 1.0;
	float freq = 1.0;
	float imax = 0.0;
	for(int i=0; i<octave; i++)
	{
		r += noise(uvw, freq) * intensity;
		imax += intensity;
		intensity *= persist;
		freq *= 2.0;
	}
	
	return r/imax;
}


vec4 mainNoise(vec3 uvw, int octave)
{
    vec3 color = vec3(valuenoise(uvw, octave, 0.8));
    vec3 c1 = vec3(1.0,0.0,0.0);
    vec3 c2 = vec3(0.1,0.2,0.3);
    float f = pow(length(color),8.0);
    color = mix(c1,c2,f);
    return vec4(color,1.0);
}

float frac(float f)
{
    return f - floor(f);
}

vec4 textureColor(vec2 uv)
{
    vec3 uvw = vec3(uv,u_timer * 0.02);
    vec3 uvw2 = mainNoise(uvw,10).xyz;
    uvw += uvw2 * 0.05;
	return texture2D(u_sampler_color,uvw.xy).xyzw;
}

vec3 textureNormal(vec2 uv)
{
	return vec3(0.0,0.0,1.0);
}

float textureReflectivity(vec2 uv){ return 0.0; }

vec4 textureEmissive(vec2 uv)
{
	// return vec3(0.0);
    vec3 uvw = vec3(uv,u_timer * 0.02);
    return mainNoise(uvw,10);
}

