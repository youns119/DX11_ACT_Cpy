#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_NormalTexture;
Texture2D g_DepthTexture;
Texture2D g_RandomRotationNoiseTexture;
Texture3D g_RandomSampleTexture;

Texture2D g_FinalTexture;
Texture2D g_AOTexture;

float4x4 g_CamProjMatrix;
float4x4 g_CamViewMatrix;

float4x4 g_CamProjMatrixInv;
float4x4 g_CamViewMatrixInv;

//float4 g_RandomVectors[8 * 8];
//uint2 g_vArraySize;

static const uint g_iKernelSize = 24;
float4 g_Kernals[g_iKernelSize];

struct SSAO_PARAMS
{
    float fRadius;
    float fBias;
} g_SSAOParams;

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
    float fValue : SV_TARGET0;
};

struct PS_OUT_COLOR
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_COMPUTE_AO(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);

    clip(vDepthDesc.z - 0.1f);

    float4 vPixelProjPos;
    vPixelProjPos.x = In.vTexCoord.x * 2.f - 1.f;
    vPixelProjPos.y = In.vTexCoord.y * -2.f + 1.f;
    vPixelProjPos.z = vDepthDesc.x;
    vPixelProjPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    float fViewZ = vDepthDesc.y;
    vPixelProjPos = vPixelProjPos * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    float4 vPixelViewPos = mul(vPixelProjPos, g_CamProjMatrixInv);
    float4 vPixelWorldPos = mul(vPixelViewPos, g_CamViewMatrixInv);

    
    //uint2 vIndex = (uint2)In.vPosition.xy % g_vArraySize;
    //uint2 vIndex;
    //vIndex.x = In.vPosition.x % g_vArraySize.x;
    //vIndex.y = In.vPosition.y % g_vArraySize.y;
    //float3 vRandomVector = normalize(float3(g_RandomVectors[vIndex.y * g_vArraySize.x + vIndex.x].xy, 0.f));
    float3 vRandomVector = float3(g_RandomRotationNoiseTexture.Sample(PointSampler, In.vTexCoord / float2(1280.f, 720.f)).xy, 0.f);

    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    //float3 vTangent = normalize(vRandomVector - vNormal * dot(vNormal, vRandomVector));
    float3 vTangent = normalize(float3(1.f, 0.f, 0.f) - vNormal * vNormal.x);
    float3 vBinormal = cross(vNormal, vTangent);
    
    float3x3 TBNMatrix = float3x3(vTangent, vBinormal, vNormal);

    float fOcclusion = 0.f;

    float fBias = g_SSAOParams.fBias * max(0.f, log(vPixelViewPos.z));

    [unroll]
    //for (uint i = 0; i < g_iKernelSize; ++i)
    for (uint i = 0; i < 16; ++i)
    {
        //float fRandomRadius = g_SSAOParams.fRadius * g_Kernals[i].w;
        float3 vSampleDir = g_RandomSampleTexture.Sample(PointSampler, float3(i / 16.f, In.vTexCoord / float2(1280.f, 720.f))).xyz;
        vSampleDir = mul(vSampleDir, TBNMatrix);
       //float4 vSampleDir = float4(mul(normalize(g_Kernals[i].xyz), TBNMatrix), 0.f);

        float3 vSampleWorldPos = vSampleDir * g_SSAOParams.fRadius + vPixelWorldPos.xyz;

        float4 vSampleViewPos = mul(float4(vSampleWorldPos, 1.f), g_CamViewMatrix);
        float4 vSampleProjPos = mul(vSampleViewPos, g_CamProjMatrix);
        vSampleProjPos /= vSampleProjPos.w;
        
        float2 vSampleTexCoord;
        vSampleTexCoord.x = vSampleProjPos.x * 0.5f + 0.5f;
        vSampleTexCoord.y = 0.5f - vSampleProjPos.y * 0.5f;

        float4 vSampleDepthDesc = g_DepthTexture.Sample(LinearSampler_Clamp, vSampleTexCoord);

        // fRange check 
        float fRangeCheck = saturate(length(vSampleDir) * g_SSAOParams.fRadius / abs(vSampleDepthDesc.y - vSampleViewPos.z));

        // 거리에 따른 다른 편향 값을 주도록 해야겠다
        fOcclusion += step(vSampleDepthDesc.y, vSampleViewPos.z - fBias) * fRangeCheck;
    }

    Out.fValue = fOcclusion / g_iKernelSize;
    //Out.vColor = float4(vRandomVector, 1.f);

    return Out;
}

PS_OUT_COLOR PS_MAIN_APPLY_AO(VS_OUT In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;

    float4 vColor = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);
    float vAO = g_AOTexture.Sample(LinearSampler, In.vTexCoord).r;

    vColor *= saturate(1.f - vAO);
    
    Out.vColor = vColor;
    return Out;
}

technique11 DefaultTechnique
{
    pass Compute_AO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMPUTE_AO();
    }

    pass Apply_AO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_APPLY_AO();
    }
}
