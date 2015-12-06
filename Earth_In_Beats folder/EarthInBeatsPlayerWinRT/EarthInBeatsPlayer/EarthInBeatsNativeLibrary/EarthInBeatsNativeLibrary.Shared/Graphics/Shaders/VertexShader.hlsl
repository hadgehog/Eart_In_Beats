cbuffer MVPConstantBuffer : register(b0){
	matrix model;
	matrix view;
	matrix projection;
};

// TODO add texture coord
struct VsInput{
	float3 pos : POSITION;
	float3 normal : NORMAL0;
	float2 tex : TEXCOORD0;
};

// TODO add texture coord
struct PsInput{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 tex : TEXCOORD0;
};

PsInput main(VsInput input){
	PsInput output;

	float4 pos = float4(input.pos, 1.0f);
	float3 normal = input.normal;

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	//normal = mul(normal, model);
	//normal = mul(normal, view);

	output.pos = pos;
	output.normal = normal;
	output.tex = input.tex;

	return output;
}
