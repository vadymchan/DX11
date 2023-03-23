#include "hologramShader.hlsli"

cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

cbuffer PerMesh : register(b2)
{
    float4x4 meshToModel;
};


struct vertexInput
{

    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float4x4 instance : INSTANCE;
};

struct vertexOutput
{
    float4 positionModel : POSITIONMODEL;
    float4 positionWorld : SV_POSITION;
    float4 normal : NORMAL;
};





vertexOutput main(vertexInput input)
{
    vertexOutput output;

    output.positionModel = mul(float4(input.position.xyz, 1),
    meshToModel);
    output.positionWorld = mul(output.positionModel, input.instance);
    output.normal = mul(mul(float4(input.normal, 0),
    meshToModel), input.instance);


    
    return output;
}



