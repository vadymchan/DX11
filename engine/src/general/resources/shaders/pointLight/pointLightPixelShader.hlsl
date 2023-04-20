#include "../globals.hlsl"

struct Input
{
    float4 viewPos : VIEWPOS;
    float4 screenPos : SV_POSITION;
    float3 normal : NORMAL;
    float3 emission : EMISSION;

};

float3 main(Input input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 cameraDir = normalize(-input.viewPos.xyz);
    float3 normedEmission = input.emission / max(input.emission.x, max(input.emission.y, max(input.emission.z, 1.0)));
    float NoV = dot(cameraDir, normal);
    return lerp(normedEmission * 0.33, input.emission, pow(max(0.0, NoV), 8.0));

}




//float3 main(Input input) : SV_TARGET
//{
//    float3 normal = normalize(input.normal);
//    float3 cameraDir = normalize(g_cameraPosition.xyz - input.worldPos.xyz);
//    float3 position = input.viewPos.xyz;

//    // Emission calculations
//    float3 normedEmission = input.emission / max(input.emission.x, max(input.emission.y, max(input.emission.z, 1.0)));
//    float NoV = dot(cameraDir, normal);
//    float3 emissiveColor = lerp(normedEmission * 0.33, input.emission, pow(max(0.0, NoV), 8.0));

//    // Fixed color for the surface
//    float3 surfaceColor = float3(1.0, 1.0, 1.0); // You can replace this with your desired color

//    // Blinn-Phong calculations
//    float3 finalColor = emissiveColor;

//    for (int i = 0; i < g_numDirectionalLights; ++i)
//    {
//        float3 lightResult = CalculateDirectionalLight(normal, g_directionalLights[i], position);
//        finalColor += surfaceColor * lightResult;
//    }

//    for (int j = 0; j < g_numPointLights; ++j)
//    {
//        float3 lightResult = CalculatePointLight(normal, position, g_pointLights[j]);
//        finalColor += surfaceColor * lightResult;
//    }

//    for (int m = 0; m < g_numSpotLights; ++m)
//    {
//        float3 lightResult = CalculateSpotLight(normal, position, g_spotLights[m]);
//        finalColor += surfaceColor * lightResult;
//    }

//    for (int k = 0; k < g_numFlashLights; ++k)
//    {
//        float3 lightResult = CalculateFlashLight(normal, position, g_flashLights[k]);
//        finalColor += surfaceColor * lightResult;
//    }

//    // Combine Blinn-Phong and emissive colors
//    float3 combinedColor = finalColor + emissiveColor;

//    return combinedColor;
//}
