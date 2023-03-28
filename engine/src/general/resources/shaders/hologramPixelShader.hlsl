#include "hologramShader.hlsli"

cbuffer PerView : register(b0)
{
    float4 cameraPos;
}


struct Input
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
};


// called in pixel shader
float3 main(Input input) : SV_TARGET
{
    float blueWave = wave(input.position.xyz, BLUE_WAVE_INTERVAL, BLUE_WAVE_SPEED, BLUE_WAVE_THICKNESS, true);
    float redWave = wave(input.position.xyz, RED_WAVE_INTERVAL, RED_WAVE_SPEED, RED_WAVE_THICKNESS, false);

    float3 toCamera = normalize(cameraPos.xyz- input.position.xyz);
    float contourGlow = pow(1.0 - abs(dot(input.normal, toCamera)), 2);

    // when contourInterference is 0, ripple effect contourWave is added to contourGlow, otherwise contourWave is 1
    float contourWave = wave(input.position.xyz, 0.1, 0.1, 0.05, false);
    float contourInterference = periodIntensity(time.x, 4, 1);
    contourWave = lerp(contourWave, 1.0, contourInterference);
    // when contourWave is 0.0, contourGlow becomes darker, otherwise contourGlow color is plain, without ripple
    contourGlow = lerp(contourGlow / 10, contourGlow, contourWave);

    float3 color = float3(0, 0.3, 0.3) * min(1, contourGlow + blueWave * 0.5);
    float colorNoise = sqrt(noise4d(float4(input.position.xyz, frac(time.x)) * 100, 1));
    color *= lerp(colorNoise, 1.0, contourInterference);
    
    color = lerp(color, float3(1.0, 0.0, 0.0), redWave * 0.25);
    return color;


}