

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
    float4x4 instance : INSTANCE;
};

struct vertexOutput
{
    float4 position : SV_POSITION;
};





vertexOutput main(vertexInput input)
{
    vertexOutput output;

    output.position = float4(input.position.xyz, 1),
    output.position = mul(output.position, meshToModel);
    output.position = mul(output.position, input.instance);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Proj);

    return output;
}



