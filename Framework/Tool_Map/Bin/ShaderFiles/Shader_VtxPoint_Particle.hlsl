
#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float fPSize : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float fPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

    float4x4 matWV = mul(g_WorldMatrix, g_ViewMatrix);

    Out.vPosition = mul(vPosition, matWV);
    Out.fPSize = length(In.TransformMatrix._11_12_13);
    Out.vLifeTime = In.vLifeTime;

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float fPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};


[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vRight = float3(1.f, 0.f, 0.f) * In[0].fPSize * 0.5f;
    float3 vUp = float3(0.f, 1.f, 0.f) * In[0].fPSize * 0.5f;

    Out[0].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), g_ProjMatrix);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;

    Out[1].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), g_ProjMatrix);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;

    Out[2].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), g_ProjMatrix);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;

    Out[3].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), g_ProjMatrix);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

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
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (0.2f >= Out.vColor.a)
        discard;

    //Out.vColor = float4((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.y, 0.f, 0.f, 1.f);


    //if (In.vLifeTime.y >= In.vLifeTime.x)
    //    discard;

    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
