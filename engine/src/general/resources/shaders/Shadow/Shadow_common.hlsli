#include "../globals.hlsli"

Texture2DArray DirectionalShadowMap : register(t8);
Texture2DArray SpotShadowMap : register(t9);
TextureCubeArray PointShadowMap : register(t10);
Texture2DArray FlashShadowMap : register(t11);



//BEGIN: Shadow calculation
//-------------------------------------------------------------------------------------------------------

// Shadow factor calculation for directional light
float CalculateDirectionalShadowFactor(float3 worldPosition, float3 normal, DirectionalLight light, int index)
{
    float4 shadowPos = mul(float4(worldPosition, 1.0), light.shadowViewMatrix);
    shadowPos = mul(shadowPos, light.shadowProjMatrix);
    shadowPos.xyz /= shadowPos.w;
    shadowPos.y = -shadowPos.y; // Flip the Y-coordinate (for correct texture mapping)
    shadowPos.xy = shadowPos.xy * 0.5 + 0.5;

    //can't use since the error occurs (No overload for method 'SampleCmpLevelZero' takes 3 arguments.)
    //float shadowFactor = DirectionalShadowMap.SampleCmpLevelZero(g_linearWrap, float3(shadowPos.xy, index), shadowPos.z).r;

    float depth = DirectionalShadowMap.Sample(g_linearWrap, float3(shadowPos.xy, index)).r;
    float shadowFactor = (shadowPos.z < depth) ? 0.0 : 1.0;

    return shadowFactor;
}


// Shadow factor calculation for directional light with PCF
float CalculateDirectionalShadowFactorPCF(float3 worldPosition, float3 normal, DirectionalLight light, int index)
{
    float4 shadowPos = mul(float4(worldPosition, 1.0), light.shadowViewMatrix);
    shadowPos = mul(shadowPos, light.shadowProjMatrix);
    shadowPos.xyz /= shadowPos.w;
    shadowPos.y = -shadowPos.y; // Flip the Y-coordinate (for correct texture mapping)
    shadowPos.xy = shadowPos.xy * 0.5 + 0.5;

    //offsets for PCF
    float2 offsets[4] =
    {
        { -0.5, 0 },
        { 0.5, 0 },
        { 0, -0.5 },
        { 0, 0.5 }
    };

    // Get the resolution of the shadow map
    int shadowMapWidth, shadowMapHeight, shadowMapArraySize;
    DirectionalShadowMap.GetDimensions(shadowMapWidth, shadowMapHeight, shadowMapArraySize);
    float2 shadowMapSize = float2(shadowMapWidth, shadowMapHeight);

    // Sample multiple times around the original position
    float shadowFactor = 0.0;
    for (int i = 0; i < 4; ++i)
    {
        float4 offsetShadowPos = shadowPos;
        offsetShadowPos.xy += offsets[i] / shadowMapSize;

        float depth = DirectionalShadowMap.Sample(g_linearWrap, float3(offsetShadowPos.xy, index)).r;
        shadowFactor += (offsetShadowPos.z < depth) ? 0.0 : 1.0;
    }

    shadowFactor /= 4.0;

    shadowFactor = smoothstep(0.33, 1.00, shadowFactor);

    return shadowFactor;
}



// Shadow factor calculation for spotlight
float CalculateSpotLightShadowFactor(float3 worldPosition, float3 normal, SpotLight light, int index)
{
    float4 shadowPos = mul(float4(worldPosition, 1.0), light.shadowViewMatrix);
    shadowPos = mul(shadowPos, light.shadowProjMatrix);
    shadowPos.xyz /= shadowPos.w;
    shadowPos.y = -shadowPos.y; // Flip the Y-coordinate (for correct texture mapping)
    shadowPos.xy = shadowPos.xy * 0.5 + 0.5;

    float depth = SpotShadowMap.Sample(g_linearWrap, float3(shadowPos.xy, index)).r;
    float shadowFactor = (shadowPos.z < depth) ? 0.0 : 1.0;

    return shadowFactor;
}




uint selectCubeFace(float3 unitDir)
{
    float maxVal = max(abs(unitDir.x), max(abs(unitDir.y), abs(unitDir.z)));
    uint maxIndex = abs(unitDir.x) == maxVal ? 0 : (abs(unitDir.y) == maxVal ? 2 : 4);
    return maxIndex + (asuint(unitDir[maxIndex / 2]) >> 31); 
    // same as:
    // return maxIndex + (unitDir[maxIndex / 2] < 0.f ? 1u : 0u);
}


// Shadow factor calculation for point light
float CalculatePointLightShadowFactor(float3 worldPosition, float3 normal, PointLight light, int index)
{
    float3 dirToLight = worldPosition - light.position;
    
    int face = selectCubeFace(dirToLight);

    float4 shadowPos = mul(float4(worldPosition, 1.0), light.shadowViewMatrices[face]);
    shadowPos = mul(shadowPos, light.shadowProjMatrix);
    shadowPos.xyz /= shadowPos.w;
    float fragmentDepth = shadowPos.z;

    float depth = PointShadowMap.Sample(g_linearWrap, float4(dirToLight, index)).r;
    float shadowFactor = (fragmentDepth < depth) ? 0.0 : 1.0;

    return shadowFactor;
}


// Shadow factor calculation for flashlight
float CalculateFlashLightShadowFactor(float3 worldPosition, float3 normal, FlashLight light, int index)
{
    float4 shadowPos = mul(float4(worldPosition, 1.0), light.shadowViewMatrix);
    shadowPos = mul(shadowPos, light.shadowProjMatrix);
    shadowPos.xyz /= shadowPos.w;
    float fragmentDepth = shadowPos.z;
    shadowPos.y = -shadowPos.y; // Flip the Y-coordinate (for correct texture mapping)
    shadowPos.xy = shadowPos.xy * 0.5 + 0.5;

    float depth = FlashShadowMap.Sample(g_linearWrap, float3(shadowPos.xy, index)).r;
    float shadowFactor = (fragmentDepth < depth ) ? 0.0 : 1.0;

    return shadowFactor;
}


//-------------------------------------------------------------------------------------------------------
//END: Shadow calculation

