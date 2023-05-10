//#include "PBRLightCalculations.hlsl"
#include "../globals.hlsli"

Texture2D albedoTexture    : register(t0);
Texture2D roughnessTexture : register(t1);
Texture2D metalnessTexture : register(t2);
Texture2D normalTexture    : register(t3);

struct Input
{
    float4 Position : SV_POSITION;
    float4 ViewPosition : ViewPosition;
    float4 WorldPosition : WorldPosition;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float4x4 View : VIEW;
    
};

float4 main(Input input) : SV_TARGET
{
    float3 finalColor = float3(0, 0, 0);
    float4 albedo;
    float roughness;
    float metalness;

    switch (g_samplerStateIndex)
    {
        case 0:
            albedo = albedoTexture.Sample(g_pointWrap, input.UV);
            break;
        case 1:
            albedo = albedoTexture.Sample(g_linearWrap, input.UV);
            break;
        case 2:
            albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
            break;
        default:
            albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
            break;
    }
    
    roughness = roughnessTexture.Sample(g_anisotropicWrap, input.UV).r;
    metalness = metalnessTexture.Sample(g_anisotropicWrap, input.UV).r;

    //get normals from the normal map
    float2 normalTextureSample = normalTexture.Sample(g_anisotropicWrap, input.UV).xy;
    float normalZComponent = sqrt(1 - normalTextureSample.x * normalTextureSample.x - normalTextureSample.y * normalTextureSample.y);
    float3 normalFromNormalMap = float3(normalTextureSample, normalZComponent);
    // TBN matrix in world space
    float3x3 tangentSpaceMatrix = float3x3(input.Tangent, input.Bitangent, input.Normal);
    float3 worldNormal = normalize(mul(normalFromNormalMap, tangentSpaceMatrix));

    float3 viewNormal = normalize(mul(worldNormal, (float3x3) input.View));

    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float3 lightResult = CalculateDirectionalLightPBR(viewNormal, g_directionalLights[i], input.ViewPosition.xyz, roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int j = 0; j < g_numPointLights; ++j)
    {
        float3 lightResult = CalculatePointLightPBR(viewNormal, input.ViewPosition.xyz, g_pointLights[j], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }
    
    for (int k = 0; k < g_numSpotLights; ++k)
    {
        float3 lightResult = CalculateSpotLightPBR(viewNormal, input.ViewPosition.xyz, g_spotLights[k], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int l = 0; l < g_numFlashLights; ++l)
    {
        float3 lightResult = CalculateFlashLightPBR(worldNormal, input.WorldPosition.xyz, g_flashLights[l], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    return float4(finalColor, 0.5);
}
