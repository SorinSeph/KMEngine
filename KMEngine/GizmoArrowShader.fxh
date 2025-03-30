cbuffer SArrowConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    int mIsHovered;
    int Padding[3];
}

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output;
    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    if (mIsHovered == 1)
    {
        output.Color = float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        output.Color = Color;
    }
       
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}