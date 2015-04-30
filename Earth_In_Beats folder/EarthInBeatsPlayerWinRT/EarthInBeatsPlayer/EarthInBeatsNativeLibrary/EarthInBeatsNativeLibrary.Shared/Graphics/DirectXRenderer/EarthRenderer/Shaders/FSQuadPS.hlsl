
SamplerState QuadSampler : register(s0);
Texture2D QuadTex : register(t0);

struct PSInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET{
	float4 color = QuadTex.Sample(QuadSampler, input.tex);
	return color;
}