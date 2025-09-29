#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"


float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

Texture2D g_DepthTexture;

float4 g_vSrcColor;
float4 g_vDstColor;

/* 1. 정점과 인덱스 버퍼 + 셰이더 바인딩해서 그린다. */
/* 2. 인덱스버퍼에 저장되어있는 인덱스에 해당하는 정점을 하나 꺼내서, */
/* 3. 정점쉐이더함수를 호출하며 인자로 정점(로컬스페이스기준)을 전달해준다. */
/* 4. 정점 쉐이더에서 정점에 필요한 연산을 수행한다.(ex> 정점의 로컬위치에다가 월드, 뷰, 투영행려을 곱한다. ) */

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

    //Out.vPosition = float4(In.vPosition, 1.f);
	
    Out.vTexCoord = In.vTexCoord;

    return Out;
}

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;

    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    Out.vTexCoord = In.vTexCoord;

    Out.vProjPos = Out.vPosition;

    return Out;
}

/* 5. 정점쉐이더가 끝나게 되면 그리는 형태에따라 추가적인 정점의 결과를 기다리낟. */
/* 5-1. D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST 였다면 추가로 두개의 정점이 정점쉐이더를 통과할때 까지 기다리낟. */
/* 6. 내가 그릴려고하는 도형의 형태에 맞게끔 정점이 모두 리턴된다면 각 정점에게 w나누기연산(원근투영)을 수행한다. 투영스페이스로 갔다. */
/* 7. 뷰포트로 정점의 위치를 변환하게된다. 윈도우 좌표로 정점들을 모두 다 옮겨낸다. */
/* 8. 래스터라이즈를 수행하게된다. (래스터라이즈 : 정점들의 정보를 기반으로 하여 픽셀의 정보를 생성한다) => 픽셀을 만들었다. */
/* 9. 만들어낸 픽셀을 PS_MAIN함수를 호출하면서 인자로 전달해 준다. */
/* 10. 픽셀 쉐이더에서 전달받은 픽셀의 정보를 기반으로하여 최종적인 픽셀의 색을 결정하여 리턴하게되고 */
/* 11. 리턴된 최종적인 색으로 백버퍼를 채우게 된다. */

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

    if (0.2f >= Out.vColor.a)
        discard;

    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SOFTPARTICLE(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    float2 vDepthTexCoord;
    vDepthTexCoord.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vDepthTexCoord.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;

    //float2 vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthTexCoord).xy;
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthTexCoord);

    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w); // 좀 더 구체적인 값 결정 방법이 있을거야
    return Out;
}

PS_OUT PS_MAIN_INTERSECTIONHIGHLIGHT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    float2 vDepthTexCoord;
    vDepthTexCoord.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vDepthTexCoord.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;

    //float2 vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthTexCoord).xy;
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthTexCoord);

    float fValue = saturate((vDepthDesc.y - In.vProjPos.w) * 5.f);
    //Out.vColor.a = max(0.1f, 1.f - fValue);
    Out.vColor.a = max(g_vSrcColor.a, 1.f - fValue);
    Out.vColor.b = 1.f;
    Out.vColor.rg = min(0.5f, 1.f - fValue);
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

    pass SoftParticle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFTPARTICLE();
    }

    pass IntersectionHighlihgt
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INTERSECTIONHIGHLIGHT();
    }

    pass SkyPass
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}