cbuffer MVPConstantBuffer : register(b0){
	matrix model;
	matrix view;
	matrix projection;
};

struct VsInput{
	float3 pos : POSITION;
	float3 normal : NORMAL0;
};

struct PsInput{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
};

PsInput main(VsInput input){
	PsInput output;

	float4 pos = float4(input.pos, 1.0f);
	float4 normal = float4(input.normal, 0.0f);

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.pos = pos;
	output.normal = normal;

	return output;
}
