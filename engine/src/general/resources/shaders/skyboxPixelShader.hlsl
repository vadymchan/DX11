#include "globals.hlsl"
TextureCube SkyboxTexture : register(t0);


struct Input
{
    float4 position : SV_Position;
    float3 texcoord : TEXCOORD0;
    
};


float4 main(Input input) : SV_TARGET
{
    
    float4 color = SkyboxTexture.Sample(g_linearWrap, input.texcoord);
    return color;
}
