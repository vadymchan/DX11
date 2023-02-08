
cbuffer PerView : register(b0)
{
    //row_major float4x4 ViewMatrix;
    //row_major float4x4 PerspectiveMatrix;
    float4x4 VP;
};

struct VOut
{
    float4 position : SV_POSITION;
    //float4 color : COLOR;
    float4x4 instance : INSTANCE;
};


VOut main(float4 position : POSITION, /*float4 color : COLOR,*/ float4x4 instance : INSTANCE)
{
    VOut output;

    //output.position = mul(instance, mul(ViewMatrix, mul(PerspectiveMatrix, position)));
    output.position = mul(VP, position);
    //output.position = position;
    //output.color = color;
    output.instance = instance;
   
    return output;
}

