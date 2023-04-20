SamplerState g_pointWrap : register(s0);
SamplerState g_linearWrap : register(s1);
SamplerState g_anisotropicWrap :register(s2);

cbuffer SamplerStateIndexBuffer : register(b3)
{
    unsigned int g_samplerStateIndex;
    float3 padding;
};

// light

Texture2D flashlightTexture : register(t1); 

#define MAX_DIRECTIONAL_LIGHTS 2
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10
#define MAX_FLASH_LIGHTS 1

struct DirectionalLight
{
    float3 color;
    float intensity;
    float3 direction;
};

struct PointLight
{
    float3 color;
    float intensity;
    float4 position;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct SpotLight
{
    float3 color;
    float intensity;
    float3 position;
    float innerAngle;
    float3 direction;
    float outerAngle;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct FlashLight // currently the same as SpotLight (but may change in the future)
{
    float3 color;
    float intensity;
    float3 position;
    float innerAngle;
    float3 direction;
    float outerAngle;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

cbuffer PerFrame : register(b4)
{
    int g_numDirectionalLights;
    int g_numPointLights;
    int g_numSpotLights;
    int g_numFlashLights;
    float4 g_cameraPosition;

    DirectionalLight g_directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight g_pointLights[MAX_POINT_LIGHTS];
    SpotLight g_spotLights[MAX_SPOT_LIGHTS];
    FlashLight g_flashLights[MAX_FLASH_LIGHTS];
}


float remap(float value, float oldMin, float oldMax, float newMin, float newMax)
{
    return newMin + (value - oldMin) * (newMax - newMin) / (oldMax - oldMin);
}


//world space 

//float3 CalculateLight(float3 normal, float3 lightDirection, float3 viewDirection, float3 lightColor, float intensity)
//{
//    float3 halfVector = normalize(viewDirection + lightDirection);
//    float NdotL = max(0.0001f, dot(normal, lightDirection));
//    float NdotH = max(0.0001f, dot(normal, halfVector));

//    float specularPower = 32.0f;
//    float specularIntensity = pow(NdotH, specularPower);

//    float3 ambient = float3(0.01, 0.01, 0.01) * lightColor;
//    float3 diffuse = NdotL * lightColor;
//    float3 specular = specularIntensity * lightColor;

//    return (ambient + diffuse + specular) * intensity;
//}

//float3 CalculateDirectionalLight(float3 normal, DirectionalLight g_directionalLight, float3 worldSpacePosition, float3 cameraPosition)
//{
//    float3 lightDirection = normalize(-g_directionalLight.direction);
//    float3 viewDirection = normalize(cameraPosition - worldSpacePosition);
//    float3 lightColor = g_directionalLight.color;

//    return CalculateLight(normal, lightDirection, viewDirection, lightColor, g_directionalLight.intensity);
//}

//float3 CalculatePointLight(float3 normal, float3 worldSpacePosition, PointLight g_pointLight, float3 cameraPosition)
//{
//    float3 lightDirection = normalize(g_pointLight.position.xyz - worldSpacePosition);
//    float3 viewDirection = normalize(cameraPosition - worldSpacePosition);
//    float3 lightColor = g_pointLight.color;

//    // Calculate attenuation
//    float distance = length(g_pointLight.position.xyz - worldSpacePosition);
//    float attenuation = g_pointLight.constantAttenuation
//        + g_pointLight.linearAttenuation * distance
//        + g_pointLight.quadraticAttenuation * distance * distance;

//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_pointLight.intensity);
//    return lightResult / attenuation;
//}

//float3 CalculateSpotLight(float3 normal, float3 worldSpacePosition, SpotLight g_spotLight, float3 cameraPosition)
//{
//    float3 lightDirection = normalize(g_spotLight.position - worldSpacePosition);
//    float3 viewDirection = normalize(cameraPosition - worldSpacePosition);
//    float3 lightColor = g_spotLight.color;

//    // Calculate angle between the light direction and the spotlight direction
//    float angleCos = max(0, dot(-lightDirection, normalize(g_spotLight.direction)));
//    // Calculate the spotlight effect using smoothstep
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

//    // Calculate attenuation
//    float distance = length(g_spotLight.position - worldSpacePosition);
//    float attenuation = g_spotLight.constantAttenuation
//        + g_spotLight.linearAttenuation * distance
//        + g_spotLight.quadraticAttenuation * distance * distance;

//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_spotLight.intensity);
//    return lightResult * spotEffect / attenuation;
//}




//float3 CalculateFlashLight(float3 normal, float3 worldSpacePosition, FlashLight g_flashLight, float3 cameraPosition)
//{
//    float3 lightDirection = normalize(g_flashLight.position - worldSpacePosition);
//    float3 viewDirection = normalize(cameraPosition - worldSpacePosition);
//    float3 lightColor = g_flashLight.color;

//    // Calculate angle between the light direction and the spotlight direction
//    float angleCos = max(0, dot(-lightDirection, normalize(g_flashLight.direction)));
//    // Calculate the spotlight effect using smoothstep
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

//    // Calculate attenuation
//    float distance = length(g_flashLight.position - worldSpacePosition);
//    float attenuation = g_flashLight.constantAttenuation
//        + g_flashLight.linearAttenuation * distance
//        + g_flashLight.quadraticAttenuation * distance * distance;

//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
//    return lightResult * spotEffect / attenuation;
//}

//-----

//view space 

float3 CalculateLight(float3 normal, float3 lightDirection, float3 viewDirection, float3 lightColor, float intensity)
{
    float3 halfVector = normalize(viewDirection + lightDirection);
    float NdotL = max(0.0001f, dot(normal, lightDirection));
    float NdotH = max(0.0001f, dot(normal, halfVector));

    float specularPower = 32.0f;
    float specularIntensity = pow(NdotH, specularPower);

    float3 ambient = float3(0.01, 0.01, 0.01) * lightColor;
    float3 diffuse = NdotL * lightColor;
    float3 specular = specularIntensity * lightColor;

    return (ambient + diffuse + specular) * intensity;
}

float3 CalculateDirectionalLight(float3 normal, DirectionalLight g_directionalLight, float3 viewSpacePosition)
{
    float3 lightDirection = normalize(-g_directionalLight.direction);
    float3 viewDirection = normalize(-viewSpacePosition);
    float3 lightColor = g_directionalLight.color;

    return CalculateLight(normal, lightDirection, viewDirection, lightColor, g_directionalLight.intensity);
}

float3 CalculatePointLight(float3 normal, float3 viewSpacePosition, PointLight g_pointLight)
{
    float3 lightDirection = normalize(g_pointLight.position.xyz - viewSpacePosition);
    float3 viewDirection = normalize(-viewSpacePosition);
    float3 lightColor = g_pointLight.color;

    // Calculate attenuation
    float distance = length(g_pointLight.position.xyz - viewSpacePosition);
    float attenuation = g_pointLight.constantAttenuation
        + g_pointLight.linearAttenuation * distance
        + g_pointLight.quadraticAttenuation * distance * distance;

    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_pointLight.intensity);
    return lightResult / attenuation;
}

float3 CalculateSpotLight(float3 normal, float3 viewSpacePosition, SpotLight g_spotLight)
{
    const float innerAngleCos = cos(g_spotLight.innerAngle);
    const float outerAngleCos = cos(g_spotLight.outerAngle);
    
    
    float3 lightDirection = normalize(g_spotLight.position - viewSpacePosition);
    float3 viewDirection = normalize(-viewSpacePosition);
    float3 lightColor = g_spotLight.color;

    // Calculate angle between the light direction and the spotlight direction
    float angleCos = max(0, dot(-lightDirection, normalize(g_spotLight.direction)));
    // Calculate the spotlight effect using smoothstep
    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

    // Calculate attenuation
    float distance = length(g_spotLight.position - viewSpacePosition);
    float attenuation = g_spotLight.constantAttenuation
        + g_spotLight.linearAttenuation * distance
        + g_spotLight.quadraticAttenuation * distance * distance;

    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_spotLight.intensity);
    return lightResult * spotEffect / attenuation;
}

//float3 CalculateFlashLight(float3 normal, float3 viewSpacePosition, FlashLight g_flashLight)
//{
//    float3 lightDirection = normalize(g_flashLight.position - viewSpacePosition);
//    float3 viewDirection = normalize(-viewSpacePosition);
//    float3 lightColor = g_flashLight.color;

//    // Calculate angle between the light direction and the spotlight direction
//    float angleCos = max(0, dot(-lightDirection, normalize(g_flashLight.direction)));
//    // Calculate the spotlight effect using smoothstep
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);
//    // Calculate attenuation
//    float distance = length(g_flashLight.position - viewSpacePosition);
//    float attenuation = g_flashLight.constantAttenuation
//    + g_flashLight.linearAttenuation * distance
//    + g_flashLight.quadraticAttenuation * distance * distance;

//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
//    return lightResult * spotEffect / attenuation;
//}



//float3 CalculateSpotLight(float3 normal, float3 worldSpacePosition, SpotLight g_spotLight, float3 cameraPosition)
//{
    
//    float3 lightDirection = normalize(g_spotLight.position - worldSpacePosition);
//    float3 viewDirection = normalize(cameraPosition - worldSpacePosition);
//    float3 lightColor = g_spotLight.color;

//    // Calculate angle between the light direction and the spotlight direction
//    float angleCos = max(0, dot(-lightDirection, normalize(g_spotLight.direction)));
//    // Calculate the spotlight effect using smoothstep
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_spotLight.intensity);
//    return lightResult * spotEffect;

//}


//float3 CalculateFlashLight(float3 normal, float3 worldSpacePosition, FlashLight g_flashLight, float3 cameraPosition)
//{



//    float3 lightDirection = normalize(g_flashLight.position - worldSpacePosition);
//    float3 viewDirection = normalize(cameraPosition - worldSpacePosition);
//    float3 lightColor = g_flashLight.color;

//    // Calculate angle between the light direction and the spotlight direction
//    float angleCos = max(0, dot(-lightDirection, normalize(g_flashLight.direction)));
//    // Calculate the spotlight effect using smoothstep
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

    
//    // Calculate attenuation
//    float distance = length(g_flashLight.position - worldSpacePosition);
//    float attenuation = g_flashLight.constantAttenuation
//        + g_flashLight.linearAttenuation * distance
//        + g_flashLight.quadraticAttenuation * distance * distance;

//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
//    return lightResult * spotEffect / attenuation;
    

    

//}


// view space 

//float3 CalculateLight(float3 normal, float3 lightDirection, float3 viewDirection, float3 lightColor, float intensity)
//{
//    float3 halfVector = normalize(viewDirection + lightDirection);
//    float NdotL = max(0, dot(normal, lightDirection));
//    float NdotH = max(0, dot(normal, halfVector));

//    float specularPower = 32.0f;
//    float specularIntensity = pow(NdotH, specularPower);

//    float3 ambient = float3(0.1, 0.1, 0.1) * lightColor;
//    float3 diffuse = NdotL * lightColor;
//    float3 specular = specularIntensity * lightColor;

//    //return (diffuse + specular) * intensity;
//    return (ambient + diffuse + specular) * intensity;
//}

//float3 CalculateDirectionalLight(float3 normal, DirectionalLight g_directionalLight, float3 viewSpacePosition)
//{
//    float3 lightDirection = normalize(-g_directionalLight.direction);
//    float3 viewDirection = normalize(-viewSpacePosition);
//    float3 lightColor = g_directionalLight.color;

//    return CalculateLight(normal, lightDirection, viewDirection, lightColor, g_directionalLight.intensity);
//}

//float3 CalculatePointLight(float3 normal, float3 position, PointLight g_pointLight)
//{
//    float3 lightDirection = normalize(g_pointLight.position.xyz - position);
//    float3 viewDirection = normalize(-position);
//    float3 lightColor = g_pointLight.color;

//    return CalculateLight(normal, lightDirection, viewDirection, lightColor, g_pointLight.intensity);
//}

//float3 CalculateSpotLight(float3 normal, float3 position, SpotLight g_spotLight)
//{
//    float3 lightDirection = normalize(g_spotLight.position - position);
//    float3 viewDirection = normalize(-position);
//    float3 lightColor = g_spotLight.color;

//    float angleCos = dot(-lightDirection, normalize(g_spotLight.direction));
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_spotLight.intensity);
//    return lightResult * spotEffect;
//}


//view space final
//float3 CalculateFlashLight(float3 normal, float3 position, FlashLight g_flashLight)
//{
//    const float innerAngleCos = cos(g_flashLight.innerAngle);
//    const float outerAngleCos = cos(g_flashLight.outerAngle);
//    const float outerAngleTan = tan(g_flashLight.outerAngle);
    
//    float3 lightDirection = normalize(g_flashLight.position - position);
//    float3 viewDirection = normalize(-position);
//    float3 lightColor = g_flashLight.color;

//    float angleCos = dot(-lightDirection, normalize(g_flashLight.direction));
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

//    // Calculate UV coordinates
//    // 1
//    // --------------------------------------

//    float3 up = float3(0.0f, 1.0f, 0.0f);
//    float3 right = cross(g_flashLight.direction, up);

//    float4x4 lightViewMatrixInv = transpose(float4x4(
//        float4(right, 0.0f),
//        float4(up, 0.0f),
//        float4(g_flashLight.direction, 0.0f),
//        float4(g_flashLight.position, 1.0f)
//    ));
    
//    float3 fragPosLightSpace = mul(lightViewMatrixInv, float4(position, 1)).xyz;
    
//    float radius = fragPosLightSpace.z * outerAngleTan;
    
//    float v = remap(-radius, radius, 0, 1, fragPosLightSpace.x);
//    float u = remap(-radius, radius, 0, 1, fragPosLightSpace.y);

//    float2 uv = float2(u, v) * 0.5 + 0.5;
    
//    float maskValue = flashlightTexture.Sample(g_linearWrap, uv).rgb;

//    // --------------------------------------
    
//    //float NdotL = dot(lightDirection, normal);
//    //float u = (NdotL + 1.0) * 0.5;
//    //float v = dot(lightDirection, surfaceTangent) * 0.5 + 0.5;
//    //float2 uv = float2(u, v);
//    //float maskValue = flashlightTexture.Sample(g_linearWrap, uv).r;
    
//    //float3 localPosition = -g_flashLight.position;
//    //float2 texCoord;
//    //texCoord.x = dot(normalize(localPosition), normalize(cross(float3(0, 1, 0), g_flashLight.direction)));
//    //texCoord.y = dot(normalize(localPosition), normalize(float3(0, 1, 0)));
//    //texCoord = texCoord * 0.5 + 0.5; // Convert to [0, 1] range

//    // Sample flashlight mask texture
//    //float maskValue = flashlightTexture.Sample(g_linearWrap, texCoord).r;

    
    
    
//    // Calculate attenuation
//    float distance = length(position);
//    float attenuation = g_flashLight.constantAttenuation
//        + g_flashLight.linearAttenuation * distance
//        + g_flashLight.quadraticAttenuation * distance * distance;

    
//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
//    return lightResult * spotEffect * maskValue / attenuation;
//}

float3 CalculateFlashLight(float3 normal, float3 position, FlashLight g_flashLight)
{
    const float innerAngleCos = cos(g_flashLight.innerAngle);
    const float outerAngleCos = cos(g_flashLight.outerAngle);
    const float outerAngleTan = tan(g_flashLight.outerAngle);
    
    
    float3 lightDirection = normalize(g_flashLight.position - position);
    float3 viewDirection = normalize(g_cameraPosition.xyz - position); // world space
    float3 lightColor = g_flashLight.color;

    float angleCos = dot(-lightDirection, normalize(g_flashLight.direction));
    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

    // Calculate UV coordinates
    // 1
    // --------------------------------------

    
    //build the light view matrix
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, normalize(g_flashLight.direction)));
    float3 forward = normalize(g_flashLight.direction);

    float4x4 lightViewMatrixInv = float4x4(
    float4(right, 0.0f),
    float4(up, 0.0f),
    float4(forward, 0.0f),
    float4(0.0f, 0.0f, 0.0f, 1.0f)
);

    lightViewMatrixInv._41 = -dot(right, g_flashLight.direction);
    lightViewMatrixInv._42 = -dot(up, g_flashLight.direction);
    lightViewMatrixInv._43 = -dot(forward, g_flashLight.direction);

    
    float3 fragPosLightSpace = mul(float4(position, 1), lightViewMatrixInv).xyz;
    
    float radius = fragPosLightSpace.z * outerAngleTan;
    
    float v = remap(fragPosLightSpace.x, -radius, radius, 0, 1);
    float u = remap(fragPosLightSpace.y , -radius, radius, 0, 1);

    float2 uv = float2(u, v) * 0.5 + 0.5;
    
    float maskValue = flashlightTexture.Sample(g_linearWrap, uv).rgb;

    // --------------------------------------
    
    // Calculate attenuation
    float distance = length(g_flashLight.position - position);
    float attenuation = g_flashLight.constantAttenuation
        + g_flashLight.linearAttenuation * distance
        + g_flashLight.quadraticAttenuation * distance * distance;
    
    //float distance = length(position);
    //float attenuation = g_flashLight.constantAttenuation
    //    + g_flashLight.linearAttenuation * distance
    //    + g_flashLight.quadraticAttenuation * distance * distance;

    
    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
    return lightResult * spotEffect * maskValue /*/ attenuation*/;
}



//float3 CalculateFlashLight(float3 normal, float3 position, FlashLight g_flashLight)
//{
//    //float3 lightDirection = normalize(g_flashLight.position - position);
//    //float3 viewDirection = normalize(-position);
//    //float3 lightColor = g_flashLight.color;

//    //float angleCos = dot(-lightDirection, normalize(g_flashLight.direction));
//    //float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

//    //float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
//    //return lightResult * spotEffect;
    
//    float3 lightDirection = normalize(g_flashLight.position - position);
//    float3 viewDirection = normalize(-position);
//    float3 lightColor = g_flashLight.color;

//    float angleCos = dot(-lightDirection, normalize(g_flashLight.direction));
//    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

//    // Calculate UV coordinates
//    float2 uv;
//    uv.x = dot(normalize(normal), normalize(lightDirection)) * 0.5 + 0.5;
//    uv.y = dot(normalize(normal), normalize(viewDirection)) * 0.5 + 0.5;

//    // Apply texture mask
//    float maskValue = flashlightTexture.Sample(g_linearWrap, uv).r;
//    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
//    return lightResult * spotEffect * maskValue;
//}


float computeFlashlightMask(float2 uv, float3 lightDirection, float3 normal)
{
    float4 flashlightMask = flashlightTexture.Sample(g_linearWrap, uv);

    // Compute the dot product between the light direction and the normal
    float NdotL = max(0, dot(normal, lightDirection));

    // Multiply the mask value by the dot product
    return flashlightMask.r * NdotL;
}
