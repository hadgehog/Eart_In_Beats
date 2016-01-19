cbuffer MVPConstantBuffer : register(b0) {
	matrix model;
	matrix view;
	matrix projection;
};

struct VsInput {
	float3 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PsInput {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PsInput main(VsInput input) {
	PsInput output;

	float4 pos = float4(input.pos, 1.0f);

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.pos = pos;
	output.tex = input.tex;

	return output;
}