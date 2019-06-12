cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProj : packoffset(c4);
};


struct VS_INSTANCING_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float4x4 transform : WORLDMATRIX;
    float4 instanceColor : INSTANCECOLOR;
};

struct VS_INSTANCING_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_INSTANCING_OUTPUT VSInstancing(VS_INSTANCING_INPUT input)
{
    VS_INSTANCING_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), input.transform), gmtxView), gmtxProj);
    output.color = input.color + input.instanceColor;
    return output;
}

float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET
{
    return input.color;
}
