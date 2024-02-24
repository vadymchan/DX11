#include "../globals.hlsli"
TextureCube SkyboxTexture : register(t0);


struct Input
{
    float4 position : SV_Position;
    float3 texcoord : TEXCOORD0;
    
};


float4 main(Input input) : SV_TARGET
{
    
    float4 color;
    
    switch (g_samplerStateIndex)
    {
        case 0:
            color.rgb = SkyboxTexture.Sample(g_pointWrap, input.texcoord).rgb;
            break;
        case 1:
            color.rgb = SkyboxTexture.Sample(g_linearWrap, input.texcoord).rgb;
            break;
        case 2:
            color.rgb = SkyboxTexture.Sample(g_anisotropicWrap, input.texcoord).rgb;
            break;
        default:
            color.rgb = SkyboxTexture.Sample(g_anisotropicWrap, input.texcoord).rgb;
            break;
    }
    
    
    color.a = 1.0f;
    
    return color;
}
