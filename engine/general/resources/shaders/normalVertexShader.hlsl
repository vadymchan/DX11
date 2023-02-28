
cbuffer PerView : register(b0)
{
    float4x4 VP;
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
    float3 normal : NORMAL;

};


vertexOutput main(vertexInput input)
{
    vertexOutput output;

    
    //output.position = mul(mul(float4(input.position, 1), input.instance), VP);
    output.position = mul(mul(mul(float4(input.position, 1),
    meshToModel), input.instance), VP);
    

   
    return output;
}

