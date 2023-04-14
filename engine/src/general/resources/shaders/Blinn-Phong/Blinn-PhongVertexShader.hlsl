
cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

cbuffer PerMesh : register(b2)
{
    float4x4 meshToModel;
}

struct vertexInput
{

    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float4x4 instance : INSTANCE;
};

struct vertexOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;

};


vertexOutput main(vertexInput input)
{
    vertexOutput output;

    
    
    output.Position = mul(mul(mul(mul(float4(input.position, 1),
    meshToModel), input.instance), View), Proj);
    output.UV = input.texCoord;
   
    return output;
}

