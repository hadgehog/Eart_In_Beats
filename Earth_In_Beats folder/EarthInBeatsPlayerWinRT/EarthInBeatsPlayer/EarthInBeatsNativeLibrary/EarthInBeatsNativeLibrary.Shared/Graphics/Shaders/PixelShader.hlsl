cbuffer Cbuffer0 : register(b0) {
	matrix ColorMtrx;
};

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

// TODO add texture coord
struct PsInput {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
};

float4 main(PsInput input) : SV_TARGET{
	/*float4 color = tex.Sample(texSampler, input.tex);

	color = mul(color, ColorMtrx);

	return color;*/

	float3 normal = input.normal;

	normal += 1;
	normal *= 0.5f;

	float4 color = float4(normal.x, normal.y, normal.z, 1);
	return color;
}