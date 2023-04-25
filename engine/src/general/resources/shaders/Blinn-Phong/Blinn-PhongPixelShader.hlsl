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
    float3 finalColor = texColor.rgb * float3(0.1, 0.1, 0.1);
    
    
    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float3 lightResult = CalculateDirectionalLight(normal, g_directionalLights[i], position );
        finalColor += texColor.rgb * lightResult;
    }

    for (int j = 0; j < g_numPointLights; ++j)
    {
        float3 lightResult = CalculatePointLight(normal, position, g_pointLights[j]);
        finalColor += texColor.rgb * lightResult;
    }
    
    for (int m = 0; m < g_numSpotLights; ++m)
    {
        float3 lightResult = CalculateSpotLight(normal, position, g_spotLights[m]);
        finalColor += texColor.rgb * lightResult;
    }

    for (int k = 0; k < g_numFlashLights; ++k)
    {
        float3 lightResult = CalculateFlashLight(input.WorldNormal, input.WorldPosition.xyz, g_flashLights[k]);
        finalColor += texColor.rgb * lightResult;
    }



    return float4(finalColor, texColor.a);
    
}