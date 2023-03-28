

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
    float4x4 instance : INSTANCE;
};

struct vertexOutput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;

};

const static float NORMAL_LENGTH = 0.05;

vertexOutput main(vertexInput input)
{
    vertexOutput output;

    output.position = mul(mul(mul(mul(float4(input.position, 1),
    meshToModel), 
    input.instance), 
    View),
    Proj);
  
    output.normal = mul(mul(mul(mul(float4(input.normal * NORMAL_LENGTH, 0),
    meshToModel),
    input.instance),
    View),
    Proj);
    return output;
}

