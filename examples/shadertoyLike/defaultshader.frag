varying vec2 v_texCoord;
uniform float u_timer;

float hash(vec2 uv, float seed)
{
    return fract( sin(seed + dot(uv, vec2(76.3756, 23.0946))) * 1342.45641 );
}

float noise(vec2 uv, float freq)
{

	vec2 i = floor(uv * freq);
	vec2 f = fract(uv * freq);
	
	float v0 = hash((i+vec2(0.0,0.0))/freq, 87.1385);
	float v1 = hash((i+vec2(1.0,0.0))/freq, 87.1385);
	float v2 = hash((i+vec2(0.0,1.0))/freq, 87.1385);
	float v3 = hash((i+vec2(1.0,1.0))/freq, 87.1385);
	
	f = 3.0*f*f - 2.0*f*f*f;
	
	float v01 = mix(v0,v1,f.x);
	float v23 = mix(v2,v3,f.x);
	
    return mix(v01,v23,f.y);
}

float valuenoise(vec2 uv, int octave, float persist)
{
	float r = 0.0;
	float intensity = 1.0;
	float freq = 1.0;
	float imax = 0.0;
	for(int i=0; i<octave; i++)
	{
		r += noise(uv, freq) * intensity;
		imax += intensity;
		intensity *= persist;
		freq *= 2.0;
	}
	
	return r/imax;
}

void mainImage(vec2 uv, float time, out vec3 color);

void main()
{
    vec3 color;
	mainImage( v_texCoord, u_timer * 0.001, color );
    gl_FragData[0] = vec4(color,1.0);
}


void mainImage(vec2 uv, float time, out vec3 color)
{
	vec2 cuv = uv - 0.5;
	cuv *= 2.0;
	
	
	vec2 disp = vec2(valuenoise(uv, 14, 0.8),valuenoise(uv.yx, 14, 0.8));
	float t = sin(time);
	float v = vec3(1.0-length(cuv+disp*t) );
	v = smoothstep(0.0,1.0,v);
	v = pow(v, 8.0) + v;
	color = smoothstep(0.0,1.0, v * vec3(0.3,0.3,1.0));
}