struct GS_IN
{
    float4 position : SV_Position;
    float4 normal : NORMAL;
};

struct GS_OUT
{
    float4 position : SV_Position;
};

[maxvertexcount(6)]
void main(triangle GS_IN input[3], inout LineStream<GS_OUT> output)
{
   
    
    output.Append((GS_OUT) input[0].position);
    output.Append((GS_OUT) float4(input[0].position + input[0].normal));
    output.RestartStrip();
    
    
    output.Append((GS_OUT) input[1].position);
    output.Append((GS_OUT) (float4(input[1].position + input[1].normal)));
    output.RestartStrip();
    
    
    output.Append((GS_OUT) input[2].position);
    output.Append((GS_OUT) (float4(input[2].position + input[2].normal)));
    output.RestartStrip();
    
}