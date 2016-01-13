//// https://www.shadertoy.com/view/XlfGRj

static const int	ITERATIONS = 1; // 17;
static const int	VOLSTEPS = 1; // 20;
static const float	FORMUPARAM = 0.53f;
static const float	STEPSIZE = 0.1f;
static const float	ZOOM = 0.8f;
static const float	TILE = 0.85;
static const float	SPEED = 0.0f;
static const float	BRIGHTNESS = 0.0015f;
static const float	DARKMATTER = 0.3f;
static const float	DISTFADING = 0.73f;
static const float	SATURATION = 0.85f;

//// inputs
cbuffer Cbuffer0 : register(b0) {
	float3 iResolution;
	float  iGlobalTime;
};

struct PsInput {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

// TODO Optimizing. decrease number of arithmetic instruction slots
float4 main(PsInput input) : SV_TARGET{
	//get coords and direction
	float2 uv = input.tex.xy / iResolution.xy - 0.5f;
	uv.y *= iResolution.y / iResolution.x;
	float3 dir = float3(uv * ZOOM, 1.0f);
	float time = iGlobalTime * SPEED + 0.25f;
	float3 from = float3(1.0f, 0.5f, 0.5f);
	from += float3(time * 2.0f, time, -2.0f);

	//volumetric rendering
	float s = 0.1f, fade = 1.0f;
	float3 v = float3(0.0f, 0.0f, 0.0f);

	[unroll(VOLSTEPS)]
	for (int r = 0; r < VOLSTEPS; r++) {
		float3 p = from + s * dir * 0.5f;
		p = abs(float3(TILE, 0.0f, 0.0f) - fmod(p, float3(TILE * 2.0f, 0.00001f, 0.00001f))); // tiling fold
		float a = 0.0f, pa = 0.0f;
		
		[unroll(ITERATIONS)]
		for (int i = 0; i < ITERATIONS; i++) {
			p = abs(p) / dot(p, p) - FORMUPARAM; // the magic formula
			float pLength = length(p); // optimization
			a += abs(pLength - pa); // absolute sum of average change
			pa = pLength;
		}

		float dm = max(0.0f, DARKMATTER - a * a * 0.001f); //dark matter
		a *= a * a; // add contrast

		if (r > 6) {				// dark matter, don't render near
			fade *= 1.0f - dm;		//v+=vec3(dm,dm*.5,0.);
		}
								  
		v += fade;
		v += float3(s, s * s, s * s * s * s) * a * BRIGHTNESS * fade; // coloring based on distance
		fade *= DISTFADING; // distance fading
		s += STEPSIZE;
	}

	float3 tmp = float3(length(v), 0.0f, 0.0f);
	v = tmp * SATURATION + (1 - SATURATION) * v;	//color adjust

	float4 outputColor = float4(v * 0.01f, 1.0f);

	return outputColor;
}