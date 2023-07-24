#include "../globals.hlsli"

Texture2D smoke_RLU : register(t0); // Right, Left, Up multipliers
Texture2D smoke_DBF : register(t1); // Down, Back, Front multipliers
Texture2D smoke_MVEA : register(t2); // Emission, Motion Vectors, Alpha
Texture2D depthBufferCopy : register(t3); // Depth buffer

SamplerState sample;

struct Input
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD; // texture sample uv for the current frame
    float2 uv2 : TEXCOORD1; // texture sample uv for the next frame
    float4x4 viewMat : VIEW;
    float4 viewPosition : VIEW_POSITION;
    float4 particleCenter : PARTICLE_CENTER;
    float frameFraction : FRAME_FRACTION;
    float nearPlane : NEAR_PLANE; // near plane (bigger value because of reversed depth)
    float farPlane : FAR_PLANE; // far plane (smaller value because of reversed depth)
};


float LinearizeDepth(float zPosition, float nearPlane, float farPlane)
{
    return nearPlane * farPlane / (farPlane - zPosition * (farPlane - nearPlane));
}


float NormalizeDepth(float linearizedDepth, float nearPlane, float farPlane)
{
    return (linearizedDepth - nearPlane) / (farPlane - nearPlane);
}


float4 main(Input input) : SV_TARGET
{
    // Sample the lightmap textures
    
    

    float4 EMVA0_initial = smoke_MVEA.Sample(g_linearWrap, input.uv);
    float4 EMVA1_initial = smoke_MVEA.Sample(g_linearWrap, input.uv2);
    float2 motionVector0 = EMVA0_initial.gb;
    float2 motionVector1 = EMVA1_initial.gb;

    // [0;1] to [-1;1] range
    motionVector0 = motionVector0 * 2.0f - 1.0f;
    motionVector1 = motionVector1 * 2.0f - 1.0f;

    //BEGIN: Motion vectors
    static const float MV_SCALE = 0.0015; // adjusted for the smoke textures
    float time = input.frameFraction; // goes from 0.0 to 1.0 between two sequential frames

    float2 uv0 = input.uv; // texture sample uv for the current frame
    uv0 -= motionVector0 * MV_SCALE * time; // if MV points in some direction, then UV flows in opposite

    float2 uv1 = input.uv2; // texture sample uv for the next frame
    uv1 -= motionVector1 * MV_SCALE * (time - 1.f); // if MV points in some direction, then UV flows in opposite

    // Here we use updated motion vectors for sampling
    float4 EMVA0_final = smoke_MVEA.Sample(g_linearWrap, uv0);
    float4 EMVA1_final = smoke_MVEA.Sample(g_linearWrap, uv1);

    float2 emissionAlpha = lerp(EMVA0_final.ra, EMVA1_final.ra, time);
    float emission = emissionAlpha.x;
    float smokeAlpha = emissionAlpha.y;
    //END: Motion vectors
    
    //float4 EMVA = smoke_MVEA.Sample(g_linearWrap, input.uv);
    //float2 motionVector = EMVA.gb;
    
    ////BEGIN: Motion vectors
    //static const float MV_SCALE = 0.0015; // adjusted for the smoke textures
    //float time = input.frameFraction; // goes from 0.0 to 1.0 between two sequential frames

    //float2 uv0 = input.uv; // texture sample uv for the current frame
    //uv0 -= motionVector * MV_SCALE * time; // if MV points in some direction, then UV flows in opposite

    //float2 uv1 = input.uv2; // texture sample uv for the next frame
    //uv1 -= motionVector * MV_SCALE * (time - 1.f); // if MV points in some direction, then UV flows in opposite

    //float4 EMVA0 = smoke_MVEA.Sample(g_linearWrap, uv0);
    //float4 EMVA1 = smoke_MVEA.Sample(g_linearWrap, uv1);

    //float2 emissionAlpha = lerp(EMVA0.ra, EMVA1.ra, time);
    //float emission = emissionAlpha.x;
    //float smokeAlpha = emissionAlpha.y;
    ////END: Motion vectors
    
    //BEGIN: Soft particles


    float threshold = 0.000001;
    
    
    float viewportWidth = 982; // Set this to your viewport width
    float viewportHeight = 953; // Set this to your viewport height

// Convert SV_Position to NDC coordinates
    float2 ndcCoords;
    ndcCoords.x = (2.0 * input.position.x - viewportWidth) / viewportWidth;
    ndcCoords.y = (viewportHeight - 2.0 * input.position.y) / viewportHeight;

// Convert NDC coordinates to texture coordinates
    float2 texCoords;
    texCoords.x = (ndcCoords.x + 1.0) / 2.0;
    texCoords.y = 1.0 - (ndcCoords.y + 1.0) / 2.0;

// Sample the depth using the computed texture coordinates
    float sceneZ = depthBufferCopy.Sample(g_anisotropicWrap, texCoords).r;
    
    //float sceneZ = depthBufferCopy.Sample(g_anisotropicWrap, input.uv).r;


    float sceneDepthLinearized = LinearizeDepth(sceneZ, input.nearPlane, input.farPlane);
    float sceneDepthNormalized = NormalizeDepth(sceneDepthLinearized, input.nearPlane, input.farPlane);

    float particleDepthLinearized = LinearizeDepth(input.position.z, input.nearPlane, input.farPlane);
    float particleDepthNormalized = NormalizeDepth(particleDepthLinearized, input.nearPlane, input.farPlane);

    
    float deltaDepth = particleDepthNormalized - sceneDepthNormalized;
    //deltaDepth = input.position.z - sceneZ;
    
    float fade = clamp(deltaDepth / threshold, 0, 1);

   // return float4(fade, fade, fade, 1);
    
    //smokeAlpha = saturate(smokeAlpha - fade);
    
    
    float4 result = float4(0, 0, 0, 0);
    result.rgb = input.color.rgb * smokeAlpha;
    result.a = smokeAlpha < input.color.a ? smokeAlpha : input.color.a;

    //END: Soft particles
    
    
    //BEGIN: 6-way lightmap
    
    float3 RLU0 = smoke_RLU.Sample(g_linearWrap, uv0).rgb;
    float3 DBF0 = smoke_DBF.Sample(g_linearWrap, uv0).rgb;
    
    float3 RLU1 = smoke_RLU.Sample(g_linearWrap, uv1).rgb;
    float3 DBF1 = smoke_DBF.Sample(g_linearWrap, uv1).rgb;
    
    float3 RLU = lerp(RLU0, RLU1, time);
    float3 DBF = lerp(DBF0, DBF1, time);
    
    // The basis vectors in view space
    
    

    float3 particleFront = normalize(-input.particleCenter.xyz);
    float3 up = float3(0, 1, 0);
    float3 particleRight = cross(up, particleFront);
    float3 particleUp = cross(particleFront, particleRight);
    float3 basis[6] =
    {
        particleRight, // Right
        -particleRight, // Left
        particleUp, // Up
        -particleUp, // Down
        -particleFront, // Back
        particleFront // Front
    };
    
    // The factors from the lightmap textures
    float factors[6] =
    {
        RLU.r, // Right
        RLU.g, // Left
        RLU.b, // Up
        DBF.r, // Down
        DBF.g, // Back
        DBF.b  // Front
    };

    // Calculate the lighting
    float3 lighting = float3(0, 0, 0);
    
    const static int DIRECTION_NUM = 6;
    
    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        const float3 lightDir = g_directionalLights[i].direction;
        for (int direction = 0; direction < DIRECTION_NUM; ++direction)
        {
            float dotProd = max(0, dot(lightDir, basis[direction]));
            lighting += dotProd * factors[direction] * g_directionalLights[i].color;
        }
    }
    
    for (int j = 0; j < g_numSpotLights; ++j)
    {
        const float3 viewToLightVector = normalize(input.viewPosition.xyz - g_spotLights[j].position);
        const float3 spotLightDirection = normalize(g_spotLights[j].direction);
        for (int direction = 0; direction < DIRECTION_NUM; ++direction)
        {
            float directionalDotProduct = max(0, dot(spotLightDirection, -basis[direction]));
            float cosAngleBetweenVectors = dot(spotLightDirection, viewToLightVector);
            float falloff = smoothstep(cos(g_spotLights[j].outerAngle), cos(g_spotLights[j].innerAngle), cosAngleBetweenVectors);
            lighting += falloff * directionalDotProduct * factors[direction] * g_spotLights[j].color;
        }
    }

    
    for (int k = 0; k < g_numPointLights; ++k)
    {
        const float3 pointLightViewPos = mul(float4(g_pointLights[k].position, 1), input.viewMat).xyz;
        const float3 lightDir = normalize(pointLightViewPos - input.viewPosition.xyz);
        for (int direction = 0; direction < DIRECTION_NUM; ++direction)
        {
            float dotProd = max(0, dot(lightDir, basis[direction]));
            lighting += dotProd * factors[direction] * g_pointLights[k].color;
        }
    }
    

    for (int m = 0; m < g_numFlashLights; ++m)
    {
        const float3 viewToLightVector = normalize(input.viewPosition.xyz - g_spotLights[j].position);
        const float3 flashLightDirection = normalize(mul(g_flashLights[m].direction, (float3x3) input.viewMat));
        for (int direction = 0; direction < DIRECTION_NUM; ++direction)
        {
            float directionalDotProduct = max(0, dot(flashLightDirection, -basis[direction]));
            float cosAngleBetweenVectors = dot(flashLightDirection, viewToLightVector);
            float falloff = smoothstep(cos(g_flashLights[m].outerAngle), cos(g_flashLights[m].innerAngle), cosAngleBetweenVectors);
            lighting += g_flashLights[m].intensity * falloff * directionalDotProduct * factors[direction] * g_flashLights[m].color;
        }
    }
    

    lighting += emission;

    result.rgb += lighting;
    
    //END: 6-way lightmap
    
    
    return result;

}

