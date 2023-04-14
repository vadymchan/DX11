#include "../globals.hlsl"

Texture2D modelTexture : register(t0);


cbuffer PerFrame : register(b0)
{
	float3 g_CameraPosition;
};

cbuffer MaterialProperties : register(b1)
{
    float3 g_AmbientColor;
    //float3 g_DiffuseColor;
    //float3 g_SpecularColor;
    //float g_SpecularPower;
};

//cbuffer LightProperties : register(b2)
//{
//	float4 g_LightPosition; // xyz: light position, w: 1.0 for point light, 0.0 for directional light
//	float3 g_LightColor;
//	float g_LightIntensity;
//};

struct Input
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
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
    float3 position = input.Position.xyz;
    float3 finalColor = texColor.rgb * g_AmbientColor;

    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float3 lightResult = CalculateDirectionalLight(normal, g_directionalLights[i], position);
        finalColor += texColor.rgb * lightResult;
    }

    for (int i = 0; i < g_numPointLights; ++i)
    {
        float3 lightResult = CalculatePointLight(normal, position, g_pointLights[i]);
        finalColor += texColor.rgb * lightResult;
    }

    for (int i = 0; i < g_numSpotLights; ++i)
    {
        float3 lightResult = CalculateSpotLight(normal, position, g_spotLights[i]);
        finalColor += texColor.rgb * lightResult;
    }

    return float4(finalColor, texColor.a);
}