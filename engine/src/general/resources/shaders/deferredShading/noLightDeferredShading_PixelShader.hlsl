#include "../globals.hlsli"


struct Input
{
    float4 viewPos : VIEWPOS;
    float4 worldPos : WORLDPOS;
    float4 screenPos : SV_POSITION;
    float3 normal : NORMAL;
    float3 emission : EMISSION;
    uint InstanceID : INSTANCE_ID;
};

struct Output
{
    float3 Emission : SV_Target0;
    uint InstanceID : SV_Target1;
};

Output main(Input input) : SV_TARGET
{
    const float EMISSION_INTENSITY = 500.0;
    
    Output output;
    
    float3 normal = normalize(input.normal);
    float3 cameraDir = normalize(g_cameraPosition.xyz - input.worldPos.xyz);
    float3 normedEmission = input.emission / max(input.emission.x, max(input.emission.y, max(input.emission.z, 1)));
    float NoV = dot(cameraDir, normal);
    float3 finalColor = lerp(normedEmission, input.emission * EMISSION_INTENSITY, pow(max(0.0, NoV), 8.0));

    output.Emission = finalColor;
    output.InstanceID = input.InstanceID;
    
    return output;
}


