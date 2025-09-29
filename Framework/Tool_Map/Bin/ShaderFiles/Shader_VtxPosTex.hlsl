#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"


float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

Texture2D g_DepthTexture;

float4 g_vSrcColor;
float4 g_vDstColor;

/* 1. ������ �ε��� ���� + ���̴� ���ε��ؼ� �׸���. */
/* 2. �ε������ۿ� ����Ǿ��ִ� �ε����� �ش��ϴ� ������ �ϳ� ������, */
/* 3. �������̴��Լ��� ȣ���ϸ� ���ڷ� ����(���ý����̽�����)�� �������ش�. */
/* 4. ���� ���̴����� ������ �ʿ��� ������ �����Ѵ�.(ex> ������ ������ġ���ٰ� ����, ��, ��������� ���Ѵ�. ) */

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

/* 5. �������̴��� ������ �Ǹ� �׸��� ���¿����� �߰����� ������ ����� ��ٸ���. */
/* 5-1. D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST ���ٸ� �߰��� �ΰ��� ������ �������̴��� ����Ҷ� ���� ��ٸ���. */
/* 6. ���� �׸������ϴ� ������ ���¿� �°Բ� ������ ��� ���ϵȴٸ� �� �������� w�����⿬��(��������)�� �����Ѵ�. ���������̽��� ����. */
/* 7. ����Ʈ�� ������ ��ġ�� ��ȯ�ϰԵȴ�. ������ ��ǥ�� �������� ��� �� �Űܳ���. */
/* 8. �����Ͷ���� �����ϰԵȴ�. (�����Ͷ����� : �������� ������ ������� �Ͽ� �ȼ��� ������ �����Ѵ�) => �ȼ��� �������. */
/* 9. ���� �ȼ��� PS_MAIN�Լ��� ȣ���ϸ鼭 ���ڷ� ������ �ش�. */
/* 10. �ȼ� ���̴����� ���޹��� �ȼ��� ������ ��������Ͽ� �������� �ȼ��� ���� �����Ͽ� �����ϰԵǰ� */
/* 11. ���ϵ� �������� ������ ����۸� ä��� �ȴ�. */

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

    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w); // �� �� ��ü���� �� ���� ����� �����ž�
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