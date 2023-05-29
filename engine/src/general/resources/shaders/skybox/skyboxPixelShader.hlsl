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
            color = SkyboxTexture.Sample(g_pointWrap, input.texcoord);
            break;
        case 1:
            color = SkyboxTexture.Sample(g_linearWrap, input.texcoord);
            break;
        case 2:
            color = SkyboxTexture.Sample(g_anisotropicWrap, input.texcoord);
            break;
        default:
            color = SkyboxTexture.Sample(g_anisotropicWrap, input.texcoord);
            break;
    }
    return color;
}
