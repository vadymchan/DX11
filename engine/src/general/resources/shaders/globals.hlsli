SamplerState g_pointWrap : register(s0);
SamplerState g_linearWrap : register(s1);
SamplerState g_anisotropicWrap : register(s2);

cbuffer SamplerStateIndexBuffer : register(b3)
{
    unsigned int g_samplerStateIndex;
    float3 padding;
};

// light
Texture2D flashlightTexture : register(t7);

#define MAX_DIRECTIONAL_LIGHTS 2
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10
#define MAX_FLASH_LIGHTS 1

struct DirectionalLight
{
    float3 color;
    float intensity;
    float3 direction;
    
    //for PBR shading
    float solidAngle;
};

struct PointLight
{
    float3 color;
    float intensity;
    float3 position;
    float radius;
    
    // for blinn-phong shading
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
    
    // for PBR shading
    float radius;
    // for blinn-phong shading
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
    
    // for PBR shading
    float radius;
    // for blinn-phong shading
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



float Remap(float value, float oldMin, float oldMax, float newMin, float newMax)
{
    return newMin + (value - oldMin) * (newMax - newMin) / (oldMax - oldMin);
}

//BEGIN: Blinn-Phong light calculation
//-------------------------------------------------------------------------------------------------------

float3 CalculateLightBlinnPhong(float3 normal, float3 lightDirection, float3 viewDirection, float3 lightColor, float intensity)
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

float3 CalculateDirectionalLightBlinnPhong(float3 normal, DirectionalLight g_directionalLight, float3 position)
{
    float3 lightDirection = normalize(-g_directionalLight.direction);
    float3 viewDirection = normalize(-position);
    float3 lightColor = g_directionalLight.color;

    return CalculateLightBlinnPhong(normal, lightDirection, viewDirection, lightColor, g_directionalLight.intensity);
}

float3 CalculatePointLightBlinnPhong(float3 normal, float3 position, PointLight g_pointLight)
{
    float3 lightDirection = normalize(g_pointLight.position.xyz - position);
    float3 viewDirection = normalize(-position);
    float3 lightColor = g_pointLight.color;

    // Calculate attenuation
    float distance = length(- position);
    float attenuation = g_pointLight.constantAttenuation
        + g_pointLight.linearAttenuation * distance
        + g_pointLight.quadraticAttenuation * distance * distance;

    float3 lightResult = CalculateLightBlinnPhong(normal, lightDirection, viewDirection, lightColor, g_pointLight.intensity);
    return lightResult / attenuation;
}

float3 CalculateSpotLightBlinnPhong(float3 normal, float3 position, SpotLight g_spotLight)
{
    const float innerAngleCos = cos(g_spotLight.innerAngle);
    const float outerAngleCos = cos(g_spotLight.outerAngle);
    
    
    float3 lightDirection = normalize(g_spotLight.position - position);
    float3 viewDirection = normalize(-position);

    // Calculate angle between the light direction and the spotlight direction
    float angleCos = max(0, dot(-lightDirection, normalize(g_spotLight.direction)));
    // Calculate the spotlight effect using smoothstep
    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

    // Calculate attenuation
    float distance = length(-position);
    float attenuation = g_spotLight.constantAttenuation
        + g_spotLight.linearAttenuation * distance
        + g_spotLight.quadraticAttenuation * distance * distance;

    float3 lightResult = CalculateLightBlinnPhong(normal, lightDirection, viewDirection, g_spotLight.color, g_spotLight.intensity);
    return lightResult * spotEffect / attenuation;
}

float3 CalculateFlashLightBlinnPhong(float3 normal, float3 position, FlashLight g_flashLight)
{
    const float innerAngleCos = cos(g_flashLight.innerAngle);
    const float outerAngleCos = cos(g_flashLight.outerAngle);
    const float outerAngleTan = tan(g_flashLight.outerAngle);
    
    
    float3 lightDirection = normalize(g_flashLight.position - position);
    float3 viewDirection = normalize(g_cameraPosition.xyz - position); // world space
    //float3 viewDirection = normalize(-position); // view space
    float3 lightColor = g_flashLight.color;

    float angleCos = dot(-lightDirection, normalize(g_flashLight.direction));
    float spotEffect = smoothstep(outerAngleCos, innerAngleCos, angleCos);

    // Calculate UV coordinates
    
    //build the light view matrix
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, normalize(g_flashLight.direction)));
    float3 forward = normalize(g_flashLight.direction);
    float3 cameraUp = normalize(cross(forward, right));


    float4x4 lightViewMatrix = 
        transpose( float4x4(
            float4(right, -dot(right, g_flashLight.position)),
            float4(cameraUp, -dot(cameraUp, g_flashLight.position)),
            float4(forward, -dot(forward, g_flashLight.position)),
            float4(0.0f, 0.0f, 0.0f, 1.0f))
    );


    
    
    float3 fragPosLightSpace = mul(float4(position, 1), lightViewMatrix).xyz;
    
    //float3 fragPosLightSpace = mul(position - g_flashLight.position, (float3x3)lightViewMatrix).xyz;
    
   
    float radius = fragPosLightSpace.z * outerAngleTan;
    
    float u = Remap(fragPosLightSpace.x, -radius, radius, 0, 1);
    float v = Remap(fragPosLightSpace.y, -radius, radius, 1, 0);

    float textureMask = flashlightTexture.Sample(g_linearWrap, float2(u, v)).rgb;

    
    // Calculate attenuation
    float distance = length(g_flashLight.position - position);
    
    //float distance = length(position);
    float attenuation = g_flashLight.constantAttenuation
        + g_flashLight.linearAttenuation * distance
        + g_flashLight.quadraticAttenuation * distance * distance;

    
    float3 lightResult = CalculateLightBlinnPhong(normal, lightDirection, viewDirection, lightColor, g_flashLight.intensity);
    return lightResult * spotEffect * textureMask / attenuation;
}

//END: Blinn-Phong light calculation
//-------------------------------------------------------------------------------------------------------

