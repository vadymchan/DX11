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

struct GS_IN
{
    float4 positionModel : POSITIONMODEL;
    float4 positionWorld : SV_POSITION;
    float4 normal : NORMAL;
};

struct GS_OUT
{
    float4 position : SV_Position;
    float4 normal : NORMAL;
};

static const float DISTORTION_SCALE = 1.0;

[maxvertexcount(3)]
void main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> output)
{
    
    float3 normal;
    //normal = normalize(normalize(input[0].normal + input[1].normal) + input[2].normal).xyz; - this is for triangle offset including tesselated vertices
    normal = normalize(cross(input[1].positionModel.xyz - input[0].positionModel.xyz, input[2].positionModel.xyz - input[0].positionModel.xyz));

    
    GS_OUT temp;
    
    //-----------------------------------------------------------------------------------------------------------------------------------
    //1 vertex
    //normal "explosion"
    //temp.position = mul(mul(input[0].positionWorld + float4(normal * time.x * 0.005 , 0),
    //View), Proj);
    
    // triangle normal offset
    temp.position = mul(mul(input[0].positionWorld + float4(vertexDistortion(input[0].positionModel.xyz, normal) * DISTORTION_SCALE, 0),
    View), Proj);
    
    temp.normal = input[0].normal;
    output.Append(temp);
    //-----------------------------------------------------------------------------------------------------------------------------------
    
    
    //-----------------------------------------------------------------------------------------------------------------------------------
    //2 vertex
    //normal "explosion"
    //temp.position = mul(mul(input[1].positionWorld + float4(normal * time.x * 0.005 , 0),
    //View), Proj);
    
    // triangle normal offset
    temp.position = mul(mul(input[1].positionWorld + float4(vertexDistortion(input[1].positionModel.xyz, normal) * DISTORTION_SCALE, 0),
    View), Proj);
    
    temp.normal = input[1].normal;
    output.Append(temp);
    //-----------------------------------------------------------------------------------------------------------------------------------
    
    
    //-----------------------------------------------------------------------------------------------------------------------------------
    //3 vertex
    
    //normal "explosion"
    //temp.position = mul(mul(input[2].positionWorld + float4(normal * time.x * 0.005 , 0),
    //View), Proj);
    
    // triangle normal offset
    temp.position = mul(mul(input[2].positionWorld + float4(vertexDistortion(input[2].positionModel.xyz, normal) * DISTORTION_SCALE, 0),
    View), Proj);
    
    temp.normal = input[2].normal;
    output.Append(temp);
    //-----------------------------------------------------------------------------------------------------------------------------------
    
    
}