//--------------------------------------------------------------------------------------
cbuffer ArrowConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    int Clicked;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    //Color = mul(Color, float4(0.0f, 0.0f, 0.5f, 0.f));
    if (Clicked != 0)
    {
        output.Color = float4(1.f, 0.0f, 0.0f, 1.f);
    }
    else
    {
        output.Color = float4(0.6f, 0.0f, 0.0f, 1.f);
    }
    //output.Color = mul(Color, float4(0.0f, 0.0f, 0.5f, 0.5f));
    return output;
}