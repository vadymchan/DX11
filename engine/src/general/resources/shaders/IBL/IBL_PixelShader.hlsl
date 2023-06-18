#include "../globals.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D roughnessTexture : register(t1);
Texture2D metalnessTexture : register(t2);
Texture2D normalTexture : register(t3);
TextureCube irradianceMap : register(t4);
TextureCube prefilterMap : register(t5);
Texture2D brdfLUT : register(t6);

cbuffer PerFrameIBL : register(b5)
{
    bool useDiffuseReflection;
    bool useSpecularReflection;
    bool useIBL;
    bool useRoughnessOverwriting;
    float overwrittenRoughnessValue;
}

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



//BEGIN: IBL light calculation
//-------------------------------------------------------------------------------------------------------

//diffuse part
//------------
float3 GetIBLDiffuseIrradiance(float3 normal)
{
    return irradianceMap.Sample(g_linearWrap, normal).rgb;
}

float3 CalculateIBLDiffuse(float3 normal, float3 albedo, float metallness)
{
    return GetIBLDiffuseIrradiance(normal) * albedo * (1 - metallness);
}
//------------
//END: diffuse part

//specular part
//-------------

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    float3 inverseRoughness = float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness);
    return F0 + (max(inverseRoughness, F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 GetIBLSpecularIrradiance(float roughness, float3 viewDirection, float3 normal)
{
    const float MAX_REFLECTION_LOD = 4.0;
    float3 R = reflect(-viewDirection, normal);
    R.y = -R.y; // flip the y coordinate 
    R.x = -R.x;
    return prefilterMap.SampleLevel(g_linearWrap, R, roughness * MAX_REFLECTION_LOD).rgb;
}


float2 GetBRDFlutTexture(float VdotN, float roughness)
{
    return brdfLUT.Sample(g_linearWrap, float2(VdotN, roughness)).rg;
}

float3 CalculateIBLSpecular(float3 normal, float3 viewDirection, float roughness, float3 F0)
{
    const float NdotV = dot(normal, viewDirection);
    
    float3 specularIrradiance = GetIBLSpecularIrradiance(roughness, viewDirection, normal); // E_spec
    float2 brdf = GetBRDFlutTexture(NdotV, roughness); // F_spec
    float F = fresnelSchlickRoughness(max(NdotV, 0.0), F0, roughness); // F_spec
    
    return specularIrradiance * (F * brdf.x + brdf.y);
}
//------------------
//END: specular part

//-------------------------------------------------------------------------------------------------------
//END: IBL light calculation



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
 

//BEGIN: double horizon falloff factor
//--------------------------------------------------------------------------------------------

float DoubleFallOffFactor(float3 surfaceNormal, float3 normal, float3 lightDirection, float r)
{
    // Compute the dot products of the light direction with the normals
    float NdotL_geo = dot(surfaceNormal, lightDirection);
    float NdotL_map = dot(normal, lightDirection);

    // Compute the falloff factors for the geometry and normal map normals
    float falloff_geo = min(1.0, (NdotL_geo + r) / (2 * max(r, 0.5f)));
    float falloff_map = min(1.0, (NdotL_map + r) / (2 * max(r, 0.5f)));

    // Combine the falloff factors. This is just one possible way to combine them;
    // the exact method will depend on the specific effect you're trying to achieve.
    float falloff = min(falloff_geo, falloff_map);

    return falloff;
}

//--------------------------------------------------------------------------------------------
//END: double horizon falloff factor



// May return direction pointing beneath surface horizon (dot(N, dir) < 0), use clampDirToHorizon to fix it.
// sphereCos is cosine of the light source angular halfsize (2D angle, not solid angle).
// sphereRelPos is position of a sphere relative to surface:
// 'sphereDir == normalize(sphereRelPos)' and 'sphereDir * sphereDist == sphereRelPos'
float3 approximateClosestSphereDir(out bool intersects, float3 reflectionDir, float sphereCos,
    float3 sphereRelPos, float3 sphereDir, float sphereDist, float sphereRadius)
{
    float RoS = dot(reflectionDir, sphereDir);

    intersects = RoS >= sphereCos;
    if (intersects)
        return reflectionDir;
    if (RoS < 0.0)
        return sphereDir;

    float3 closestPointDir = normalize(reflectionDir * sphereDist * RoS - sphereRelPos);
    return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

// Input dir and NoD is N and NoL in a case of lighting computation 
void clampDirToHorizon(inout float3 dir, inout float NoD, float3 normal, float minNoD)
{
    if (NoD < minNoD)
    {
        dir = normalize(dir + (minNoD - NoD) * normal);
        NoD = minNoD;
    }
}



// [ de Carpentier 2017, "Decima Engine: Advances in Lighting and AA" ]
// sphereSin and sphereCos are sin and cos of the light source angular halfsize (2D angle, not solid angle).
void SphereMaxNoH(float NoV, inout float NoL, inout float VoL, float sphereSin, float sphereCos, bool bNewtonIteration, out float NoH, out float VoH)
{
    float RoL = 2 * NoL * NoV - VoL;
    if (RoL >= sphereCos)
    {
        NoH = 1;
        VoH = abs(NoV);
    }
    else
    {
        float rInvLengthT = sphereSin * rsqrt(1 - RoL * RoL);
        float NoTr = rInvLengthT * (NoV - RoL * NoL);
        float VoTr = rInvLengthT * (2 * NoV * NoV - 1 - RoL * VoL);

        if (bNewtonIteration && sphereSin != 0)
        {
			// dot( cross(N,L), V )
            float NxLoV = sqrt(saturate(1 - pow(NoL, 2) - pow(NoV, 2) - pow(VoL, 2) + 2 * NoL * NoV * VoL));

            float NoBr = rInvLengthT * NxLoV;
            float VoBr = rInvLengthT * NxLoV * 2 * NoV;

            float NoLVTr = NoL * sphereCos + NoV + NoTr;
            float VoLVTr = VoL * sphereCos + 1 + VoTr;

            float p = NoBr * VoLVTr;
            float q = NoLVTr * VoLVTr;
            float s = VoBr * NoLVTr;

            float xNum = q * (-0.5 * p + 0.25 * VoBr * NoLVTr);
            float xDenom = p * p + s * (s - 2 * p) + NoLVTr * ((NoL * sphereCos + NoV) * pow(VoLVTr, 2) + q * (-0.5 * (VoLVTr + VoL * sphereCos) - 0.5));
            float TwoX1 = 2 * xNum / (pow(xDenom, 2) + pow(xNum, 2));
            float SinTheta = TwoX1 * xDenom;
            float CosTheta = 1.0 - TwoX1 * xNum;
            NoTr = CosTheta * NoTr + SinTheta * NoBr;
            VoTr = CosTheta * VoTr + SinTheta * VoBr;
        }

        NoL = NoL * sphereCos + NoTr;
        VoL = VoL * sphereCos + VoTr;

        float InvLenH = rsqrt(2 + 2 * VoL);
        NoH = saturate((NoL + NoV) * InvLenH);
        VoH = saturate(InvLenH + InvLenH * VoL);
    }
}

// [ de Carpentier 2017, "Decima Engine: Advances in Lighting and AA" ]
float GetNoHSquared(float radiusTan, float NoL, float NoV, float VoL)
{
    // radiusCos can be precalculated if radiusTan is a directional light
    float radiusCos = rsqrt(1.0 + radiusTan * radiusTan);
    
    // Early out if R falls within the disc
    float RoL = 2.0 * NoL * NoV - VoL;
    if (RoL >= radiusCos)
        return 1.0;

    float rOverLengthT = radiusCos * radiusTan * rsqrt(1.0 - RoL * RoL);
    float NoTr = rOverLengthT * (NoV - RoL * NoL);
    float VoTr = rOverLengthT * (2.0 * NoV * NoV - 1.0 - RoL * VoL);

    // Calculate dot(cross(N, L), V). This could already be calculated and available.
    float triple = sqrt(saturate(1.0 - NoL * NoL - NoV * NoV - VoL * VoL + 2.0 * NoL * NoV * VoL));
    
    // Do one Newton iteration to improve the bent light vector
    float NoBr = rOverLengthT * triple, VoBr = rOverLengthT * (2.0 * triple * NoV);
    float NoLVTr = NoL * radiusCos + NoV + NoTr, VoLVTr = VoL * radiusCos + 1.0 + VoTr;
    float p = NoBr * VoLVTr, q = NoLVTr * VoLVTr, s = VoBr * NoLVTr;
    float xNum = q * (-0.5 * p + 0.25 * VoBr * NoLVTr);
    float xDenom = p * p + s * ((s - 2.0 * p)) + NoLVTr * ((NoL * radiusCos + NoV) * VoLVTr * VoLVTr +
                   q * (-0.5 * (VoLVTr + VoL * radiusCos) - 0.5));
    float twoX1 = 2.0 * xNum / (xDenom * xDenom + xNum * xNum);
    float sinTheta = twoX1 * xDenom;
    float cosTheta = 1.0 - twoX1 * xNum;
    NoTr = cosTheta * NoTr + sinTheta * NoBr; // use new T to update NoTr
    VoTr = cosTheta * VoTr + sinTheta * VoBr; // use new T to update VoTr
    
    // Calculate (N.H)^2 based on the bent light vector
    float newNoL = NoL * radiusCos + NoTr;
    float newVoL = VoL * radiusCos + VoTr;
    float NoH = NoV + newNoL;
    float HoH = 2.0 * newVoL + 2.0;
    return max(0.0, NoH * NoH / HoH);
}


float3 CalculateLightPBR(
    float3 normal,
    float3 surfaceNormal,
    float3 lightDirection,
    float3 viewDirection,
    float3 lightColor,
    float solidAngle,
    float lightRadius,
    float intensity,
    float roughness,
    float metalness,
    float3 albedo)
{
    
    float lightDistance = length(lightDirection);
    
    lightDirection = normalize(lightDirection);
    
    float minNoD = 0.001f;
    float NdotL = dot(normal, lightDirection);
    //float NdotL = max(0.001f, dot(normal, lightDirection));
    clampDirToHorizon(lightDirection, NdotL, normal, minNoD);
    
    float3 halfVector = normalize(viewDirection + lightDirection);
    float NdotV = max(0.001f, dot(normal, viewDirection));
    float NdotH = max(0.001f, dot(normal, halfVector));
    float VdotH = max(0.001f, dot(viewDirection, halfVector));
    float LdotH = max(0.001f, dot(lightDirection, halfVector));


    float F0 = lerp(0.04, albedo, metalness);

    // Diffuse component
    float3 lambertian = (solidAngle * albedo * (1 - metalness)) / PI;
    lambertian *= (float3(1, 1, 1) - Fresnel(NdotL, float3(F0, F0, F0)));
    lambertian *= NdotL;

    
    // sphere area light
    // -------------------------------------------------------------
    float radiusSin = lightRadius / lightDistance;
    // gives the same result as:
    //float thirdSide = sqrt(lightDistance * lightDistance - lightRadius * lightRadius);
    //float radiusTan = lightRadius / thirdSide;
    
    float VdotL = dot(viewDirection, lightDirection);
    NdotH = GetNoHSquared(radiusSin, NdotL, NdotV, VdotL);

    //the same formula
    //SphereMaxNoH(NdotV, NdotL, VdotL, radiusSin, sqrt(1 - radiusSin * radiusSin), true, NdotH, VdotH);
    
    // -------------------------------------------------------------

    // Specular component
    float roughness2 = roughness * roughness;
    float D = GGX(roughness2, NdotH);
    NdotL = max(0.000f, NdotL);
    float G = Smith(roughness2, NdotV, NdotL);
    float3 F = Fresnel(LdotH, float3(F0, F0, F0));
    float3 cook_torrance = (min(1, D * solidAngle / (4 * NdotL)) * G * F );

    float falloff = DoubleFallOffFactor( surfaceNormal, normal, lightDirection, lightRadius);
    
    return falloff * lightColor * intensity * (lambertian + cook_torrance);
    
    
   
}



//=============================================================================================================

float3 CalculateDirectionalLightPBR(float3 normal, float3 macroNormal, DirectionalLight g_directionalLight, float3 position, float roughness, float metalness, float3 albedo)
{
    float3 lightDirection = normalize(-g_directionalLight.direction);
    float3 viewDirection = normalize(-position);
    float lightRadius = 0;

    return CalculateLightPBR(normal, macroNormal, lightDirection, viewDirection, g_directionalLight.color, g_directionalLight.solidAngle, lightRadius, g_directionalLight.intensity, roughness, metalness, albedo);
}

float3 CalculatePointLightPBR(float3 normal, float3 macroNormal, float3 position, PointLight g_pointLight, float roughness, float metalness, float3 albedo)
{
    float3 lightDirection = g_pointLight.position - position;
    float3 viewDirection = normalize(-position);

    float distance = length(g_pointLight.position - position);
    float solidAngle = 2 * PI * (1 - sqrt(1 - pow(g_pointLight.radius / distance, 2)));


    float3 lightResult = CalculateLightPBR(normal, macroNormal, lightDirection, viewDirection, g_pointLight.color, solidAngle, g_pointLight.radius, g_pointLight.intensity, roughness, metalness, albedo);
    return lightResult;
    
}


float3 CalculateSpotLightPBR(float3 normal, float3 macroNormal, float3 position, SpotLight g_spotLight, float roughness, float metalness, float3 albedo)
{
    float3 lightDirection = g_spotLight.position - position;
    float3 lightDirectionNormalized = normalize(lightDirection);
    float3 viewDirection = normalize(-position);
    
    float outerConeCos = cos(g_spotLight.outerAngle);
    float innerConeCos = cos(g_spotLight.innerAngle);
    float angleCos = dot(-lightDirectionNormalized, g_spotLight.direction);
    
    float distance = length(g_spotLight.position - position);
    float solidAngle = 2 * PI * (1 - sqrt(1 - pow(g_spotLight.radius / distance, 2)));
    
    float spotEffect = smoothstep(outerConeCos, innerConeCos, angleCos);

    float3 lightResult = CalculateLightPBR(normal, macroNormal, lightDirection, lightDirectionNormalized, g_spotLight.color, solidAngle, g_spotLight.radius, g_spotLight.intensity, roughness, metalness, albedo);
    return lightResult * spotEffect;
}

float3 CalculateFlashLightPBR(float3 normal, float3 macroNormal, float3 worldSpacePosition, FlashLight g_flashLight, float roughness, float metalness, float3 albedo)
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
    
    float3 lightResult = CalculateLightPBR(normal, macroNormal, lightDirection, viewDirection, g_flashLight.color, solidAngle, g_flashLight.radius, g_flashLight.intensity, roughness, metalness, albedo);
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


    
    albedo = albedoTexture.Sample(g_anisotropicWrap, input.UV);
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
    float3 viewMacroNormal = normalize(mul(input.Normal, (float3x3) input.View));

    // PBR
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------
    for (int i = 0; i < g_numDirectionalLights; ++i)
    {
        float3 lightResult = CalculateDirectionalLightPBR(viewNormal, viewMacroNormal, g_directionalLights[i], input.ViewPosition.xyz, roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int j = 0; j < g_numPointLights; ++j)
    {
        float3 lightResult = CalculatePointLightPBR(viewNormal, viewMacroNormal, input.ViewPosition.xyz, g_pointLights[j], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }
    
    for (int k = 0; k < g_numSpotLights; ++k)
    {
        float3 lightResult = CalculateSpotLightPBR(viewNormal, viewMacroNormal,  input.ViewPosition.xyz, g_spotLights[k], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }

    for (int l = 0; l < g_numFlashLights; ++l)
    {
        float3 lightResult = CalculateFlashLightPBR(worldNormal, input.Normal, input.WorldPosition.xyz, g_flashLights[l], roughness, metalness, albedo.rgb);
        finalColor += lightResult;
    }
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------
    
    
    //IBL
    //------------------------------------------------------------------------------------------------------------
    
    if (useIBL)
    {
        if (useDiffuseReflection)
        {
            float3 iblDiffuse = CalculateIBLDiffuse(worldNormal, albedo.rgb, metalness);
            finalColor += albedo.rgb * iblDiffuse;
        }

        if (useRoughnessOverwriting)
        {
            roughness = overwrittenRoughnessValue;
        }

        if (useSpecularReflection)
        {
            float F0 = lerp(0.04, albedo, metalness);
            float3 iblSpecular = CalculateIBLSpecular(worldNormal, normalize(input.WorldPosition.xyz - g_cameraPosition.xyz), roughness, float3(F0, F0, F0));
            finalColor += iblSpecular;
        }
    }

    
    //------------------------------------------------------------------------------------------------------------
    
    
    return float4(finalColor, 0.5);
}
