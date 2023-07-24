#include "../globals.hlsli"

Texture2D albedoTexture    : register(t0);
Texture2D roughnessTexture : register(t1);
Texture2D metalnessTexture : register(t2);
Texture2D normalTexture    : register(t3);

struct Input
{
    float4 Position : SV_POSITION;
    float4 ViewPosition : ViewPosition;
    float4 WorldPosition : WorldPosition;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float4x4 View : VIEW;
    
};

struct Output
{
    float3 Albedo : SV_Target0;
    float4 RoughnessMetallness : SV_Target1;
    float4 Normals : SV_Target2;
    float4 Emission : SV_Target3;
    uint InstanceID : SV_Target4;
};

Output main(Input input) : SV_TARGET
{
    Output output;
    float4 albedo;
    float roughness;
    float metalness;


    albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
    roughness = roughnessTexture.Sample(g_anisotropicWrap, input.UV).r;
    metalness = metalnessTexture.Sample(g_anisotropicWrap, input.UV).r;

    return output;
}
