#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2DArray g_ShadowTexture;
Texture2D g_NormalTexture;
Texture2D g_DepthTexture;
Texture3D g_NoiseTexture;

float4x4 g_ProjMatrixInv;
float4x4 g_ViewMatrixInv;

float4x4 g_LightViewMatrices[3];
float4x4 g_LightProjMatrices[3];

float4 g_CircumSphere[3];

//float4 g_NoiseFilter[12][12][16];

struct SHADOWMASK_PARAMS
{
    float4 vShadowTexelSize;
    float4 vNoiseTexelSize;
    float fNumFilters;
    float fRadius;
} g_ShadowMaskParamas;

SamplerComparisonState LinearCmpBorder;

uint Compute_CascadeIndex(float3 vPixelWorldPos)
{
    uint iIndex = 0;
    [unroll]
    for (uint i = 0; i < 3; ++i)
    {
        iIndex = i;
        float3 vPixelShadowViewPos = mul(float4(vPixelWorldPos, 1.f), g_LightViewMatrices[i]).xyz;
        vPixelShadowViewPos.z -= 500.f; /* half Far -> 내가 임의로 캐스케이드 그리기 위해서 far 의 절반위로 올렷음 */

        float3 vResult = step(abs(vPixelShadowViewPos), g_CircumSphere[i].w);

        if (all(vResult))
            return iIndex;
        
    }
    return 2;
}

float Find_AverageBlockerDepth(float fCompareValue, float3 vShadowMapTexCoord)
{
    float fAverageDepth = 0.f;
    float fNumBlockers = 0;

    [unroll]
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float fBlockerDepth = g_ShadowTexture.Sample(LinearSampler, vShadowMapTexCoord, int2(i, j)).x;
            float fIsBlocked = step(fBlockerDepth, fCompareValue);
            fAverageDepth += fIsBlocked * fBlockerDepth;
            fNumBlockers += fIsBlocked;
        }

    }

    return fAverageDepth / fNumBlockers;
}

float Compute_ShadowValue(float fCompareValue, float3 vShadowMapTexCoord, float2 vShadowMapTexelSize, float2 vNoiseTexCoord, float fNumFilters)
{
    static uint iNumMaxFilters = 16;

    float fShadowValue = 0.f;
    float4 vRadiusInShadowMap = vShadowMapTexelSize.xyxy * g_ShadowMaskParamas.fRadius;
    
    /* early rejection 하나마나 */
    [unroll]
    for (uint iOffset = 0; iOffset < iNumMaxFilters; ++iOffset)
    {
        if (iOffset > (uint)fNumFilters)
            break;

        float3 vOffsetCoord = float3(iOffset / fNumFilters, vNoiseTexCoord);

        float4 vOffset;
        //vOffset = g_NoiseTexture.Sample(PointSampler, vOffsetCoord) * vShadowMapTexelSize.xyxy * g_ShadowMaskParamas.fRadius;
        vOffset = g_NoiseTexture.Sample(PointSampler, vOffsetCoord) * vRadiusInShadowMap;

        fShadowValue += g_ShadowTexture.SampleCmpLevelZero(LinearCmpBorder, vShadowMapTexCoord + float3(vOffset.xy, 0.f), fCompareValue);
        fShadowValue += g_ShadowTexture.SampleCmpLevelZero(LinearCmpBorder, vShadowMapTexCoord + float3(vOffset.zw, 0.f), fCompareValue);
    }
    fShadowValue /= fNumFilters * 2.f;
    
    return fShadowValue;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexCoord = In.vTexcoord;

    return Out;
}

struct PS_OUT
{
    float fShadowValue : SV_TARGET0;
};

PS_OUT PS_MAIN_HARDSHADOW(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vNormal = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);

    float4 vPixelWorldPos = TransformNDCToWorld(In.vTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv);


    float4 vProjPosFromLight[3];
    float4 vViewPosFromLight[3];

    float2 vTexCoord[3];
    uint iIndex = 0;

    [unroll]
    for (uint i = 0; i < 3; ++i)
    {
        vViewPosFromLight[i] = mul(vPixelWorldPos, g_LightViewMatrices[i]);
        vProjPosFromLight[i] = mul(vViewPosFromLight[i], g_LightProjMatrices[i]);

        vTexCoord[i].x = vProjPosFromLight[i].x * 0.5f + 0.5f;
        vTexCoord[i].y = vProjPosFromLight[i].y * -0.5f + 0.5f;
    }
    
    iIndex = Compute_CascadeIndex(vPixelWorldPos.xyz);

    float3 vLightDir = float3(1.f, -2.f, 1.f); 
    float fShadowValue = 0.f;

    float fBias = 0.2f * pow(iIndex + 1.f, 2);
    fBias += log10(vViewPosFromLight[iIndex].z) * 0.1f;
    fBias += (1.f - dot(vLightDir.xyz, vNormal.xyz)) * 0.05f;

    float fCompareValue = (vViewPosFromLight[iIndex].z - fBias) / 1000.f;

    Out.fShadowValue = g_ShadowTexture.SampleCmp(LinearCmpBorder, float3(vTexCoord[iIndex], iIndex), fCompareValue);
    
    return Out;
}

PS_OUT PS_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vNormal = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);

    float4 vPixelWorldPos = TransformNDCToWorld(In.vTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv);


    float4 vProjPosFromLight[3];
    float4 vViewPosFromLight[3];

    float2 vTexCoord[3];
    uint iIndex = 0;

    [unroll]
    for (uint i = 0; i < 3; ++i)
    {
        vViewPosFromLight[i] = mul(vPixelWorldPos, g_LightViewMatrices[i]);
        vProjPosFromLight[i] = mul(vViewPosFromLight[i], g_LightProjMatrices[i]);

        vTexCoord[i].x = vProjPosFromLight[i].x * 0.5f + 0.5f;
        vTexCoord[i].y = vProjPosFromLight[i].y * -0.5f + 0.5f;
    }
    
    iIndex = Compute_CascadeIndex(vPixelWorldPos.xyz);

    float3 vLightDir = float3(1.f, -2.f, 1.f); // light dir 구해서 표면 노말이랑 곱해가지고 수직일수록 편향값 많이 준다고?
    float fShadowValue = 0.f;

    float fBias = 0.2f * pow(iIndex + 1.f, 2);
    fBias += log10(vViewPosFromLight[iIndex].z) * 0.1f;
    fBias += (1.f - dot(vLightDir.xyz, vNormal.xyz)) * 0.05f;

    float fCompareValue = (vViewPosFromLight[iIndex].z - fBias) / 1000.f;

    Out.fShadowValue = Compute_ShadowValue(
        fCompareValue, 
        float3(vTexCoord[iIndex], iIndex), 
        g_ShadowMaskParamas.vShadowTexelSize.xy, 
        g_ShadowMaskParamas.vNoiseTexelSize.xy * In.vTexCoord,
        //g_ShadowMaskParamas.vNoiseTexelSize.xy * vTexCoord[iIndex],
        g_ShadowMaskParamas.fNumFilters);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass HardShadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HARDSHADOW();
    }
    pass PCF
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

//float2 LightSizeUV = float2(0.005, 0.005); // 광원의 크기 (UV 단위)
//int BlockerSearchSamples = 16;
//int PCFSamples = 16;






//// 1. 블로커 검색
//float FindAverageBlockerDepth(float2 uv, float receiverDepth, out int blockerCount)
//{
//    float avgBlockerDepth = 0.0;
//    blockerCount = 0;

//    for (int i = 0; i < BlockerSearchSamples; ++i)
//    {
//        float2 offset = Hammersley2D(i, BlockerSearchSamples) * LightSizeUV;
//        float sampleDepth = ShadowMap.SampleLevel(ShadowSampler, uv + offset, 0).r;

//        if (sampleDepth < receiverDepth)
//        {
//            avgBlockerDepth += sampleDepth;
//            blockerCount++;
//        }
//    }

//    if (blockerCount > 0)
//        avgBlockerDepth /= blockerCount;

//    return avgBlockerDepth;
//}

//// 2. 펜움브라 크기 추정
//float ComputePenumbraSize(float receiverDepth, float avgBlockerDepth)
//{
//    float zDifference = receiverDepth - avgBlockerDepth;
//    return zDifference / avgBlockerDepth;
//}

//// 3. PCF 필터링
//float PCSSShadow(float2 uv, float receiverDepth)
//{
//    int blockerCount;
//    float avgBlockerDepth = FindAverageBlockerDepth(uv, receiverDepth, blockerCount);

//    // 블로커 없으면 완전 밝게 (광원 막히지 않음)
//    if (blockerCount == 0)
//        return 1.0;

//    // 펜움브라 계산
//    float penumbra = ComputePenumbraSize(receiverDepth, avgBlockerDepth);
//    float2 filterRadiusUV = LightSizeUV * penumbra;

//    float shadow = 0.0;

//    for (int i = 0; i < PCFSamples; ++i)
//    {
//        float2 offset = Hammersley2D(i, PCFSamples) * filterRadiusUV;
//        shadow += ShadowMap.SampleCmpLevelZero(ShadowSampler, uv + offset, receiverDepth);
//    }

//    return shadow / PCFSamples;
//}
