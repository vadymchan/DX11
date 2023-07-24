TextureCube environmentMap : register(t0);
SamplerState CubeSampler : register(s0);

const static float PI = 3.14159265359;
const static float GOLDEN_RATIO = (1.0 + sqrt(5.0)) / 2.0;

struct Input
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : WORLD_POSITION;
};

float3 randomHemisphere(out float NdotV, float i, float N)
{
    float theta = 2.0 * PI * i / GOLDEN_RATIO;
    float phiCos = NdotV = 1.0 - (i + 0.5) / N;
    float phiSin = sqrt(1.0 - phiCos * phiCos);
    float thetaCos, thetaSin;
    sincos(theta,  thetaSin,  thetaCos);
    return float3(thetaCos * phiSin, thetaSin * phiSin, phiCos);
}

float4 main(Input input) : SV_TARGET
{
    return float4(1, 0, 0, 0);
    
    
    float3 N = normalize(input.WorldPos);

    float3 irradiance = float3(0.0, 0.0, 0.0);
    
    // tangent space calculation from origin point
    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float Nsamples = 1024.0; // Number of samples (e.g. 1024)
    float NdotV;
    for (float i = 0.0; i < Nsamples; i++)
    {
        // Use the Fibonacci sampling
        float3 tangentSample = randomHemisphere(NdotV, i, Nsamples);

        // tangent space to world
        float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

        float theta = acos(NdotV);
        irradiance += environmentMap.Sample(CubeSampler, sampleVec).rgb * NdotV * sin(theta);
    }
    irradiance = PI * irradiance * (1.0 / Nsamples);
    
    return float4(irradiance, 1.0);
}


// uniform samping

//---------------------------------------------------------

//TextureCube environmentMap : register(t0);
//SamplerState CubeSampler : register(s0);

//const static float PI = 3.14159265359;

//struct Input
//{
//    float4 Pos : SV_POSITION;
//    float3 WorldPos : WORLD_POSITION;
//    //float3 TexCoord : TEXCOORD0;
    
//};

//float4 main(Input input) : SV_TARGET
//{
//    float3 N = normalize(input.WorldPos);

//    float3 irradiance = float3(0.0, 0.0, 0.0);
    
//    // tangent space calculation from origin point
//    float3 up = float3(0.0, 1.0, 0.0);
//    float3 right = normalize(cross(up, N));
//    up = normalize(cross(N, right));
       
//    float sampleDelta = 0.025;
//    float nrSamples = 0.0f;
//    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
//    {
//        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
//        {
//            // spherical to cartesian (in tangent space)
//            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
//            // tangent space to world
//            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

//            irradiance += environmentMap.Sample(CubeSampler, sampleVec).rgb * cos(theta) * sin(theta);
//            nrSamples++;
//        }
//    }
//    irradiance = PI * irradiance * (1.0 / nrSamples);
    
//    //for debugging purposes
//    //return float4(environmentMap.Sample(CubeSampler, input.TexCoord).rgb, 1.0);
    
//    return float4(irradiance, 1.0);
//}

//TextureCube environmentMap : register(t0);
//SamplerState CubeSampler : register(s0);

//const static float PI = 3.14159265359;
//const static float GOLDEN_RATIO = (1.0 + sqrt(5.0)) / 2.0;

//struct Input
//{
//    float4 Pos : SV_POSITION;
//    float3 WorldPos : WORLD_POSITION;
//};

//float3 randomHemisphere(out float NdotV, float i, float N)
//{
//    float theta = 2.0 * PI * i / GOLDEN_RATIO;
//    float phiCos = NdotV = 1.0 - (i + 0.5) / N;
//    float phiSin = sqrt(1.0 - phiCos * phiCos);
//    float thetaCos, thetaSin;
//    sincos(theta, thetaSin, thetaCos);
//    return float3(thetaCos * phiSin, thetaSin * phiSin, phiCos);
//}

//float4 main(Input input) : SV_TARGET
//{
//    float3 N = normalize(input.WorldPos);

//    float3 irradiance = float3(0.0, 0.0, 0.0);
    
//    // tangent space calculation from origin point
//    float3 up = float3(0.0, 1.0, 0.0);
//    float3 right = normalize(cross(up, N));
//    up = normalize(cross(N, right));
       
//    float Nsamples = 1024.0; // Number of samples (e.g. 1024)
//    float NdotV;
//    for (float i = 0.0; i < Nsamples; i++)
//    {
//        // Use the Fibonacci sampling
//        float3 tangentSample = randomHemisphere(NdotV, i, Nsamples);

//        // tangent space to world
//        float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

//        float theta = acos(NdotV);
//        irradiance += environmentMap.Sample(CubeSampler, sampleVec).rgb * NdotV * sin(theta);
//    }
//    irradiance = PI * irradiance * (1.0 / Nsamples);
    
//    return float4(irradiance, 1.0);
//}