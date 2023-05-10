struct Input
{
    float4 viewPos : VIEWPOS;
    float4 screenPos : SV_POSITION;
    float3 normal : NORMAL;
    float3 emission : EMISSION;

};

float3 main(Input input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 cameraDir = normalize(-input.viewPos.xyz);
    float3 normedEmission = input.emission / max(input.emission.x, max(input.emission.y, max(input.emission.z, 1.0)));
    float NoV = dot(cameraDir, normal);
    return lerp(normedEmission * 0.33, normedEmission, pow(max(0.0, NoV), 8.0));

}
