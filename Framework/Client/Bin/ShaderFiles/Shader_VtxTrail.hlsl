
#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;
Texture2D g_ColorTexture;

float3 g_vColor;

float4 g_vCamPosition;
float g_fWidth;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vLifeTime : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
   
    Out.vPosition = float4(In.vPosition, 1.f);
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}
    
struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vLifeTime : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fLifeLerp : TEXCOORD1;
};

/* Geometry Shader == ±‚«œºŒ¿Ã¥ı */
/* */
[maxvertexcount(8)]
void GS_MAIN(line GS_IN In[2], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[8];

    float fLifeTimeLerp = 0.f;
    if (In[0].vLifeTime.x != 0.f)
        In[0].vLifeTime.y / In[0].vLifeTime.x;    
    
    float3 vPosSrc = In[0].vPosition;
    float3 vPosDst = In[1].vPosition;
    float3 vCenter = (vPosSrc + vPosDst) * 0.5f;

    float3 vLook = normalize(g_vCamPosition.xyz - vCenter.xyz);
    float3 vTrailDir = normalize(vPosDst - vPosSrc);
    float3 vUp = float3(0.f, 1.f, 0.f);
    float3 vRight = normalize(cross(vTrailDir, vUp)) * g_fWidth;
    vUp = normalize(cross(vRight, vTrailDir)) * g_fWidth;
    // float3 vRight = normalize(cross(vTrailDir, vLook)) * g_fWidth;
    
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
   
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].fLifeLerp = fLifeTimeLerp;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(0.f, 1.f);
    Out[1].fLifeLerp = fLifeTimeLerp;

    Out[2].vPosition = float4(In[1].vPosition.xyz + vRight, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 0.f);
    Out[2].fLifeLerp = fLifeTimeLerp;

    Out[3].vPosition = float4(In[1].vPosition.xyz - vRight, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(1.f, 1.f);
    Out[3].fLifeLerp = fLifeTimeLerp;
   
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
    DataStream.RestartStrip();
    
    Out[4].vPosition = float4(In[0].vPosition.xyz + vUp, 1.f);
    Out[4].vPosition = mul(Out[4].vPosition, matVP);
    Out[4].vTexcoord = float2(0.f, 0.f);
    Out[4].fLifeLerp = fLifeTimeLerp;

    Out[5].vPosition = float4(In[0].vPosition.xyz - vUp, 1.f);
    Out[5].vPosition = mul(Out[5].vPosition, matVP);
    Out[5].vTexcoord = float2(0.f, 1.f);
    Out[5].fLifeLerp = fLifeTimeLerp;

    Out[6].vPosition = float4(In[1].vPosition.xyz + vUp, 1.f);
    Out[6].vPosition = mul(Out[6].vPosition, matVP);
    Out[6].vTexcoord = float2(1.f, 0.f);
    Out[6].fLifeLerp = fLifeTimeLerp;

    Out[7].vPosition = float4(In[1].vPosition.xyz - vUp, 1.f);
    Out[7].vPosition = mul(Out[7].vPosition, matVP);
    Out[7].vTexcoord = float2(1.f, 1.f);
    Out[7].fLifeLerp = fLifeTimeLerp;
   
    DataStream.Append(Out[4]);
    DataStream.Append(Out[5]);
    DataStream.Append(Out[6]);
    DataStream.Append(Out[7]);
    DataStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fLifeLerp : TEXCOORD1;
};

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    if (In.fLifeLerp > 1.f)
        discard;
    
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float2 vColorTexCoord = float2(In.fLifeLerp, In.fLifeLerp);
    float4 vColorDesc = g_ColorTexture.Sample(LinearSampler, vColorTexCoord);
    
    Out.vDiffuse.rgb += vColorDesc.rgb;
    Out.vDiffuse.a *= 1.f - pow(In.fLifeLerp, 2.f);
    
    if(Out.vDiffuse.a <= 0.2f)
        discard;
    
    
    return Out;
}

PS_OUT PS_MAIN_COLORED(PS_IN In)
{
    //if (In.fLifeLerp > 1.f)
    //    discard;
    
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse.rgb = g_vColor;
    // Out.vDiffuse.a *= 1.f - pow(In.fLifeLerp, 2.f);
    
    //if (Out.vDiffuse.a <= 0.2f)
    //    discard;
    Out.vDiffuse.a = 1.f;
    
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ColoredPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_COLORED();
    }
}
