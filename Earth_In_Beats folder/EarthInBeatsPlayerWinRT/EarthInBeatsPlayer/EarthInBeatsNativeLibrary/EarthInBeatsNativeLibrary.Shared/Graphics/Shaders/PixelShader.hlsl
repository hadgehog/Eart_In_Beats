struct PsInput{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL0;
	float4 color : COLOR0;
};

//////////
float4 main(PsInput input) : SV_TARGET{
	float4 color;

	color = float4(input.color.rgb, 1.0f);

	return color;
}