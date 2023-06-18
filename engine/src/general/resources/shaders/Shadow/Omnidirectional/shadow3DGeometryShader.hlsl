
struct GS_INPUT
{
    float4 Pos : SV_POSITION;

};


struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;

};

#define MAX_OUTPUT_TRIANGLES 6
#define MAX_OUTPUT_VERTICES 3 * MAX_OUTPUT_TRIANGLES


cbuffer DepthCubemapMatrices : register(b0)
{
    float4x4 ViewMatrices[6];
    float4x4 ProjectionMatrix;
}

[maxvertexcount(MAX_OUTPUT_VERTICES)]
void main(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> OutputStream)
{
    for (uint face = 0; face < 6; ++face)
    {
        for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output;

            float4 viewPos = mul(input[i].Pos, ViewMatrices[face]);
            output.Pos = mul(viewPos, ProjectionMatrix);

            output.RTIndex = face;

            OutputStream.Append(output);
        }

        OutputStream.RestartStrip();
    }
}
