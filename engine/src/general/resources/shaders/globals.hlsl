SamplerState g_pointWrap : register(s0);
SamplerState g_linearWrap : register(s1);
SamplerState g_anisotropicWrap :register(s2);

cbuffer SamplerStateIndexBuffer : register(b3)
{
    unsigned int g_samplerStateIndex;
    float3 padding;
};
