#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
float4 g_vSrcColor = float4( 1.f, 0.f, 1.f, 1.f );
float4 g_vDstColor = float4(0.f, 1.f, 1.f, 1.f);

/* for.CLOCK */
float2 g_vBegin, g_vEnd;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    Out.vTexCoord = In.vTexCoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    //if (0.2f >= Out.vColor.a)
    //    discard;

    clip(Out.vColor.a - 0.2f);

    return Out;
}

PS_OUT PS_MAIN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vTexelColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    if (0.2f >= vTexelColor.a)
        discard;

    Out.vColor = vTexelColor * g_vSrcColor;

    return Out;
}

PS_OUT PS_MAIN_CLOCK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vTexelColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    if (0.2f >= vTexelColor.a)
        discard;


    // theata 를 선형 보간해서 원하는 방향 벡터를 찾고 그 다음에 그걸로 is between ? cpp 에서는 0 ~ 1 사이의 값을 던져주고?? 원하는 시작위치, 마지막 위치, 보간을 원하는 시작위치마지막 위치도 던져줘야하네?
    if (IsBetween(In.vTexCoord, g_vBegin, g_vEnd))
    //if (In.vTexCoord.x > 0.5f && In.vTexCoord.y < 0.5f)
    {
        Out.vColor = vTexelColor * g_vSrcColor;
    }
    else
    {
        Out.vColor = vTexelColor * g_vDstColor;
    }

    clip(Out.vColor.a - 0.1f);

    return Out;
}

PS_OUT PS_MAIN_FILTERBLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vTexelColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
    float4 vFilterColor = g_vSrcColor;

    vFilterColor *= (1.f - vTexelColor.a);

    Out.vColor = vTexelColor + g_vSrcColor;

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
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

    pass ColorBLENDPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

    pass ClockPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CLOCK();
    }

    pass FilterBlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FILTERBLEND();
    }
}