

cbuffer MVP : register(b0){
	matrix MVP;
}

struct VsInput{
	float2 pos : POSITION;
};

struct PsInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PsInput main(VsInput input){
	PsInput output;

	output.pos = mul(float4(input.pos, 0.0f, 1.0f), MVP);
	output.tex.x = input.pos.x + 0.5f;
	output.tex.y = (input.pos.y - 0.5f) * -1.0f;

	return output;
}