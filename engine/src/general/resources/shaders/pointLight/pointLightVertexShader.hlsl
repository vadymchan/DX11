#include "../globals.hlsl"

cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};



struct Input
{
    //per-vertex
    float3 position : POSITION;
    float3 normal : NORMAL;
    //per-instance
    float4x4 instance : INSTANCE;
    float4 emission : EMISSION; 
};

struct Output
{
    float4 viewPos : VIEWPOS;
    float4 screenPos : SV_POSITION;
    float3 normal : NORMAL;
    float3 emission : EMISSION;
};

Output main(Input input)
{
    Output output;
    output.viewPos = mul(mul(float4(input.position, 1), input.instance), View);
    output.screenPos = mul(output.viewPos, Proj);
    output.normal = mul(mul(float4(input.normal, 0), input.instance), View);
    output.emission = input.emission.xyz;

    return output;
}



