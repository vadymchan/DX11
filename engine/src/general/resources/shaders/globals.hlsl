SamplerState g_pointWrap : register(s0);
SamplerState g_linearWrap : register(s1);
SamplerState g_anisotropicWrap :register(s2);

cbuffer SamplerStateIndexBuffer : register(b3)
{
    unsigned int g_samplerStateIndex;
    float3 padding;
};


#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

struct DirectionalLight
{
    float3 direction;
    float pad1;
    float3 color;
    float pad2;
};

struct PointLight
{
    float3 position;
    float pad1;
    float3 color;
    float pad2;
};

struct SpotLight
{
    float3 position;
    float innerConeCos;
    float3 direction;
    float outerConeCos;
    float3 color;
    float pad;
};

cbuffer PerFrame : register(b4)
{
    int g_numDirectionalLights;
    int g_numPointLights;
    int g_numSpotLights;
    float3 g_cameraPosition;

    
    
    DirectionalLight g_directionalLights[MAX_DIRECTIONAL_LIGHTS];

    
    PointLight g_pointLights[MAX_POINT_LIGHTS];

    
    SpotLight g_spotLights[MAX_SPOT_LIGHTS];
}

// ... The rest of the shader code ...


float3 CalculateLight(float3 normal, float3 lightDirection, float3 viewDirection, float3 lightColor)
{
    float3 halfVector = normalize(viewDirection + lightDirection);
    float NdotL = max(0, dot(normal, lightDirection));
    float NdotH = max(0, dot(normal, halfVector));

    float specularPower = 32.0f;
    float specularIntensity = pow(NdotH, specularPower);

    float3 ambient = float3(0.1, 0.1, 0.1) * lightColor;
    float3 diffuse = NdotL * lightColor;
    float3 specular = specularIntensity * lightColor;

    return ambient + diffuse + specular;
}

float3 CalculateDirectionalLight(float3 normal, DirectionalLight g_directionalLight, float3 viewSpacePosition)
{
    float3 lightDirection = normalize(-g_directionalLight.direction);
    float3 viewDirection = normalize(-viewSpacePosition);
    float3 lightColor = g_directionalLight.color;

    return CalculateLight(normal, lightDirection, viewDirection, lightColor);
}

float3 CalculatePointLight(float3 normal, float3 position, PointLight g_pointLight)
{
    float3 lightDirection = normalize(g_pointLight.position - position);
    float3 viewDirection = normalize(-position);
    float3 lightColor = g_pointLight.color;

    return CalculateLight(normal, lightDirection, viewDirection, lightColor);
}

float3 CalculateSpotLight(float3 normal, float3 position, SpotLight g_spotLight)
{
    float3 lightDirection = normalize(g_spotLight.position - position);
    float3 viewDirection = normalize(-position);
    float3 lightColor = g_spotLight.color;

    float angleCos = dot(-lightDirection, normalize(g_spotLight.direction));
    float spotEffect = smoothstep(g_spotLight.outerConeCos, g_spotLight.innerConeCos, angleCos);

    float3 lightResult = CalculateLight(normal, lightDirection, viewDirection, lightColor);
    return lightResult * spotEffect;
}

//float4 main(PS_INPUT input) : SV_TARGET
//{
//    float3 normal = normalize(input.Normal);
//    float3 position = input.ViewSpacePosition;

//    float3 directionalLightColor = CalculateDirectionalLight(normal);
//    float3 pointLightColor = CalculatePointLight(normal, position);
//    float3 spotLightColor = CalculateSpotLight(normal, position);

//    float3 finalColor = directionalLightColor + pointLightColor + spotLightColor;

//    // Multiply the calculated lighting with the object's base color (from texture or material properties)

//    return float4(finalColor, 1.0f);
//}

