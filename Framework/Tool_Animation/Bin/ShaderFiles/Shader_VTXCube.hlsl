#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
TextureCube g_Texture;

Texture2D g_Texture2D;
Texture2D g_Texture2D_Distortion;

Texture2D g_DepthTexture;
Texture2D g_NormalTexture;
Texture2D g_ShadowMaskTexture;

float4x4 g_WorldMatrixInv;
float4x4 g_ProjMatrixInv;
float4x4 g_ViewMatrixInv;

struct CAUSTICS_PARAMS
{
    float2 vSpeed;
    float fTime;
    float fDistortionFactor;
    float fIntensity;
    
} g_CausticsParams;

struct LIGHT_PARAMS
{
    float3 vDir;
} g_LightParams;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexCoord : TEXCOORD0;
};

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float3 vTexCoord : TEXCOORD0;
    float4 vProjPos : POSITION0;
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

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;

    float4x4 matWV;
    float4x4 matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexCoord = In.vTexCoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vTexCoord : TEXCOORD0;
};

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float3 vTexCoord : TEXCOORD0;
    float4 vProjPos : POSITION0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    return Out;
}

PS_OUT PS_MAIN_SCREENSPACE_DECAL_CAUSTICS(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vScreenTexCoord;
    vScreenTexCoord.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vScreenTexCoord.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, vScreenTexCoord);
    float3 vWorldPos = TransformNDCToWorld(vScreenTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv).xyz;
    float4 vLocalPos = mul(float4(vWorldPos, 1.f), g_WorldMatrixInv);

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, vScreenTexCoord);
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);

    float3 vLightDir = -1.f * normalize(g_LightParams.vDir);
    float NoL = dot(vNormal, vLightDir);

    /* vtx texCUBE 는 로컬 범위가 -1 ~ 1 이다.*/
    //clip(1.f - abs(vLocalPos.xz));
    clip(abs(vLocalPos.xz - 1.f));
    clip(NoL);

    /* LightView3x3 */
    float3 vLightLook = -vLightDir;
    float3 vLightRight = normalize(cross(float3(0.f, 1.f, 0.f), vLightLook));
    float3 vLightUp = cross(vLightLook, vLightRight);

    float3x3 vLightViewMatrix = transpose(float3x3(vLightRight, vLightUp, vLightLook));
    
    float2 vProjectedTexCoord = mul(vWorldPos, vLightViewMatrix).xy;

    //float2 vTexCoord_Distortion = vProjectedTexCoord / 6.f + (g_PanningParams.fTime * float2(-0.15f, 0.3f));
    float2 vTexCoord_Distortion = vProjectedTexCoord / 16.f;
    float2 vDistortedUV = g_Texture2D_Distortion.Sample(LinearSampler, vTexCoord_Distortion).gb;
    vDistortedUV *= g_CausticsParams.fDistortionFactor/* 0.05f */;

    /* first */
    float2 vTexCoord = vProjectedTexCoord / 35.f + (g_CausticsParams.fTime * g_CausticsParams.vSpeed);
    vTexCoord += vDistortedUV;

    float4 vColor = g_Texture2D.Sample(LinearSampler, vTexCoord) * 0.5f;

    /* second */
    vTexCoord = vProjectedTexCoord / 45.f + (g_CausticsParams.fTime * g_CausticsParams.vSpeed * float2(0.5f, 0.8f));
    vTexCoord += vDistortedUV;

    vColor += g_Texture2D.Sample(LinearSampler, vTexCoord) * 0.5f;

    /* shadow mask */
    vColor = lerp(0.f, vColor, g_ShadowMaskTexture.Sample(LinearSampler, vScreenTexCoord).r);

    /* slope */
    vColor *= pow(NoL, 4.f) * 1.2;

    Out.vColor = vColor.gggg * g_CausticsParams.fIntensity;

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ScreenSpaceDecalCaustics
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCREENSPACE_DECAL_CAUSTICS();
    }
}