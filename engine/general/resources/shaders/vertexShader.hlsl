
cbuffer PerView : register(b0)
{
    //row_major float4x4 ViewMatrix;
    //row_major float4x4 PerspectiveMatrix;
    float4x4 VP;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4x4 instance : INSTANCE;
};


VOut main(float4 position : POSITION, /*float4 color : COLOR,*/ float4x4 instance : INSTANCE)
{
    VOut output;


    output.position = mul(mul(position, instance), VP);
    output.color = position;
    output.instance = instance;
   
    return output;
}

