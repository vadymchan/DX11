

struct PatchConstants
{
    float EdgeFactors[3] : SV_TessFactor;
    float InsideFactors[1] : SV_InsideTessFactor;
};

struct DomainInput
{
    float4 positionModel : POSITIONMODEL;
    float4 positionWorld : SV_POSITION;
    float4 normal : NORMAL;
    
};

struct DomainOut
{
    float4 positionModel : POSITIONMODEL;
    float4 positionWorld : SV_POSITION;
    float4 normal : NORMAL;
    
};



[domain("tri")]
DomainOut main(PatchConstants patchConstants, float3 barycentrics : SV_DomainLocation,
               const OutputPatch<DomainInput, 3> input)
{
    DomainOut result;
    // manual barycentric interpolation
    result.positionWorld = barycentrics.x * input[0].positionWorld +
                      barycentrics.y * input[1].positionWorld +
                      barycentrics.z * input[2].positionWorld;
    
    result.positionModel = barycentrics.x * input[0].positionModel +
                      barycentrics.y * input[1].positionModel +
                      barycentrics.z * input[2].positionModel;
    
    float3 normal = input[0].normal * barycentrics.x + input[1].normal * barycentrics.y + input[2].normal * barycentrics.z;
    result.normal = float4(normalize(normal),0);
    return result;
}
