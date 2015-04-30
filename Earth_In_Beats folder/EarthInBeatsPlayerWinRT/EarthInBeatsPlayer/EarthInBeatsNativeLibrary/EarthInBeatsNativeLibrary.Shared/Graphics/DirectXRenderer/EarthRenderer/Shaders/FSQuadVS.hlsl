
// TODO : check tex coords
static const float2 quadScale = float2(0.5f, 0.5f);
//static const float2 quadMove = float2(0.5f, -0.5f);
static const float2 quadMove = float2(0.5f, 0.5f);

cbuffer Buf : register(b0){
	matrix Transform;
}

struct VSInput{
	float2 pos : POSITION;
};

struct PSInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PSInput main(VSInput input){
	PSInput output;

	output.pos = mul(float4(input.pos.xy, 0, 1), Transform);
	output.tex = input.pos * quadScale + quadMove;
	//output.tex.y = -output.tex.y;

	return output;
}