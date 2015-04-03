
static const float PixelSize = 1.0f / 1024.0f;

static const float B = 1.0f, C = 0.0f;// cubic B - spline
//static const float B = 1.0f / 3.0f, C = 1.0f / 3.0f;// recommended
//static const float B = 0.0f, C = 1.0f / 2.0f;// Catmull - Rom spline


Texture2D fontTexture : register(t0);
SamplerState fontSampler : register(s0);

struct PsInput{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float MitchellNetravali(float x);

float4 main(PsInput input) : SV_TARGET{
	float c = input.tex.y;
	float4 color;

	/*color = fontTexture.Sample(fontSampler, input.tex);
	color.rgb = color.r;*/

	//float pixSz = PixelSize;// *1.0f / 0.15f;

	//c = 0.0f;
	//c += fontTexture.Sample(fontSampler, float2(input.tex.x - pixSz, input.tex.y + pixSz));
	//c += fontTexture.Sample(fontSampler, float2(input.tex.x, input.tex.y + pixSz));
	//c += fontTexture.Sample(fontSampler, float2(input.tex.x + pixSz, input.tex.y + pixSz));

	//c += fontTexture.Sample(fontSampler, float2(input.tex.x - pixSz, input.tex.y));
	//c += fontTexture.Sample(fontSampler, float2(input.tex.x, input.tex.y));
	//c += fontTexture.Sample(fontSampler, float2(input.tex.x + pixSz, input.tex.y));

	//c += fontTexture.Sample(fontSampler, float2(input.tex.x - pixSz, input.tex.y - pixSz));
	//c += fontTexture.Sample(fontSampler, float2(input.tex.x, input.tex.y - pixSz));
	//c += fontTexture.Sample(fontSampler, float2(input.tex.x + pixSz, input.tex.y - pixSz));

	//c /= 9.0f;

	//c = fontTexture.Sample(fontSampler, input.tex);

	/*float realPos = (input.tex.x * 1024.0f) % 4;

	if (realPos == 0){
		c = MitchellNetravali(input.tex.x + 1);
	}
	else if (realPos == 1){
		c = MitchellNetravali(input.tex.x);
	}
	else if (realPos == 2){
		c = MitchellNetravali(input.tex.x - 1);
	}
	else if (realPos == 3){
		c = MitchellNetravali(input.tex.x - 2);
	}*/

	return float4(c, c, c, 1.0f); // color;//
}


float MitchellNetravali(float x)
{
	float ax = abs(x);
	if (ax < 1) {
		return ((12 - 9 * B - 6 * C) * ax * ax * ax +
			(-18 + 12 * B + 6 * C) * ax * ax + (6 - 2 * B)) / 6;
	}
	else if ((ax >= 1) && (ax < 2)) {
		return ((-B - 6 * C) * ax * ax * ax +
			(6 * B + 30 * C) * ax * ax + (-12 * B - 48 * C) *
			ax + (8 * B + 24 * C)) / 6;
	}
	else {
		return 0;
	}
}