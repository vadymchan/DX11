
struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};


Output main(uint vertexID : SV_VertexID)
{
    Output output;
    
    
    switch (vertexID)
    {
        case 0:
            output.position = float4(-1, 1, 0, 1);
            output.texcoord = float2(0, 0);
            break;
        case 1:
            output.position = float4(1, 1, 0, 1);
            output.texcoord = float2(1, 0);
            break;
        case 2:
            output.position = float4(-1, -1, 0, 1);
            output.texcoord = float2(0, 1);
            break;
        case 3:
            output.position = float4(1, -1, 0, 1);
            output.texcoord = float2(1, 1);
            break;
    }
    
    //float posX = float(((vertexID >> 1) & 1) * 2) - 1;
    //float posY = 1 - float(vertexID & 1) * 2;
    
    //output.position = float4(posX, posY, 0, 1);
    //output.texcoord = float2(((int) posX + 1) >> 1, float((((int) posY + 1) ^ 2) >> 1));
    
    //output.position = float4(posX, posY, 1, 1);
    
    //float texX = ((int) posX + 1) >> 1;
    //float texY = (((int) posY + 1) ^ 2) >> 1;
    
    //output.texcoord = float2(texX, texY);
    
    return output;
}