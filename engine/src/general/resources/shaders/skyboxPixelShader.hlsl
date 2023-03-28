#include "globals.hlsl"
TextureCube SkyboxTexture : register(t0);


struct Input
{
    float4 position : SV_Position;
    float3 direction : DIRECTION0;
    float3 texcoord : TEXCOORD0;
    float3 campos : CAMPOS;
    
};

cbuffer InvView : register(b1)
{
    float4x4 invView;
    float4x4 invProj;
}

float4 main(Input input) : SV_TARGET
{
    
    // Convert screen position to NDC coordinates
    //float3 ndc = normalize(input.direction * 2 - 1);

    // Calculate the screen-space depth (z-coordinate) at the current pixel
    //float depth = ...; // Read depth from your depth buffer (e.g., using a Texture2D)

    //// Calculate the clip-space position of the current pixel
    //float4 clipPosition = float4(input.screenPosition, depth, 1.0f);

    
    
    //// Inverse projection matrix
    //float4x4 invProjectionMatrix = ...;

    //// Calculate the view-space position
    //float4 viewPosition = mul(clipPosition, invProjectionMatrix);

    // Get the view direction by normalizing the view-space position (assuming the camera is at the origin)
    //float3 viewDirection = normalize(input.direction.xyz);
    
    //float4 color = SkyboxTexture.Sample(g_anisotropicWrap, ndc);
    
    
    // Compute clip space position
    float4 clipPos = float4(input.texcoord.xy, 1.0, 1.0);

    // Transform clip space position to world space
    float4 worldPos = mul(mul(clipPos, invProj), invView);
    worldPos /= worldPos.w;

    // Calculate the direction vector from the camera position
    
    float3 direction = normalize(worldPos.xyz - input.campos);

    
    
    // Sample the cubemap using the direction vector
    
    //float4 color = SkyboxTexture.Sample(g_pointWrap, direction);
    float4 color = SkyboxTexture.Sample(g_pointWrap, input.texcoord);
    return color;
}
