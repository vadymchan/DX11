struct GS_IN
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
};

struct GS_OUT
{
    float4 position : SV_Position;
};

[maxvertexcount(6)]
void main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> output)
{
    
    GS_OUT temp;
    //original triangle
    for (int i = 0; i < 3; ++i)
    {
        
        output.Append((GS_OUT)input[i].position);
    }
    
    float4 midpoint = (input[0].position + input[1].position + input[2].position) / 3.0f;

    //Compute average normal and scale it by desired length
    float4 normal = normalize(float4(normalize(input[0].normal), 0.0f)) ;
    
    
    output.Append((GS_OUT) midpoint);
    output.Append((GS_OUT) (midpoint + normal));
    //output.Append((GS_OUT) (midpoint + normal));
    

}