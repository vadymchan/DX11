#include "../globals.hlsl"
Texture2D hdrTexture : register(t0);

cbuffer PostProcess : register(b0)
{
    float EV100;
    float Gamma;
    float2 paddingPostProcess;
};



float3 acesHdr2Ldr(float3 hdr)
{
    
    //float a = 2.51f;
    //float b = 0.03f;
    //float c = 2.43f;
    //float d = 0.59f;
    //float e = 0.14f;
    //return saturate((hdr * (a * hdr + b)) / (hdr * (c * hdr + d) + e));
    
    //--------------------------------
    
    //return hdr / (hdr + 1.0f);
    
    //--------------------------------
    
    float3x3 m1 = float3x3(
		float3(0.59719f, 0.07600f, 0.02840f),
		float3(0.35458f, 0.90834f, 0.13383f),
		float3(0.04823f, 0.01566f, 0.83777f)
		);
    float3x3 m2 = float3x3(
		float3(1.60475f, -0.10208, -0.00327f),
		float3(-0.53108f, 1.10813, -0.07276f),
		float3(-0.07367f, -0.00605, 1.07602f)
		);

    float3 v = mul(hdr, m1);
    float3 a = v * (v + float3(0.0245786f, 0.0245786f, 0.0245786f)) - float3(0.000090537f, 0.000090537f, 0.000090537f);
    float3 b = v * (v * float3(0.983729f, 0.983729f, 0.983729f) + float3(0.4329510f, 0.4329510f, 0.4329510f)) + float3(0.238081f, 0.238081f, 0.238081f);
    float3 ldr = saturate(mul(a / b, m2));

    return ldr;
}

float3 adjustExposure(float3 color, float EV100)
{
    
    float LMax = (78.0f / (0.65f * 100.0f)) * pow(2.0f, EV100);
    return color * (1.0f / LMax);
}

float3 correctGamma(float3 color, float gamma)
{
    return pow(color, 1.0f / gamma);
}


float3 correctGamma1(float3 color, float gamma)
{
    return pow(color, gamma);
}



struct Input
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float3 main(Input input) : SV_TARGET
{
    
    float3 hdrColor;
    
    switch (g_samplerStateIndex)
    {
        case 0:
            hdrColor = hdrTexture.Sample(g_pointWrap, input.uv).rgb;
            break;
        case 1:
            hdrColor = hdrTexture.Sample(g_linearWrap, input.uv).rgb;
            break;
        case 2:
            hdrColor = hdrTexture.Sample(g_anisotropicWrap, input.uv).rgb;
            break;
        default:
            hdrColor = hdrTexture.Sample(g_anisotropicWrap, input.uv).rgb;
            break;
    }
    
    // Sample the HDR texture
    //float3 hdrColor = hdrTexture.Sample(g_pointWrap, input.uv).rgb;

    // Apply exposure adjustment
    float3 adjustedColor = adjustExposure(hdrColor, EV100);

    // Apply ACES tone mapping
    float3 toneMappedColor = acesHdr2Ldr(adjustedColor);

    // Apply gamma correction
    float3 finalColor = correctGamma(toneMappedColor, Gamma);
    //finalColor = correctGamma(finalColor, Gamma);

    return finalColor;
    //return float4(finalColor, 1.0);
    //return float4(toneMappedColor, 1.0);
    //return float4(hdrColor, 1.0);
}
