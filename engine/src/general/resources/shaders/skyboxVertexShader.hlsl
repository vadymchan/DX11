
cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

cbuffer InvView : register(b1)
{
    float4x4 invView;
    float4x4 invProj;
}

struct Input
{
    uint vertexID : SV_VertexID;
    //float3 position : POSITION0;
    //float2 tex : TEXCOORD0;
};

struct Output
{
    float3 direction : DIRECTION0;
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;
    float3 campos : CAMPOS;
    
};

Output main(Input input)
{
    Output output;

    //float4 pos = float4(input.position, 1.0f);
    float x = float((input.vertexID & 2) * 2) - 1;
    float y = float((input.vertexID & 1) * 4) - 1;
    output.position = float4(x, y, 0, 1);

    //output.position = float4(x * 2 - 1 + offsetX, y * 2 - 1 + offsetY, 0, 1);

    //output.tex.x = float(input.vertexID & 2);
    //output.tex.y = float((input.vertexID & 1) << 1);
    
    
    output.tex = float3(output.position.x, output.position.y, 1);
    //if (input.vertexID == 0)
    //{
    //    output.tex = float2(0, 0);

    //}
    //else if (input.vertexID == 1)
    //{
    //    output.tex = float2(0, 3);
        
    //}
    //else
    //{
    //    output.tex = float2(3, 0);
        
    //}
    output.tex = mul(output.tex, (float3x3)invView);
        output.campos = mul(float4(0, 0, 0, 1), View).xyz;
    //pos = mul(mul(pos, View), Proj);
    //pos.z = pos.w;
    
    //output.direction = pos.xyz;
    //output.direction = float3(input.position.xy, 0.5) * 0.5 + 0.5;;
    
    //output.position = float4(input.position, 1);
    
    //output.tex = input.tex;

    return output;
}
