
struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};


Output main(uint vertexID : SV_VertexID)
{
    Output output;

    
    float posX = float(vertexID & 1) * 2 - 1;
    float posY = float(((vertexID >> 1) & 1)) * -2 + 1;

    
    output.position = float4(posX, posY, 0, 1);

    
    float texX = ((int) posX + 1) >> 1;
    float texY = (((int) posY + 1) ^ 2) >> 1;
    
    output.texcoord = float2(texX, texY);
    
    return output;
}