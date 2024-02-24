

cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

cbuffer PerMesh : register(b2)
{
    float4x4 meshToModel;
};


struct Input
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    float4x4 instance : INSTANCE;
    float time : TIME;
};

struct Output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float time : TIME;
};





Output main(Input input)
{
    Output output;

    output.position = float4(input.position.xyz, 1),
    output.position = mul(output.position, meshToModel);
    output.position = mul(output.position, input.instance);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Proj);

    output.time = input.time;
    output.uv = input.uv;
    
    return output;
}



