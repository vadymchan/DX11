//#include "../globals.hlsl"

//cbuffer PerView : register(b0)
//{
//    float4x4 View;
//    float4x4 Proj;
//};

//struct VSInput
//{
//    float3 position : POSITION;
//    float3 normal : NORMAL;
//    float3 emission : EMISSION;
//};

//struct PSInput
//{
//    float4 worldPos : SV_POSITION;
//    float3 normal : NORMAL;
//    float3 emission : EMISSION;
//};

//PSInput mainVS(VSInput input)
//{
    
//    PSInput output;
//    float4 worldPos = float4(input.position, 1.0);
//    output.worldPos = mul(g_viewProjectionMatrix, worldPos);

//    return output;
//}

//float4 mainPS(PSInput input) : SV_TARGET
//{
//    float3 normal = normalize(input.normal);
//    float3 cameraDir = normalize(g_cameraPosition.xyz - input.worldPos.xyz);
//    float3 normedEmission = input.emission / max(input.emission.x,
//    max(input.emission.y, max(input.emission.z, 1.0)));
//    float NoV = dot(cameraDir, normal);
//    float3 emissiveColor = lerp(normedEmission * 0.33,
//    input.emission, pow(max(0.0, NoV), 8.0));

//    float3 toLight = g_lightPosition - input.worldPos.xyz;
//    float lightDistance = length(toLight);
//    float3 lightDir = normalize(toLight);
//    float lightIntensity = saturate(1.0 - (lightDistance / g_lightRadius));

//    float NdotL = dot(normal, lightDir);
//    float3 diffuse = saturate(NdotL) * g_lightColor * lightIntensity;

//    return float4(emissiveColor + diffuse, 1.0);
//}