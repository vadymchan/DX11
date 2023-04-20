#include "../globals.hlsl"

Texture2D modelTexture : register(t0);


struct Input
{
    float4 Position : SV_POSITION;
    float4 ViewPosition : ViewPosition;
    float4 WorldPosition : WorldPosition;
    float3 Normal : NORMAL;
    float3 WorldNormal : WORLDNORMAL;
    float2 UV : TEXCOORD;
};



float4 main(Input input) : SV_TARGET
{
    float4 texColor;

    switch (g_samplerStateIndex)
    {
        case 0:
            texColor = modelTexture.Sample(g_pointWrap, input.UV);
            break;
        case 1:
            texColor = modelTexture.Sample(g_linearWrap, input.UV);
            break;
        case 2:
            texColor = modelTexture.Sample(g_anisotropicWrap, input.UV);
            break;
        default:
            texColor = modelTexture.Sample(g_anisotropicWrap, input.UV);
            break;
    }

    float3 normal = normalize(input.Normal);
    float3 position = input.ViewPosition.xyz;
    float3 finalColor = texColor.rgb * float3(0.2, 0.2, 0.2);
    
   
    
    
    

    
    //world space 
    
    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float3 lightResult = CalculateDirectionalLight(normal, g_directionalLights[i], position /*- g_cameraPosition.xyz*//*, g_cameraPosition.xyz*/);
        finalColor += texColor.rgb * lightResult;
    }

    for (int j = 0; j < g_numPointLights; ++j)
    {
        float3 lightResult = CalculatePointLight(normal, position, g_pointLights[j]/*, g_cameraPosition.xyz*/);
        finalColor += texColor.rgb * lightResult;
    }
    
    for (int m = 0; m < g_numSpotLights; ++m)
    {
        float3 lightResult = CalculateSpotLight(normal, position, g_spotLights[m]/*, g_cameraPosition.xyz*/);
        finalColor += texColor.rgb * lightResult;
    }

    for (int k = 0; k < g_numFlashLights; ++k)
    {
        //float3 lightResult = CalculateSpotLight(normal, position, g_flashLights[k]);
        //float flashlightMask = computeFlashlightMask(input.UV, g_spotLights[k].direction, normal);
        float3 lightResult = CalculateFlashLight(input.WorldNormal, input.WorldPosition.xyz, g_flashLights[k]/*, g_cameraPosition.xyz*/);
        finalColor += texColor.rgb * lightResult;
    }



    return float4(finalColor, texColor.a);
    
    
    //float3 normal = float3(0, 0, 1);
    //float3 worldSpacePosition = float3(0, 0, 0);
    //FlashLight g_flashLight;
    //g_flashLight.position = float3(0, 0, -10);
    //g_flashLight.direction = float3(0, 0, 1);
    //g_flashLight.color = float3(1, 1, 1);
    //g_flashLight.intensity = 1;
    //g_flashLight.innerAngle = 0.9;
    //g_flashLight.outerAngle = 0.7;
    //float3 cameraPosition = float3(0, 0, -5);
    
    //float3 lightResult = CalculateFlashLight(normal, position, g_flashLights[k], g_cameraPosition.xyz);
    
    
    //float3 finalColor = texColor.rgb * g_AmbientColor;

    //for (int i = 0; i < g_numDirectionalLights; ++i)
    //{
    //    float3 lightResult = CalculateDirectionalLight(normal, g_directionalLights[i], position /*- g_cameraPosition.xyz*/);
    //    finalColor += texColor.rgb * lightResult;
    //}

    //for (int j = 0; j < g_numPointLights; ++j)
    //{
    //    float3 lightResult = CalculatePointLight(normal, position, g_pointLights[j]);
    //    finalColor += texColor.rgb * lightResult;
    //}
    
    //for (int m = 0; m < g_numSpotLights; ++m)
    //{
    //    float3 lightResult = CalculateSpotLight(normal, position, g_spotLights[m]);
    //    finalColor += texColor.rgb * lightResult;
    //}

    //for (int k = 0; k < g_numFlashLights; ++k)
    //{
    //    //float3 lightResult = CalculateSpotLight(normal, position, g_flashLights[k]);
    //    //float flashlightMask = computeFlashlightMask(input.UV, g_spotLights[k].direction, normal);
    //    float3 lightResult = CalculateFlashLight(normal, position, g_flashLights[k]);
    //    finalColor += texColor.rgb * lightResult;
    //}
}