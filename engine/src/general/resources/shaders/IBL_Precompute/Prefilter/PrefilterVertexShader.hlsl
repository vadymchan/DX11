


cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

struct Input
{
    float3 Pos : POSITION;
    //float3 TexCoord : TEXCOORD0;
};

struct Output
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : WORLD_POSITION;
    //float3 TexCoord : TEXCOORD0;
};

Output main(Input input)
{
    Output output;

    output.WorldPos = input.Pos;
    
    output.Pos = mul(float4(input.Pos, 1.0), View);
    output.Pos = mul(output.Pos, Proj);
    
    //output.TexCoord = input.TexCoord;

    return output;
}
