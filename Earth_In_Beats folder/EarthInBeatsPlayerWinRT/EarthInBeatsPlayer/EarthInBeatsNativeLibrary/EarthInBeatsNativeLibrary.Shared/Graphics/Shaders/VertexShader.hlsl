cbuffer MVPConstantBuffer : register(b0){
	matrix model;
	matrix view;
	matrix projection;
};

struct VsInput{
	float3 pos : POSITION;
	float3 norm : NORMAL0;
};

struct PsInput{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL0;
};

PsInput main(VsInput input){
	PsInput output = (PsInput)0;

	float4 pos = float4(input.pos, 1.0f);
	float4 normal = float4(input.norm, 0.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);

	output.norm = mul(input.norm, model).xyz;

	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}
