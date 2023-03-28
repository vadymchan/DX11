
struct Input
{
    float4 position : SV_POSITION;
};


float3 main(Input input) : SV_TARGET
{

    return float3(1, 1, 0);
    
}