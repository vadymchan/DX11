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

float4 main(Input input) : SV_TARGET
{
    // Sample the lightmap textures
    
    float4 EMVA = smoke_MVEA.Sample(g_linearWrap, input.uv);
    float2 motionVector = EMVA.gb;

   
    //BEGIN: Motion vectors
    static const float MV_SCALE = 0.0015; // adjusted for the smoke textures
    float time = input.frameFraction; // goes from 0.0 to 1.0 between two sequential frames

    float2 uv0 = input.uv; // texture sample uv for the current frame
    uv0 -= motionVector * MV_SCALE * time; // if MV points in some direction, then UV flows in opposite

    float2 uv1 = input.uv2; // texture sample uv for the next frame
    uv1 -= motionVector * MV_SCALE * (time - 1.f); // if MV points in some direction, then UV flows in opposite

    float4 EMVA0 = smoke_MVEA.Sample(g_linearWrap, uv0);
    float4 EMVA1 = smoke_MVEA.Sample(g_linearWrap, uv1);

    float2 emissionAlpha = lerp(EMVA0.ra, EMVA1.ra, time);
    float emission = emissionAlpha.x;
    float smokeAlpha = emissionAlpha.y;
    //END: Motion vectors
    
    //BEGIN: Soft particles


    float threshold = 0.00001;
    float sceneZ = depthBufferCopy.Sample(sample, input.uv).r;

    float sceneDepthLinearized = input.nearPlane * input.farPlane / (input.farPlane - sceneZ * (input.farPlane - input.nearPlane));
    float sceneDepthNormalized = (sceneDepthLinearized - input.nearPlane) / (input.farPlane - input.nearPlane);

    float partZ = input.position.z;
    
    float particleDepthLinearized = input.nearPlane * input.farPlane / (input.farPlane - partZ * (input.farPlane - input.nearPlane));
    float particleDepthNormalized = (particleDepthLinearized - input.nearPlane) / (input.farPlane - input.nearPlane);

    float deltaDepth = particleDepthNormalized - sceneDepthNormalized;
    
    float fade = clamp(deltaDepth / threshold, 0, 1);

    smokeAlpha = saturate(smokeAlpha - fade);
    
    
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

