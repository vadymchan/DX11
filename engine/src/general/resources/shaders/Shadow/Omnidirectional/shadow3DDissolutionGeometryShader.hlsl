struct Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float time : TIME;
};

struct Output
{
    float4 position : SV_Position;
    uint RTIndex : SV_RenderTargetArrayIndex;
    float2 uv : TEXCOORD0;
    float time : TIME;
};




#define MAX_OUTPUT_TRIANGLES 6
#define MAX_OUTPUT_VERTICES 3 * MAX_OUTPUT_TRIANGLES


cbuffer DepthCubemapMatrices : register(b0)
{
    float4x4 ViewMatrices[6];
    float4x4 ProjectionMatrix;
}

[maxvertexcount(MAX_OUTPUT_VERTICES)]
void main(triangle Input input[3], inout TriangleStream<Output> OutputStream)
{
    for (uint face = 0; face < 6; ++face)
    {
        for (uint i = 0; i < 3; ++i)
        {
            Output output;

            float4 viewPos = mul(input[i].position, ViewMatrices[face]);
            output.position = mul(viewPos, ProjectionMatrix);
            
            output.uv = input[i].uv;
            output.time = input[i].time;
            
            output.RTIndex = face;

            OutputStream.Append(output);
        }

        OutputStream.RestartStrip();
    }
}
