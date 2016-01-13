Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

cbuffer Cbuffer0 : register(b0) {
	matrix ColorMtrx;
};

struct PsInput {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 tex : TEXCOORD0;
};

float4 main(PsInput input) : SV_TARGET{
	//float3 normal = input.normal;

	//normal += 1;
	//normal *= 0.5f;

	//float4 color = float4(normal.x, normal.y, normal.z, 1);
	//return color;

	float4 color = tex.Sample(texSampler, input.tex);

	return color;
}