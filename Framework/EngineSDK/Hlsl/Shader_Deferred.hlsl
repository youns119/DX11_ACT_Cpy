#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_DepthTexture;
Texture2D g_MtrlProperty; // emissive mask
Texture2D g_EmissionColor;

Texture2D g_ShadeTexture;
Texture2D g_AmbientTexture;

Texture2DArray g_ShadowTexture;

Texture2D g_BlendAccTexture;
Texture2D g_BlendRevealTexture;
Texture2D g_DistortionTexture;
Texture2D g_EffectTexture;

Texture2D g_FinalTexture;

// 돌려쓰기 위한 텍스처
Texture2D g_SourTexture;
Texture2D g_DestTexture;

float4x4 g_ProjMatrixInv;
float4x4 g_ViewMatrixInv;

float4 g_vCamPosition;

float4 ACESToneMapping(float4 vColor)
{
    if (vColor.a >= 1.f)
        return vColor;
    else
        return saturate((vColor * (2.51f * vColor + 0.03f)) / (vColor * (2.43f * vColor + 0.59f) + 0.14f));
}

// 포스트 프로세싱 파라미터들
struct FOG_PARAMS
{
    float4 vFogColor;
    float4 vAbyssalColor;
    float2 vFogRange;
    float2 vAbyssalRagne;
} g_FogParams;

struct BLOOM_PARAMS
{
    float fThreshold;
} g_BloomParams;

struct COLORCORRECTION_PARAMS
{
    float fContrast;
    float fBrightness;
    float fSaturation;
    float fTemperature;
    float fTint;
    float fExposure;
    float fGamma;
    float isToneMapping;
} g_ColorCorrectionParams;

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

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_DEFFERED
{
    float4 vColor : SV_TARGET0;
};

float BloomCurve(float x, float fThreshold)
{
    float result = x;
    x *= 2.0f;

#define BLOOMCURVE_METHOD_3
    
#ifdef BLOOMCURVE_METHOD_1
    result = x * 0.05 + max(0, x - fThreshold) * 0.5; // default gThreshold = 1.26
#endif

#ifdef BLOOMCURVE_METHOD_2
    result = x * x / 3.2;
#endif

#ifdef BLOOMCURVE_METHOD_3
    result = max(0, x - fThreshold); // default gThreshold = 1.0
    result *= result;
#endif 

    return result * 0.5f;
}

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(PointSampler, In.vTexCoord);

    return Out;
}

PS_OUT_DEFFERED PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT_DEFFERED Out = (PS_OUT_DEFFERED) 0;

    // shade
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    if (0.f == vDiffuse.a) // 안그린 부분 그냥 자르기
        discard;

    float4 vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vAmbient = g_AmbientTexture.Sample(LinearSampler, In.vTexCoord);

    //Out.vColor = vDiffuse * vShade + vSpecular;
    Out.vColor = vShade + vAmbient;
    /* sRGB 에 있던 Albedo를 다시 Linear 로 변경( 어둡던 원본 색상을 다시 밝은 리니어 공간으로 이동 )*/
    Out.vColor = pow(max(0.f, Out.vColor), 1.f / 2.2f);
    

   // emissive
    float fEmissiveMask = g_MtrlProperty.Sample(LinearSampler, In.vTexCoord).b;
    float4 vEmissionColor = g_EmissionColor.Sample(LinearSampler, In.vTexCoord);

    Out.vColor += fEmissiveMask * vEmissionColor;
    
    return Out;
}

PS_OUT PS_MAIN_DEFERRED_FOG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);
    float fViewZ = vDepthDesc.y;


    float fFogFactor = saturate((fViewZ - g_FogParams.vFogRange.x) / (g_FogParams.vFogRange.y - g_FogParams.vFogRange.x));

    Out.vColor = lerp(vFinal, g_FogParams.vFogColor, fFogFactor);

    
    float fHeight = TransformNDCToWorld(In.vTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv).y;

    float fHeightFactor = saturate((fHeight - g_FogParams.vAbyssalRagne.x) / (g_FogParams.vAbyssalRagne.y - g_FogParams.vAbyssalRagne.x));
    Out.vColor = lerp(g_FogParams.vAbyssalColor, Out.vColor, fHeightFactor);

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);

    Out.vColor = vFinal;

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_BLENDEFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vBlendAcc = g_BlendAccTexture.Sample(LinearSampler, In.vTexCoord);
    vBlendAcc = ACESToneMapping(vBlendAcc);
    float fRevealage = g_BlendRevealTexture.Sample(LinearSampler, In.vTexCoord).r;

    float4 vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexCoord);

    Out.vColor = vFinal * fRevealage + vBlendAcc * (1 - fRevealage) + vEffect;

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 vDistortion = g_DistortionTexture.Sample(LinearSampler_Clamp, In.vTexCoord);

    //float2 vDistortionUV = (vDistortion.xy * 2.f - 1.f) * 0.015f;
    
    //float4 vFinal = 0;
    
    //if (vDistortion.a == 0.f)
    //    vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);
    //else
    //    vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexCoord + vDistortionUV);
    
    float2 vDistortionUV = vDistortion.xy * 0.025f;
    float4 vFinal = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexCoord + vDistortionUV);

    Out.vColor = vFinal;

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_QUANTIZATION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);

    //static const float DitherMatrix[8][8] =
    //{
    //    { 0, 32, 8, 40, 2, 34, 10, 42 },
    //    { 48, 16, 56, 24, 50, 18, 58, 26 },
    //    { 12, 44, 4, 36, 14, 46, 6, 38 },
    //    { 60, 28, 52, 20, 62, 30, 54, 22 },
    //    { 3, 35, 11, 43, 1, 33, 9, 41 },
    //    { 51, 19, 59, 27, 49, 17, 57, 25 },
    //    { 15, 47, 7, 39, 13, 45, 5, 37 },
    //    { 63, 31, 55, 23, 61, 29, 53, 21 }
    //};
    static const float DitherMatrix[4][4] =
    {
        { 0.0, 8.0, 2.0, 10.0 },
        { 12.0, 4.0, 14.0, 6.0 },
        { 3.0, 11.0, 1.0, 9.0 },
        { 15.0, 7.0, 13.0, 5.0 }
    };


    int iMatrixDimension = 4;
    //float2 TexCoord = float2(In.vTexCoord.x * 1280.f, In.vTexCoord.y * 720.f);
    float Threshold = DitherMatrix[(unsigned int) In.vPosition.x % iMatrixDimension][(unsigned int) In.vPosition.y % iMatrixDimension];
    
    float fSpread = g_vCamPosition.x; // temp global value
    //float4 vColor = vFinal + (Threshold / pow(iMatrixDimension, 2.f) - 0.5f) * 0.25f;
    float4 vColor = vFinal + (Threshold / float(iMatrixDimension * iMatrixDimension) - 0.5f) * g_vCamPosition.x;

    //int iNumColors = 4;
    //int iNumColors = 12;
    int iNumColors = 10;
  
    Out.vColor = floor(vColor * (iNumColors - 1) + 0.5f) / (iNumColors - 1);
    
    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_EDGEDETECTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    //float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vFinalColor = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);
    
    static const float HighPassFilter[5][5] =
    {
        { 0.f, 0.f, -1.f, 0.f, 0.f },
        { 0.f, -1.f, -2.f, -1.f, 0.f },
        { -1.f, -2.f, 16.f, -2.f, -1.f },
        { 0.f, -1.f, -2.f, -1.f, 0.f },
        { 0.f, 0.f, -1.f, 0.f, 0.f }
    };

    //static const float HighPassFilter[3][3] =
    //{
    //    {-1.f, -1.f, -1.f },
    //    {-1.f, 8.f, -1.f, },
    //    {-1.f, -1.f, -1.f },
    //};

    

    float3 vNormalAcc = 0.f;
    float vDepthAcc = 0.f;

    float4 vColor = 0.f;
    for (unsigned int i = 0; i < 5; ++i)
    {
        for (unsigned int j = 0; j < 5; ++j)
        {
            float2 vTexCoord = float2(In.vTexCoord.x + (1.f / 1280.f) * (i - 2.f), In.vTexCoord.y + (1.f / 720.f) * (j - 2.f));
            //float2 vTexCoord = float2(In.vTexCoord.x + (1.f / 1280.f) * (i - 1.f), In.vTexCoord.y + (1.f / 720.f) * (j - 1.f));
            float3 vNormalPass = g_NormalTexture.Sample(LinearSampler_Clamp, vTexCoord).rgb;
            float vDepthPass = g_DepthTexture.Sample(LinearSampler_Clamp, vTexCoord).y;
            vNormalAcc += HighPassFilter[i][j] * vNormalPass;
            vDepthAcc += HighPassFilter[i][j] * vDepthPass;
        }
    }

    //float fEdge = length(vNormalAcc + vDepthAcc) / 1.f;
    float fEdge = length(vNormalAcc + vDepthAcc) / 2.f;
    fEdge = step(0.8f, fEdge);

    //Out.vColor = min(vFinalColor, vFinalColor + (1.f - fEdge));

    Out.vColor = vFinalColor - fEdge;


    
    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_BLOOM(PS_IN In)
{
    Texture2D DownSamplingTexture = g_SourTexture;
    Texture2D FinalTexture = g_FinalTexture;
    PS_OUT Out = (PS_OUT) 0;

    float3 vColor = g_FinalTexture.Sample(LinearSampler, In.vTexCoord).rgb;
    float3 vBlured = DownSamplingTexture.Sample(LinearSampler_Clamp, In.vTexCoord).rgb;

    Out.vColor.xyz = vColor + vBlured;

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_BRIGHTNESSDETECTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vFinalColor = g_FinalTexture.Sample(LinearSampler, In.vTexCoord);

    float fBrightness = dot(vFinalColor.xyz, float3(0.2126f, 0.7152f, 0.0722f));
    Out.vColor = vFinalColor * step(g_BloomParams.fThreshold, fBrightness);

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_BLOOMCURVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 vColor = g_FinalTexture.Sample(LinearSampler, In.vTexCoord).xyz;

    float fBrightness = dot(vColor, float3(0.2126f, 0.7152f, 0.0722f));
    //float fBrightness = dot(vColor, float3(0.3f, 0.3f, 0.3f));
    float fBloomIntensity = BloomCurve(fBrightness, g_BloomParams.fThreshold);
    //float3 vBloomColor = vColor * fBloomIntensity / fBrightness;
    float3 vBloomColor = vColor * fBloomIntensity;
    Out.vColor = float4(vBloomColor, 1.f);

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_SAMPLING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    [unroll]
    for (unsigned int i = 0; i < 5; ++i)
    {
        [unroll]
        for (unsigned int j = 0; j < 5; ++j)
        {
            int2 vOffset = int2((i - 2) * 1, (j - 2) * 1);
            float4 vBlur = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexCoord, vOffset);
            //float4 vBlur = g_FinalTexture.SampleLevel(LinearSampler_Clamp, In.vTexCoord, 1.f, vOffset);
            Out.vColor.xyz += vBlur.xyz / 25.f;
        }
    }
    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    static const float fWeight[5][5] =
    {
        { 1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f },
        { 4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f },
        { 7 / 273.0f, 26 / 273.0f, 41 / 273.0f, 26 / 273.0f, 7 / 273.0f },
        { 4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f },
        { 1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f }
    };

    [unroll]
    for (unsigned int i = 0; i < 5; ++i)
    {
        [unroll]
        for (unsigned int j = 0; j < 5; ++j)
        {
            float4 vBlur = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(i - 2, j - 2));
            //float4 vBlur = g_FinalTexture.SampleBias(LinearSampler_Clamp, In.vTexCoord, fWeight[i][j] / 40.f, int2(i - 2, j - 2));

            Out.vColor.xyz += vBlur.xyz * fWeight[i][j];
        }
    }

    return Out;
}

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

PS_OUT PS_MAIN_UIBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 blurX = 0;
    float4 blurY = 0;

    [unroll]
    for (int i = -6; i <= 6; ++i)
    {
        float2 offset = float2((10.f / 1280.f) * i, 0);
        blurX += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexCoord + offset);
    }
    
    blurX /= 6.f;

    [unroll]
    for (int j = -6; j <= 6; ++j)
    {
        float2 offset = float2(0, (10.f / 720.f) * j);
        blurY += g_fWeights[j + 6] * g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexCoord + offset);
    }
    blurY /= 6.f;

    Out.vColor = (blurX + blurY) * 0.5f;
    Out.vColor.rgb *= 0.7f;
    
    return Out;
}


PS_OUT PS_MAIN_POSTPROCESS_BLOOM_ACC(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 vBlured = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexCoord).rgb;
    float3 vBloomCurve = g_SourTexture.Sample(LinearSampler_Clamp, In.vTexCoord).rgb;
    //Out.vColor = float4(vBlured * 0.5f + vBloomCurve * 0.5f, 1.f);
    Out.vColor = float4(vBlured + vBloomCurve, 1.f);

    return Out;
}

PS_OUT PS_MAIN_POSTPROCESS_COLOR_CORRECTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;


    float3 vColor = g_FinalTexture.Sample(LinearSampler, In.vTexCoord).rgb;

    /* 이미 색상 선형 공간임 */

    const float fContrast = g_ColorCorrectionParams.fContrast;
    const float fBrightness = g_ColorCorrectionParams.fBrightness;
    const float fSaturation = g_ColorCorrectionParams.fSaturation;
    const float fTemperature = clamp(g_ColorCorrectionParams.fTemperature, 1000.f, 40000.f) / 100.f;
    const float fTint = g_ColorCorrectionParams.fTint;
    const float fExposure = g_ColorCorrectionParams.fExposure;
    const float fGamma = g_ColorCorrectionParams.fGamma;

    /* Exposure */
    vColor *= fExposure;

    /* White Balance */
    float3 vBalanceFactor;
#define GPU_FRIENDLY
#ifdef GPU_FRIENDLY
    vBalanceFactor.r = lerp(
        1.f,
        saturate(1.2929361f * pow(max(0.f, fTemperature - 60.f), -0.1332f)),
        step(66.f, fTemperature));
    vBalanceFactor.g = lerp(
        saturate(0.3900815787690196f * log(fTemperature) - 0.6318414437886275f),
        saturate(1.129890860895294f * pow(max(0.f, fTemperature - 60.f), -0.0755148492f)),
        step(66.f, fTemperature)) + fTint;
    vBalanceFactor.b = saturate(0.543206789110196 * log(fTemperature - 10.0) - 1.19625408914f);
#else
    // R
    if (fTemperature <= 66.0)
        vBalanceFactor.r = 1.0;
    else
        vBalanceFactor.r = saturate(1.292936186062745 * pow((fTemperature - 60.0), -0.1332047592));

    // G
    if (fTemperature <= 66.0)
        vBalanceFactor.g = saturate(0.3900815787690196 * log(fTemperature) - 0.6318414437886275);
    else
        vBalanceFactor.g = saturate(1.129890860895294 * pow((fTemperature - 60.0), -0.0755148492));
    vBalanceFactor.g += fTint;

    // B
    if (fTemperature >= 66.0)
        vBalanceFactor.b = 1.0;
    else if (fTemperature <= 19.0)
        vBalanceFactor.b = 0.0;
    else
        vBalanceFactor.b = saturate(0.543206789110196 * log(fTemperature - 10.0) - 1.19625408914);
#endif
    vColor *= vBalanceFactor;

    /* Contrast, Brightness */
    vColor = fContrast * (vColor - 0.5f) + 0.5f + fBrightness;

    
    /* Hue shifting */
    // 나중에 하자


    /* Saturation */
    //float vGrayValue = dot(vColor, float3(0.2126f, 0.7152f, 0.0722f));
    float vGrayValue = dot(vColor, float3(0.299, 0.587, 0.114));
    vColor = lerp(vGrayValue, vColor, fSaturation);
    

    /* Tone mapping */
    float3 vToneMapping = vColor * (2.51f * vColor + 0.03f) / (vColor * (2.43f * vColor + 0.59f) + 0.14f);
    vColor = lerp(vColor, vToneMapping, saturate(g_ColorCorrectionParams.isToneMapping));
    vColor = saturate(vColor);

    /* Gamma */
    /* aces 톤 매핑의 결과로 밝아진 느낌을 감마로 보정할 수 있음 */
    vColor = pow(vColor, fGamma);

    Out.vColor = float4(vColor, 1.f);
    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    pass DeferredFog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_FOG();
    }

    pass PostProcess_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_FINAL();
    }

    pass PostProcess_Quantization
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_QUANTIZATION();
    }

    pass PostProcess_EdgeDetection
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_EDGEDETECTION();
    }

    pass PostProcess_Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_BLOOM();
    }

    pass PostProcess_Sampling
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_SAMPLING();
    }

    pass PostProcess_BloomCurve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_BLOOMCURVE();
    }

    pass PostProcess_BlendEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_BLENDEFFECT();
    }

    pass PostProcess_Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_DISTORTION();
    }

    pass PostProcess_Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_BLUR();
    }

    pass PostProcess_BloomAcc
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_BLOOM_ACC();
    }

    pass PostProcess_ColorCorrection
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESS_COLOR_CORRECTION();
    }

    pass PostProcess_UIBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UIBLUR();
    }
}
