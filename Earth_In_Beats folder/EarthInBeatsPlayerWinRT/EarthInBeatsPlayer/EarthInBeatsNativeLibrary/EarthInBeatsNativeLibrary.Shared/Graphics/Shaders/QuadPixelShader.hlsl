Texture2D Tex : register(t0);
SamplerState TexSampler : register(s0);

struct PsInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PsInput input) : SV_TARGET{
	float4 color;

	color = Tex.Sample(TexSampler, input.tex);

	return color;
}