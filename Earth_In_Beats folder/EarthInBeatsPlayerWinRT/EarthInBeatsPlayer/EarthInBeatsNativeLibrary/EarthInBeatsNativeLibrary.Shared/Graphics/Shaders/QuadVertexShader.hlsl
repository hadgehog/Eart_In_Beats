
static const float2 TexCoordScale = float2(1.0f, -1.0f);
static const float2 TexCoordOffset = float2(0.5f, 0.5f);

cbuffer Buf0 : register(b0){
	matrix MVP;
	float2 flip;
	float2 leftTopCrop;
	float2 sizeCrop;
	float2 maskSizeRatio;
}

struct VsInput{
	float2 pos : POSITION;
};

struct PsInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 maskTex : TEXCOORD1;
};

// multiply-add
float2 Mad(float2 v, float2 m, float2 a);
float2 GetTexCoords(float2 v);

PsInput main(VsInput input){
	PsInput output = (PsInput)0;

	output.pos = mul(float4(input.pos.xy, 0.0f, 1.0f), MVP);
	
	output.tex = GetTexCoords(input.pos * flip);
	output.maskTex = GetTexCoords(input.pos * maskSizeRatio);
	
	output.tex = Mad(output.tex, sizeCrop, leftTopCrop);

	return output;
}

float2 Mad(float2 v, float2 m, float2 a) {
	float2 res = v * m + a;
	return res;
}

float2 GetTexCoords(float2 v) {
	float2 res = Mad(v, TexCoordScale, TexCoordOffset);
	return res;
}