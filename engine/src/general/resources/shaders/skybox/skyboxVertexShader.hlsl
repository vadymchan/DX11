
cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

cbuffer InvPerView : register(b1)
{
    float4x4 invView;
    float4x4 invProj;
}

struct Input
{
    uint vertexID : SV_VertexID;
};

struct Output
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;
    
};

Output main(Input input)
{
    Output output;
    float x = float((input.vertexID & 2) * 2) - 1;
    float y = float((input.vertexID & 1) * 4) - 1;
    output.position = float4(x, y, 0, 1);
    
    output.tex = float3(output.position.x, output.position.y, 1);
    output.tex = mul(output.tex, (float3x3)invView);

    return output;
}
