
cbuffer Buf : register(b0){
	matrix Transform;
}

struct VSInput{
	float2 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PSInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PSInput main(VSInput input){
	PSInput output;

	output.pos = mul(float4(input.pos.xy, 0, 1), Transform);
	output.tex = input.tex;

	return output;
}