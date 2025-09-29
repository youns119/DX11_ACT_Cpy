#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

/* Terrain Shader */

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// float3로 변경할 예전
float4 g_vLightDir;
float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

Texture2D g_BrushTexture;
Texture2D g_MaskTexture;
Texture2D g_DiffuseTexture[3];
Texture2D g_MSTexture;

Texture2D g_NormalTexture;
Texture2D g_SecondNormalTexture;
Texture2D g_ThirdNormalTexture;

float4 g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

float3 g_vBrushPosition = float3(20.f, 0.f, 20.f);
float g_fBrushRange = 10.f;

uint g_iObjectID;

float g_fMaskDensity;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPosition : POSITION0;
    float4 vProjPosition : POSITION1;
    float4 vTangent : TANGENT;
    float4 vBiNormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	
    Out.vTexCoord = In.vTexCoord;

    Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPosition = Out.vPosition;

    Out.vTangent = normalize(mul(float4(1.f, 0.f, 0.f, 0.f), g_WorldMatrix));
    Out.vBiNormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPosition : POSITION0;
    float4 vProjPosition : POSITION1;
    float4 vTangent : TANGENT;
    float4 vBiNormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vMaterialProperty : SV_Target3; // emissive intensity, smoothness, metalic, 
    float4 vEmissionColor : SV_Target4;
    uint iObjectID : SV_Target5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse[3];
    vDiffuse[0] = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexCoord * 30.f);
    vDiffuse[1] = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexCoord * g_fMaskDensity);
    vDiffuse[2] = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexCoord * 30.f);
    
    
    float4 vMask = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);

    // 검정(메인) 비율 = 1 - (R + G + B)
    float fMainWeight = 1.0 - (vMask.r + vMask.g + vMask.b);
    fMainWeight = max(fMainWeight, 0.0);

    float4 vBaseColor = vDiffuse[0]; // 검정 마스크
    float4 vRedColor = vDiffuse[1]; // 빨강 마스크 (알파 포함)
    float4 vGravelColor = vDiffuse[2]; // 초록

    // 마스크 적용 (0~1)
    float fMaskG = vMask.g;
    float fMaskR = vMask.r;

    // 각 텍스처의 알파를 이용해서 마스크와 곱함
    float fAlphaG = vGravelColor.a * fMaskG;
    float fAlphaR = vRedColor.a * fMaskR;

    // 1. baseColor에 자갈 덮기
    float4 vMixed = lerp(vBaseColor, vGravelColor, fAlphaG);

    // 2. 그 위에 빨강 텍스처 덮기
    vMixed = lerp(vMixed, vRedColor, fAlphaR);

    float4 vMaterialDiffuse = vMixed;
    
    // 스무스니스
    float4 vMetalicSmooth = g_MSTexture.Sample(LinearSampler, In.vTexCoord * 30.f); // vBaseColor와 동일 스케일

    //float fMetallic = vMetalicSmooth.r; // 메탈릭
    float fSmoothness = vMetalicSmooth.a; // 스무스니스

    // 마스크: 검정 영역만 적용 (R+G+B가 작을수록 진함)
    float fBaseMask = fMainWeight;
    //fMetallic *= fBaseMask;
    fSmoothness *= fBaseMask;
 
    // 노멀맵  
    float3 vNormalMap = g_NormalTexture.Sample(LinearSampler, In.vTexCoord * 80.f).zyx * 2.0f - 1.0f;
    float3 vSecNormalMap = g_SecondNormalTexture.Sample(LinearSampler, In.vTexCoord * 2.f).zyx * 2.0f - 1.0f;
    float3 vThirdNormalMap = g_ThirdNormalTexture.Sample(LinearSampler, In.vTexCoord * 2.f).zyx * 2.0f - 1.0f;
    
    // 각 노멀맵에 가중치 부여 (입자 크기, 바람 영향 등 고려)
    float3 vBlendedNormal = normalize(
    vNormalMap * 0.3f + // 미세한 노이즈 
    vSecNormalMap * 0.4f + // 중간 패턴
    vThirdNormalMap * 0.3f // 기본 굴곡 
);
    
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz); /* Tangent To World */
    float3 vNormal = normalize(mul(vBlendedNormal, NormalWorldMatrix)); /* World Space */
    
    Out.vDiffuse = vMaterialDiffuse;

    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.vMaterialProperty = float4(0.f, fSmoothness, 0.f, 0.f);
    //Out.vPickDepth = float2(Out.vDepth.x, 1.f);
    Out.iObjectID = g_iObjectID;
    
    return Out;
}

PS_OUT PS_BRUSH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse[2];
    for (uint i = 0; i < 2; ++i)
        vDiffuse[i] = g_DiffuseTexture[i].Sample(LinearSampler, In.vTexCoord * 30.f);

    float4 vMask = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vBrush = 0.f;

    if (g_vBrushPosition.x - g_fBrushRange <= In.vWorldPosition.x && In.vWorldPosition.x < g_vBrushPosition.x + g_fBrushRange &&
		g_vBrushPosition.z - g_fBrushRange <= In.vWorldPosition.z && In.vWorldPosition.z < g_vBrushPosition.z + g_fBrushRange)
    {
        float2 vTexcoord = 0.f;

        vTexcoord.x = (In.vWorldPosition.x - (g_vBrushPosition.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPosition.z + g_fBrushRange) - In.vWorldPosition.z) / (2.f * g_fBrushRange);

        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }

    float4 vMaterialDiffuse = vDiffuse[0] * vMask + vDiffuse[1] * (1 - vMask) + vBrush;

    Out.vDiffuse = vMaterialDiffuse;

    /* 0 ~ 1 */   /* -1 ~ 1 */
	/* -1 -> 0, 1 -> 1 */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.vPickDepth = float2(Out.vDepth.x, 1.f);
    Out.iObjectID = g_iObjectID;
 
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

    pass BrushPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRUSH();
    }
}