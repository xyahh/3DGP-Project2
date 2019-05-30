//void VSMain(float3 iPos : POSITION, float4 iColor : COLOR,
//            out float4 oPos : SV_POSITION, out float4 oColor : COLOR)
//{
//    oPos = float4(iPos, 0.f);
//    oColor = iColor;
//}

void VSMain(uint uVertexID : SV_VertexID, out float4 oPos : SV_POSITION)
{
    if (uVertexID == 0)
        oPos = float4(0.f, 0.5f, 0.5f, 1.f);
    else if (uVertexID == 1)
        oPos = float4(-0.5f, -0.5f, 0.5f, 1.f);
    else if (uVertexID == 2)
        oPos = float4(0.5f, -0.5f, 0.5f, 1.f);
}



void PSMain(float4 iPos : SV_POSITION, out float4 oColor : SV_TARGET)
{
    oColor = float4(1.f, 1.f, 0.f, 1.f);;
}