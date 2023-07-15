#include "../../globals.hlsli"


Texture2D noiseTexture : register(t12);

struct Input
{
    float4 position : SV_Position;
    uint RTIndex : SV_RenderTargetArrayIndex;
    float2 uv : TEXCOORD0;
    float time : TIME;
};

void main(Input input)
{
    
    float noise = noiseTexture.Sample(g_anisotropicWrap, input.uv).r;
    if (noise > input.time)
    {
        discard;
    }
}
