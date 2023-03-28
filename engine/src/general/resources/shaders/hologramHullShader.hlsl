struct PatchOut 
{
    float EdgeFactors[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};

struct HullInput
{
    float4 positionModel : POSITIONMODEL;
    float4 positionWorld : SV_POSITION;
    float4 normal : NORMAL;
    
};

struct HullOutput
{
    float4 positionModel : POSITIONMODEL;
    float4 positionWorld : SV_POSITION;

    float4 normal : NORMAL;
    
};

[patchconstantfunc]
PatchOut mainPatch(InputPatch<HullInput, 3> input, uint patchId : SV_PrimitiveID)
{
    PatchOut output;
    
    output.EdgeFactors[0] = length(input[1].positionWorld - input[0].positionWorld);
    output.EdgeFactors[1] = length(input[2].positionWorld - input[0].positionWorld);
    output.EdgeFactors[2] = length(input[2].positionWorld - input[1].positionWorld);
    output.InsideFactor = max(output.EdgeFactors[0], max(output.EdgeFactors[1], output.EdgeFactors[2]));
    return output;
}


[outputcontrolpoints(3)] 
[domain("tri")]
[outputtopology("triangle_cw")]
[partitioning("integer")]
[patchconstantfunc("mainPatch")]
HullOutput main(InputPatch<HullInput, 3> input,
 uint pointId : SV_OutputControlPointID,
 uint patchId : SV_PrimitiveID)
{
    return input[pointId];
}