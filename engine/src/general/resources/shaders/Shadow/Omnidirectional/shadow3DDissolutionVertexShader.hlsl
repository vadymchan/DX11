
cbuffer PerMesh : register(b2)
{
    float4x4 meshToModel;
};


struct Input
{
    float3 position : POSITION;
    float4x4 instance : INSTANCE;
    float2 uv : TEXCOORD0;
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

    output.uv = input.uv;
    output.time = input.time;
    
    return output;
}



