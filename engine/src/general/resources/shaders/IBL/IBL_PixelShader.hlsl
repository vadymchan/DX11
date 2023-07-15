#include "../globals.hlsli"
#include "IBL_common.hlsli"



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


    
    albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
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
    float3 viewMacroNormal = normalize(mul(input.Normal, (float3x3) input.View));

    // PBR
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------
    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float3 lightResult = CalculateDirectionalLightPBR(viewNormal, viewMacroNormal, g_directionalLights[i], input.ViewPosition.xyz, roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int j = 0; j < g_numPointLights; ++j)
    {
        float3 lightResult = CalculatePointLightPBR(viewNormal, viewMacroNormal, input.ViewPosition.xyz, g_pointLights[j], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }
    
    for (int k = 0; k < g_numSpotLights; ++k)
    {
        float3 lightResult = CalculateSpotLightPBR(viewNormal, viewMacroNormal,  input.ViewPosition.xyz, g_spotLights[k], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int l = 0; l < g_numFlashLights; ++l)
    {
        float3 lightResult = CalculateFlashLightPBR(worldNormal, input.Normal, input.WorldPosition.xyz, g_flashLights[l], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
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
