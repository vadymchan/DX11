
#define pi 3.14159265359
#define TRIANGLE
//#define SHADERTOY


Texture2D modelTexture : register(t0);
SamplerState modelSampler : register(s0); // later move it to global.hlsl


cbuffer ConstBuffer : register(b1)
{
    float2 resolution; 
    float2 time;
};

cbuffer Material : register(b0)
{
    float4 color;
};

float saw(float4 x)
{
    return abs(frac(x) - 0.5) * 2.0;
}

float dw(float2 p, float2 c, float t)
{
    return sin(length(p - c) - t);
}

float dw1(float2 uv)
{
    float v = 0.0;
    float t = time * 2.0;
    v += dw(uv, float2(sin(t * 0.07) * 30.0, cos(t * 0.04) * 20.0), t * 1.3);
    v += dw(uv, float2(cos(t * 0.13) * 30.0, sin(t * 0.14) * 20.0), t * 1.6 + 1.0);
    v += dw(uv, float2(18, -15), t * 0.7 + 2.0);
    v += dw(uv, float2(-18, 15), t * 1.1 - 1.0);
    return v / 4.0;
}

float fun(float x, float y)
{
    return dw1(float2(x - 0.5, y - 0.5) * 80.0);
}

float3 duv(float2 uv)
{
    float x = uv.x;
    float y = uv.y;
    float v = fun(x, y);
    float d = 1.0 / 400.0;
    float dx = (v - fun(x + d, y)) / d;
    float dy = (v - fun(x, y + d)) / d;
    float a = atan2(dx, dy) / pi / 2.0;
    return float3(v, 0, (v * 4.0 + a));
}

struct Input
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET
{
    
#ifdef SHADERTOY
    float2 uv = position.xy / resolution.x;
    float3 h = duv(uv);
    float sp = saw(h.z + time.x * 1.3);
    sp = clamp((sp - 0.25) * 2.0, 0.5, 1.0);
    return float4((h.x + 0.5) * sp, (0.3 + saw(h.x + 0.5) * 0.6) * sp, (0.6 - h.x) * sp, 1.0);
#endif

#ifdef TRIANGLE
    
    float4 color = modelTexture.Sample(modelSampler, input.texCoord);
    
    return color;
#endif

    
}