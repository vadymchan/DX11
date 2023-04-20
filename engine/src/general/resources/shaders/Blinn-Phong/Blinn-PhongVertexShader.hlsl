
cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

cbuffer PerMesh : register(b2)
{
    float4x4 meshToModel;
}

struct Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float4x4 instance : INSTANCE;
};

struct Output
{
    float4 Position : SV_POSITION;
    float4 ViewPosition : ViewPosition;
    float4 WorldPosition : WorldPosition;
    float3 Normal : NORMAL;
    float3 WorldNormal : WORLDNORMAL;
    float2 UV : TEXCOORD;

};


Output main(Input input)
{
    Output output;

    output.WorldPosition = mul(mul(float4(input.position, 1),
    meshToModel), input.instance);
    output.ViewPosition = mul(output.WorldPosition, View);
    output.Position = mul(output.ViewPosition, Proj);
    
    //output.ViewPosition = mul(mul(float4(input.position, 1),
    //meshToModel), input.instance);
    //output.Position = mul(mul(output.ViewPosition, View), Proj);
    
    
    output.Normal = mul(mul(mul(float4(input.normal, 0),
    meshToModel),
    input.instance),
    View);
    
    output.WorldNormal = mul(mul(float4(input.normal, 0),
    meshToModel),
    input.instance);
    
    output.UV = input.texCoord;
    return output;
}

