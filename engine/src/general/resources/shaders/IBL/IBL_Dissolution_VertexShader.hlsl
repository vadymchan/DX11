
cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

cbuffer PerMesh : register(b2)
{
    float4x4 meshToModel;
}

struct Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float2 texCoord : TEXCOORD0;
    float4x4 instance : INSTANCE;
    float4x4 invInstance : INVINSTANCE;
    float time : TIME;
};

struct Output
{
    float4 Position : SV_POSITION;
    float4 ViewPosition : ViewPosition;
    float4 WorldPosition : WorldPosition;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float4x4 View : VIEW;
    float time : TIME;
    
};


Output main(Input input)
{
    Output output;

    output.WorldPosition = mul(mul(
        float4(input.position, 1),
        meshToModel),
        input.instance);
    
    output.ViewPosition = mul(output.WorldPosition, View);
    
    output.Position = mul(output.ViewPosition, Proj);

    output.Normal = input.normal;
    output.Normal = mul(output.Normal, (float3x3) meshToModel);
    output.Normal = mul(output.Normal, transpose((float3x3) input.invInstance));
    //output.Normal = mul(output.Normal, (float3x3) View);
    output.Normal = (output.Normal != float3(0, 0, 0)) ? normalize(output.Normal) : output.Normal;
    
    output.Tangent = input.Tangent;
    output.Tangent = mul(output.Tangent, (float3x3) meshToModel);
    output.Tangent = mul(output.Tangent, transpose((float3x3) input.invInstance));
    //output.Tangent = mul(output.Tangent, (float3x3) View);
    output.Tangent = (output.Tangent != float3(0, 0, 0)) ? normalize(output.Tangent) : output.Tangent;
    
    output.Bitangent = input.Bitangent;
    output.Bitangent = mul(output.Bitangent, (float3x3) meshToModel);
    output.Bitangent = mul(output.Bitangent, transpose((float3x3) input.invInstance));
    //output.Bitangent = mul(output.Bitangent, (float3x3) View);
    output.Bitangent = (output.Bitangent != float3(0, 0, 0)) ? normalize(output.Bitangent) : output.Bitangent;
    
    output.UV = input.texCoord;
    
    output.View = View;
 
    output.time = input.time;
    
    return output;
}

