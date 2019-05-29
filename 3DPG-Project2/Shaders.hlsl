struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT VSMain(float3 position : POSITION, float4 color : COLOR)
{
    VS_OUTPUT output;
    output.position = float4(position, 1.f);
    output.color = color;
    return output;
}

float4 PSMain(VS_OUTPUT input): SV_TARGET
{
    return input.color;
}