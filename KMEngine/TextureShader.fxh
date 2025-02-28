Texture2D txDiffuse;
SamplerState samLinear;

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;

    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    // Scale the texture coordinates to tile the texture twice
    float2 tiledUV = frac(input.Tex * 2.0);
    return txDiffuse.Sample(samLinear, tiledUV);
}