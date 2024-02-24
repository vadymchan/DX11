
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
    float4x4 invInvstance : INVINSTANCE;
};

struct Output
{
    float4 Position : SV_POSITION;
    float4 ViewPosition : VIEWPOSITION;
    float4 WorldPosition : WORLDPOSITION;
    float3 Normal : NORMAL;
    float3 WorldNormal : WORLDNORMAL;
    float2 UV : TEXCOORD;
    float2 test : TEXCOORD1;
};


Output main(Input input)
{
    Output output;

    output.WorldPosition = mul( mul(
        float4(input.position, 1),
        meshToModel), 
        input.instance);
    
    float test = View._21;
    float test1 = View._12;
    
    output.test = float2(test, test1);
    
    output.ViewPosition = mul(output.WorldPosition, View);
    
    output.Position = mul(output.ViewPosition, Proj);
    
    output.Normal = normalize(mul(mul(mul(
        float4(input.normal, 0),
        meshToModel),
        input.instance),
        View));
   
    output.WorldNormal = normalize(mul(mul(
        float4(input.normal, 0),
        meshToModel),
        input.instance));
    
    output.UV = input.texCoord;
    return output;
}

