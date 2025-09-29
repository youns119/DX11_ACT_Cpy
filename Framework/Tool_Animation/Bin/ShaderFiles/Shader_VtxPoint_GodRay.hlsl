
#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 g_vCamPosition;
float3 g_vMulColor;
float3 g_vAddColor;

bool g_bIsLoop;

Texture2D g_DepthTexture;

float BOITWeight(in float z, in float zw, in float a)
{
    return pow(a, 4.f) / ((a + 0.0001f) + (z + 0.0001f) * 0.01f);
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float fPSize : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
    float fSpeed : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float fPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
   
   /* 로컬스페이스내에서 움직임을 가져간다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fPSize = length(In.TransformMatrix._11_12_13);
    Out.vLifeTime = In.vLifeTime;
    Out.vRight = In.TransformMatrix._11_12_13_14;
    Out.vUp = In.TransformMatrix._21_22_23_24;
    return Out;
}
    
struct GS_IN
{
    float4 vPosition : POSITION;
    float fPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float fLifeLerp : TEXCOORD3;
};

/* Geometry Shader == 기하셰이더 */
/* */
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float fLifeTimeLerp = In[0].vLifeTime.y / In[0].vLifeTime.x;
    float fSizeRight = length(In[0].vRight);
    float fSizeUp = length(In[0].vUp);
    float3 vCamLook = normalize(g_vCamPosition.xyz - In[0].vPosition.xyz);
    float3 vUp = float3(-0.25f, 1.f, -0.25f);
    float3 vRight = normalize(cross(vUp, vCamLook));
    float3 vLook = normalize(cross(vRight, vUp));
    
    vRight *= fSizeRight;
    vUp *= fSizeUp;
    
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
   
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vProjPos = Out[0].vPosition;
    Out[0].fLifeLerp = fLifeTimeLerp;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vProjPos = Out[1].vPosition;
    Out[1].fLifeLerp = fLifeTimeLerp;

    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vProjPos = Out[2].vPosition;
    Out[2].fLifeLerp = fLifeTimeLerp;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = Out[3].vPosition;
    Out[3].fLifeLerp = fLifeTimeLerp;
   
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
    DataStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float fLifeLerp : TEXCOORD3;
};

struct PS_OUT_BLENDACC
{
    float4 vDiffuseAcc : SV_Target0;
};

struct PS_OUT_REVEAL
{
    float4 vRevealage : SV_TARGET0;
};

PS_OUT_BLENDACC PS_MAIN_ACC(PS_IN In)
{
    PS_OUT_BLENDACC Out = (PS_OUT_BLENDACC) 0;
    
    Out.vDiffuseAcc = float4(g_vAddColor.rgb, 1.f);
    
    float fEdgeX = 0.25;
    float fEdgeY = 0.1;
    float fFadeX = 1.f;
    float fFadeY = 1.f;

    if (In.vTexcoord.x < fEdgeX)
        fFadeX = smoothstep(0.f, fEdgeX, In.vTexcoord.x);
    else if (In.vTexcoord.x > (1 - fEdgeX))
        fFadeX = 1.f - smoothstep(1.f - fEdgeX, 1.f, In.vTexcoord.x);
    if (In.vTexcoord.y < fEdgeY)
        fFadeY = smoothstep(0.f, fEdgeY, In.vTexcoord.y);
    else if (In.vTexcoord.y > (1.f - fEdgeY))
        fFadeY = 1.f - smoothstep(1.f - fEdgeY, 1.f, In.vTexcoord.y);
    Out.vDiffuseAcc.a *= fFadeX * fFadeY;
    
    float fLifeAlpha = 0.f;
    if (In.fLifeLerp < 0.5f)
        fLifeAlpha = saturate(In.fLifeLerp * 4.f);
    else if (In.fLifeLerp == 0.5f)
        fLifeAlpha = 1.f;
    else
        fLifeAlpha = 1.f - pow((In.fLifeLerp - 0.5f) * 2.f, 2.f);
    Out.vDiffuseAcc.a *= saturate(fLifeAlpha);
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    float fWeight = BOITWeight(In.vProjPos.z, In.vProjPos.z / In.vProjPos.w, Out.vDiffuseAcc.a) * In.vTexcoord.y;
    Out.vDiffuseAcc = float4(Out.vDiffuseAcc.r * fWeight, Out.vDiffuseAcc.g * fWeight, Out.vDiffuseAcc.b * fWeight, Out.vDiffuseAcc.a * fWeight);
    return Out;
}

PS_OUT_REVEAL PS_MAIN_REVEALAGE(PS_IN In)
{
    PS_OUT_REVEAL Out = (PS_OUT_REVEAL) 0;
    
    float4 vDiffuseAcc = float4(g_vAddColor.rgb, 1.f);
    
    float fEdgeX = 0.25;
    float fEdgeY = 0.15;
    float fFadeX = 1.f;
    float fFadeY = 1.f;

    if (In.vTexcoord.x < fEdgeX)
        fFadeX = smoothstep(0.f, fEdgeX, In.vTexcoord.x);
    else if (In.vTexcoord.x > (1 - fEdgeX))
        fFadeX = 1.f - smoothstep(1.f - fEdgeX, 1.f, In.vTexcoord.x);
    
    if (In.vTexcoord.y < fEdgeY)
        fFadeY = smoothstep(0.f, fEdgeY, In.vTexcoord.y);
    else if (In.vTexcoord.y > (1.f - fEdgeY))
        fFadeY = 1.f - smoothstep(1.f - fEdgeY, 1.f, In.vTexcoord.y);
    
    vDiffuseAcc.a *= fFadeX * fFadeY;
    
    float fLifeAlpha = 0.f;
    if (In.fLifeLerp < 0.5f)
        fLifeAlpha = saturate(In.fLifeLerp * 4.f);
    else if (In.fLifeLerp == 0.5f)
        fLifeAlpha = 1.f;
    else
        fLifeAlpha = 1.f - pow((In.fLifeLerp - 0.5f) * 2.f, 2.f);
    vDiffuseAcc.a *= saturate(fLifeAlpha);
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    float fWeight = BOITWeight(In.vProjPos.z, In.vProjPos.z / In.vProjPos.w, vDiffuseAcc.a);
    Out.vRevealage.r = vDiffuseAcc.a * fWeight * In.vTexcoord.y;
    return Out;
}

technique11 DefaultTechnique
{
    pass BlendAccPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_BlendAcc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_ACC();
    }

    pass RevealagePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_Revealage, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_REVEALAGE();
    }
}
