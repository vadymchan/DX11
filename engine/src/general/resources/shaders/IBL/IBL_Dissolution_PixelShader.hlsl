#include "../globals.hlsli"
#include "IBL_common.hlsli"
#include "../Shadow/Shadow_common.hlsli"

Texture2D noiseTexture : register(t12);

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
    float time : TIME;
    
};


float4 main(Input input) : SV_TARGET
{

    // dissolution effect
    float noise = noiseTexture.Sample(g_anisotropicWrap, input.UV).r;
    const float noiseThreshold = 0.03;
    if (noise > input.time)
    {
        discard;
    }
    else if (noise > input.time - noiseThreshold)
    {
        const float EMISSION_INTENSITY = 3;
        return float4(1, 0, 0, 1) * EMISSION_INTENSITY;
    }
    
    float3 finalColor = float3(0, 0, 0);
    float4 albedo;
    float roughness;
    float metalness;

    albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
    roughness = useRoughnessOverwriting ? overwrittenRoughnessValue : roughnessTexture.Sample(g_anisotropicWrap, input.UV).r;
    metalness = metalnessTexture.Sample(g_anisotropicWrap, input.UV).r;

    //get normals from the normal map
    float2 normalTextureSample = normalTexture.Sample(g_anisotropicWrap, input.UV).xy;
    float normalZComponent = sqrt(1 - normalTextureSample.x * normalTextureSample.x - normalTextureSample.y * normalTextureSample.y);
    float3 normalFromNormalMap = float3(normalTextureSample, normalZComponent);
    // TBN matrix in world space
    float3x3 tangentSpaceMatrix = float3x3(input.Tangent, input.Bitangent, input.Normal);
    float3 worldNormal = normalize(mul(normalFromNormalMap, tangentSpaceMatrix));

    float3 viewNormal = normalize(mul(worldNormal, (float3x3) input.View));
    float3 viewMacroNormal = normalize(mul(input.Normal, (float3x3) input.View));


    
    // PBR
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------
    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float shadowFactor = CalculateDirectionalShadowFactorPCF(input.WorldPosition.xyz, input.Normal, g_directionalLights[i], i);
        float3 lightResult = CalculateDirectionalLightPBR(viewNormal, viewMacroNormal, g_directionalLights[i], input.ViewPosition.xyz, roughness, metalness, albedo.rgb);
    
        finalColor += lightResult * shadowFactor;
    }

    
    for (int j = 0; j < g_numPointLights; ++j)
    {
        float shadowFactor = CalculatePointLightShadowFactor(input.WorldPosition.xyz, input.Normal, g_pointLights[j], j);
        float3 lightResult = CalculatePointLightPBR(worldNormal, input.Normal, input.WorldPosition.xyz, g_pointLights[j], roughness, metalness, albedo.rgb);
        
        finalColor += lightResult * shadowFactor;
    }

    for (int k = 0; k < g_numSpotLights; ++k)
    {
        float shadowFactor = CalculateSpotLightShadowFactor(input.WorldPosition.xyz, input.Normal, g_spotLights[k], k);
        float3 lightResult = CalculateSpotLightPBR(viewNormal, viewMacroNormal, input.ViewPosition.xyz, g_spotLights[k], roughness, metalness, albedo.rgb);

        finalColor += lightResult * shadowFactor;
    }

    for (int l = 0; l < g_numFlashLights; ++l)
    {
        float shadowFactor = CalculateFlashLightShadowFactor(input.WorldPosition.xyz, input.Normal, g_flashLights[l], l);
        float3 lightResult = CalculateFlashLightPBR(worldNormal, input.Normal, input.WorldPosition.xyz, g_flashLights[l], roughness, metalness, albedo.rgb);
    
        finalColor += lightResult * shadowFactor;
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------------------
    
    
    //IBL
    //------------------------------------------------------------------------------------------------------------
    
    if (useIBL)
    {
        if (useDiffuseReflection)
        {
            float3 iblDiffuse = CalculateIBLDiffuse(worldNormal, albedo.rgb, metalness);
            finalColor += albedo.rgb * iblDiffuse;
        }

        if (useRoughnessOverwriting)
        {
            roughness = overwrittenRoughnessValue;
        }

        if (useSpecularReflection)
        {
            float F0 = lerp(0.04, albedo, metalness);
            float3 iblSpecular = CalculateIBLSpecular(worldNormal, normalize(input.WorldPosition.xyz - g_cameraPosition.xyz), roughness, float3(F0, F0, F0));
            finalColor += iblSpecular;
        }
    }

    
    //------------------------------------------------------------------------------------------------------------
    
    
    return float4(finalColor, 1);
}
