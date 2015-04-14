
static const float PixelSize = 1.0f / 1024.0f;

static const float B = 1.0f, C = 0.0f;// cubic B - spline
//static const float B = 1.0f / 3.0f, C = 1.0f / 3.0f;// recommended
//static const float B = 0.0f, C = 1.0f / 2.0f;// Catmull - Rom spline


Texture2D fontTexture : register(t0);
SamplerState fontSampler : register(s0);

struct PsInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PsInput input) : SV_TARGET{
	float c = input.tex.y;
	float4 color;

	return float4(c, c, c, 1.0f); // color;//
}