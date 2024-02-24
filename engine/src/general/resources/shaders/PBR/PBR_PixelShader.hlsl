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



//BEGIN: PBR light calculation
//-------------------------------------------------------------------------------------------------------

static const float PI = 3.14159265359;

float3 Fresnel(float NdotL, float3 F0)
{
    return F0 + (1 - F0) * pow(1 - NdotL, 5);
}

float Smith(float rough2, float NoV, float NoL)
{
    NoV *= NoV;
    NoL *= NoL;
    return 2.0 / (sqrt(1 + rough2 * (1 - NoV) / NoV) * sqrt(1 + rough2 * (1 - NoL) / NoL));
}

float GGX(float rough2, float NoH)
{
    float denom = NoH * NoH * (rough2 - 1.0) + 1.0;
    denom = PI * denom * denom;
    return rough2 / denom;
}
    
float3 CalculateLightPBR(
    float3 normal,
    float3 lightDirection,
    float3 viewDirection,
    float3 lightColor,
    float3 solidAngle,
    float intensity, float roughness,
    float metalness,
    float3 albedo)
{
    float3 halfVector = normalize(viewDirection + lightDirection);
    float NdotL = max(0.00f, dot(normal, lightDirection));
    float NdotV = max(0.00f, dot(normal, viewDirection));
    float NdotH = max(0.00f, dot(normal, halfVector));
    float LdotH = max(0.00f, dot(lightDirection, halfVector));

    
    float F0 = lerp(0.04, albedo, metalness);

    //diffuse component
    //-----------------
    float3 lambertian = (solidAngle * albedo * (1 - metalness)) / PI;
    lambertian *= (float3(1, 1, 1) - Fresnel(NdotL, float3(F0, F0, F0)));
    lambertian *= NdotL;
    //------------------
    
    
    //specular component
    //------------------
    float roughness2 = pow(roughness, 2);
    float D = GGX(roughness2, NdotH);
    float G = Smith(roughness2, NdotV, NdotL);
    float3 F = Fresnel(LdotH, float3(F0, F0, F0));
    float3 cook_torrance = (min(1, D * solidAngle / (4 * NdotL)) * G * F);
    //-----------------

    return lightColor * intensity * (lambertian + cook_torrance);
}



float3 CalculateDirectionalLightPBR(float3 normal, DirectionalLight g_directionalLight, float3 position, float roughness, float metalness, float3 albedo)
{
    float3 lightDirection = normalize(-g_directionalLight.direction);
    float3 viewDirection = normalize(-position);

    return CalculateLightPBR(normal, lightDirection, viewDirection, g_directionalLight.color, g_directionalLight.solidAngle, g_directionalLight.intensity, roughness, metalness, albedo);
}

float3 CalculatePointLightPBR(float3 normal, float3 position, PointLight g_pointLight, float roughness, float metalness, float3 albedo)
{
    float3 lightDirection = normalize(g_pointLight.position - position);
    float3 viewDirection = normalize(-position);

    float distance = length(g_pointLight.position - position);
    float solidAngle = 2 * PI * (1 - sqrt(1 - pow(g_pointLight.radius / distance, 2)));


    float3 lightResult = CalculateLightPBR(normal, lightDirection, viewDirection, g_pointLight.color, solidAngle, g_pointLight.intensity, roughness, metalness, albedo);
    return lightResult;
    
}


float3 CalculateSpotLightPBR(float3 normal, float3 position, SpotLight g_spotLight, float roughness, float metalness, float3 albedo)
{
    float3 lightDirection = normalize(g_spotLight.position - position);
    float3 viewDirection = normalize(-position);
    
    float outerConeCos = cos(g_spotLight.outerAngle);
    float innerConeCos = cos(g_spotLight.innerAngle);
    float angleCos = dot(-lightDirection, g_spotLight.direction);
    
    float distance = length(g_spotLight.position - position);
    float solidAngle = 2 * PI * (1 - sqrt(1 - pow(g_spotLight.radius / distance, 2)));
    
    float spotEffect = smoothstep(outerConeCos, innerConeCos, angleCos);

    float3 lightResult = CalculateLightPBR(normal, lightDirection, viewDirection, g_spotLight.color, solidAngle, g_spotLight.intensity, roughness, metalness, albedo);
    return lightResult * spotEffect;
}

float3 CalculateFlashLightPBR(float3 normal, float3 worldSpacePosition, FlashLight g_flashLight, float roughness, float metalness, float3 albedo)
{
    //trig functions 
    const float innerAngleCos = cos(g_flashLight.innerAngle);
    const float outerAngleCos = cos(g_flashLight.outerAngle);
    const float outerAngleTan = tan(g_flashLight.outerAngle);
    
    
    //BEGIN: Calculate UV coordinates for the texture mask
    //----------------------------------------------------
    //build the light view matrix
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, normalize(g_flashLight.direction)));
    float3 forward = normalize(g_flashLight.direction);
    float3 cameraUp = normalize(cross(forward, right));

    float4x4 lightViewMatrix =
        transpose(float4x4(
            float4(right, -dot(right, g_flashLight.position)),
            float4(cameraUp, -dot(cameraUp, g_flashLight.position)),
            float4(forward, -dot(forward, g_flashLight.position)),
            float4(0.0f, 0.0f, 0.0f, 1.0f))
    );
    
    //transform the world space position into light space
    float3 fragPosLightSpace = mul(float4(worldSpacePosition, 1), lightViewMatrix).xyz;

    float radius = fragPosLightSpace.z * outerAngleTan;
    
    float u = Remap(fragPosLightSpace.x, -radius, radius, 0, 1);
    float v = Remap(fragPosLightSpace.y, -radius, radius, 1, 0);

    float textureMask = flashlightTexture.Sample(g_linearWrap, float2(u, v)).rgb;
    //----------------------------------------------------
    //END: Calculate UV coordinates for the texture mask
   
    float3 lightDirection = normalize(g_flashLight.position - worldSpacePosition);
    float3 viewDirection = normalize(g_cameraPosition.xyz - worldSpacePosition);
    
    //light cone
    float angleCos = dot(-lightDirection, normalize(g_flashLight.direction));
    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);
    
    //solid angle
    float distance = length(g_flashLight.position - worldSpacePosition);
    float solidAngle = 2 * PI * (1 - sqrt(1 - pow(g_flashLight.radius / distance, 2)));
    
    float3 lightResult = CalculateLightPBR(normal, lightDirection, viewDirection, g_flashLight.color, solidAngle, g_flashLight.intensity, roughness, metalness, albedo);
    return lightResult * spotEffect * textureMask;
    
}

//END: PBR light calculation
//-------------------------------------------------------------------------------------------------------

float4 main(Input input) : SV_TARGET
{
    float3 finalColor = float3(0, 0, 0);
    float4 albedo;
    float roughness;
    float metalness;

    switch (g_samplerStateIndex)
    {
        case 0:
            albedo = albedoTexture.Sample(g_pointWrap, input.UV);
            break;
        case 1:
            albedo = albedoTexture.Sample(g_linearWrap, input.UV);
            break;
        case 2:
            albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
            break;
        default:
            albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
            break;
    }
    
    roughness = roughnessTexture.Sample(g_anisotropicWrap, input.UV).r;
    metalness = metalnessTexture.Sample(g_anisotropicWrap, input.UV).r;

    //get normals from the normal map
    float2 normalTextureSample = normalTexture.Sample(g_anisotropicWrap, input.UV).xy;
    float normalZComponent = sqrt(1 - normalTextureSample.x * normalTextureSample.x - normalTextureSample.y * normalTextureSample.y);
    float3 normalFromNormalMap = float3(normalTextureSample, normalZComponent);
    // TBN matrix in world space
    float3x3 tangentSpaceMatrix = float3x3(input.Tangent, input.Bitangent, input.Normal);
    float3 worldNormal = normalize(mul(normalFromNormalMap, tangentSpaceMatrix));

    float3 viewNormal = normalize(mul(worldNormal, (float3x3) input.View));

    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float3 lightResult = CalculateDirectionalLightPBR(viewNormal, g_directionalLights[i], input.ViewPosition.xyz, roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int j = 0; j < g_numPointLights; ++j)
    {
        float3 lightResult = CalculatePointLightPBR(viewNormal, input.ViewPosition.xyz, g_pointLights[j], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }
    
    for (int k = 0; k < g_numSpotLights; ++k)
    {
        float3 lightResult = CalculateSpotLightPBR(viewNormal, input.ViewPosition.xyz, g_spotLights[k], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int l = 0; l < g_numFlashLights; ++l)
    {
        float3 lightResult = CalculateFlashLightPBR(worldNormal, input.WorldPosition.xyz, g_flashLights[l], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    return float4(finalColor, 0.5);
}
