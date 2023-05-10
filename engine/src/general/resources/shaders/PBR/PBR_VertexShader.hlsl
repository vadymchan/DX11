
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

    output.Normal = normalize(input.normal);
    output.Normal = mul(output.Normal, (float3x3) meshToModel);
    output.Normal = mul(output.Normal, transpose((float3x3) input.invInstance));
    //output.Normal = mul(output.Normal, (float3x3) View);
    output.Normal = normalize(output.Normal);
    
    output.Tangent = normalize(input.Tangent);
    output.Tangent = mul(output.Tangent, (float3x3) meshToModel);
    output.Tangent = mul(output.Tangent, transpose((float3x3) input.invInstance));
    //output.Tangent = mul(output.Tangent, (float3x3) View);
    output.Tangent = normalize(output.Tangent);
    
    output.Bitangent = normalize(input.Bitangent);
    output.Bitangent = mul(output.Bitangent, (float3x3) meshToModel);
    output.Bitangent = mul(output.Bitangent, transpose((float3x3) input.invInstance));
    //output.Bitangent = mul(output.Bitangent, (float3x3) View);
    output.Bitangent = normalize(output.Bitangent);
    
    output.UV = input.texCoord;
    
    output.View = View;
 
    return output;
}

