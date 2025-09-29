#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInv, g_ProjMatrixInv;

Texture2D g_Texture;
Texture2D g_DepthTexture;
TextureCube g_SkyTexture;
Texture2D g_NormalTexture;
Texture2D g_DistortionTexture;

float4 g_vCamPosition;
float g_fTimeAcc;
struct DEBUG_PARAMS
{
    float2 vTileSize;
    float2 vPanningSpeed;
    float fDistortionIntensity;
    float fLightPower;
    float fLightIntensity;
} g_DebugParams;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    Out.vTexCoord = In.vTexCoord;
    Out.vProjPos = Out.vPosition;
    Out.vNormal = float4(normalize(mul(float4(0.f, 0.f, -1.f, 0.f), g_WorldMatrix).xyz), 1.f);
    Out.vTangent = float4(normalize(mul(float4(1.f, 0.f, 0.f, 0.f), g_WorldMatrix).xyz), 1.f);
    Out.vBinormal = float4(normalize(cross(normalize(Out.vNormal.xyz), normalize(Out.vTangent.xyz))), 0.f);

    return Out;
}

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    float2 vDepthTexCoord;
    vDepthTexCoord.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vDepthTexCoord.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthTexCoord);

    float4 vSurfaceWorldPosition = TransformNDCToWorld(vDepthTexCoord, float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f), g_ProjMatrixInv, g_ViewMatrixInv);
    float4 vImageWorldPosition = TransformNDCToWorld(vDepthTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv);

    float3 vShallowColor = float3(0.5f, 0.8f, 1.f);
    float3 vDeepColor = float3(0.25f, 0.4f, 0.65f);

    float fDeep = vSurfaceWorldPosition.y - vImageWorldPosition.y;
    float3 vColor = lerp(vShallowColor, vDeepColor, saturate(fDeep * 0.2f));

    float2 vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexCoord).yz * g_DebugParams.fDistortionIntensity;

    float2 vTexCoord = In.vTexCoord * g_DebugParams.vTileSize + g_DebugParams.vPanningSpeed * g_fTimeAcc + vDistortion;
    float3 vNormalDesc = g_NormalTexture.Sample(LinearSampler, vTexCoord).xyz;

    float3x3 TBN = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    float3 vNormal = normalize(mul(vNormalDesc.xyz * 2.f - 1.f, TBN));
    //float3 vNormal = normalize(mul(float3(1.f, 1.f, 0.f), TBN));

    /* 물 깊이에 따라서 짙은색과 불투명 */
    Out.vColor = float4(vColor, saturate(saturate(fDeep * 0.25f) + 0.15f));

    /* 스카이 박스랑 반사 */
    //float F0 = 0.02f;
    float F0 = 0.f;
    float3 vView = normalize(g_vCamPosition.xyz - vSurfaceWorldPosition.xyz);
    float NoV = saturate(dot(vNormal, vView));
    float3 vReflectionDir = normalize(reflect(-vView, vNormal));

    float3 vSkyColor = saturate(g_SkyTexture.Sample(LinearSampler, vReflectionDir).xyz);

    float fFresnel = saturate(F0 + (1.0f - F0) * pow(1.0f - NoV, 5.0f));

    Out.vColor.xyz = lerp(Out.vColor.xyz, float3(vSkyColor), saturate(fFresnel));

    /* 얕은 수면인 경우 하얀색 */
    Out.vColor = lerp(Out.vColor, float4(1.f, 1.f, 1.f, 1.f), step(vDepthDesc.y - In.vProjPos.w, 0.05f));


    float3 vLightDir = normalize(float3(0.f, -2.f, 0.805f)) * -1.f;
    float3 vHalfDir = normalize(vLightDir + vView);

    float HoV = saturate(dot(vHalfDir, vNormal));

    Out.vColor.xyz += pow(HoV, g_DebugParams.fLightPower) * g_DebugParams.fLightIntensity;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Water
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_OnlyCompare, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}