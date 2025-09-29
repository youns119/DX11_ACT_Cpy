#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;
Texture2D g_DepthTexture;

float g_GaussianWeights13[13] = { 0.0022f, 0.0080f, 0.0266f, 0.0648f, 0.1209f, 0.1747f, 0.1974f, 0.1747f, 0.1209f, 0.0648f, 0.0266f, 0.0080f, 0.0022f };
//float g_GaussianWeights9[9] = { 0.0560f, 0.1030f, 0.1653f, 0.2188f, 0.2394f, 0.2188f, 0.1653f, 0.1030f, 0.0560f }; // sigma 2.5
float g_GaussianWeights9[9] = { 0.0001f, 0.0044f, 0.0540f, 0.2420f, 0.3989f, 0.2420f, 0.0540f, 0.0044f, 0.0001f }; // sigma 1.0
float g_GaussianWeights5[5] = { 0.0588f, 0.2353f, 0.4118f, 0.2353f, 0.0588f }; // sigma 1.0

struct RADIAL_BLUR_PARAMS
{
    float2 vCenter;
    float fDensity;
    float fIntensity;
    float fRadius;
} g_RadialBlurParams;


struct BILATERAL_BLUR_PARAMS
{
    float fDepthSigma;
} g_BilateralBlurParams;

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
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_GAUSSIANBLUR9_X(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    [unroll]
    for (unsigned int i = 0; i < 9; ++i)
    {
        float4 vBlur = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(i - 4, 0));
        Out.vColor.xyz += vBlur.xyz * g_GaussianWeights9[i];
    }

    return Out;
}

PS_OUT PS_MAIN_GAUSSIANBLUR9_Y(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;
    [unroll]
    for (unsigned int i = 0; i < 9; ++i)
    {
        float4 vBlur = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(0, i - 4));
        Out.vColor.xyz += vBlur.xyz * g_GaussianWeights9[i];
    }

    return Out;
}

PS_OUT PS_MAIN_GAUSSIANBLUR5_X(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    [unroll]
    for (unsigned int i = 0; i < 5; ++i)
    {
        float4 vBlur = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(i - 2, 0));
        Out.vColor.xyz += vBlur.xyz * g_GaussianWeights5[i];
    }

    return Out;
}

PS_OUT PS_MAIN_GAUSSIANBLUR5_Y(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;
    [unroll]
    for (unsigned int i = 0; i < 5; ++i)
    {
        float4 vBlur = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(0, i - 2));
        Out.vColor.xyz += vBlur.xyz * g_GaussianWeights5[i];
    }

    return Out;
}

PS_OUT PS_MAIN_GAUSSIANBLUR_5x5(VS_OUT In)
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
            float4 vBlur = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(i - 2, j - 2));
            //float4 vBlur = g_FinalTexture.SampleBias(LinearSampler_Clamp, In.vTexCoord, fWeight[i][j] / 40.f, int2(i - 2, j - 2));

            Out.vColor.xyz += vBlur.xyz * fWeight[i][j];
        }
    }

    return Out;
}

PS_OUT PS_MAIN_RADIALBLUR(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexCoordCenter = g_RadialBlurParams.vCenter;
    float2 vToCenterDir = vTexCoordCenter - In.vTexCoord;
    float fSampleDistance = 0.01f * g_RadialBlurParams.fDensity;
    
    float Samples[9] = { -4, -3, -2, -1, 0, 1, 2, 3, 4 };

    float3 vColor = 0.f;
    [unroll]
    for (uint i = 0; i < 9; ++i)
    {
        float2 vTexCoord = saturate( In.vTexCoord + vToCenterDir * Samples[i] * fSampleDistance );
        vColor += g_Texture.Sample(LinearSampler, vTexCoord).xyz * g_GaussianWeights9[i];
    }
    vColor = lerp(g_Texture.Sample(LinearSampler, In.vTexCoord).xyz, vColor, g_RadialBlurParams.fIntensity);

    Out.vColor = float4(vColor, 1.f);
    return Out;
}

PS_OUT PS_MAIN_BOX(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    [unroll]
    for (unsigned int i = 0; i < 5; ++i)
    {
        [unroll]
        for (unsigned int j = 0; j < 5; ++j)
        {
            float4 vBlur = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(i - 2, j - 2));

            Out.vColor.xyz += vBlur.xyz;
        }
    }
    Out.vColor.xzy /= 25.f;

    return Out;
}

PS_OUT PS_MAIN_BILATERAL(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float centerDepth = g_DepthTexture.Sample(LinearSampler_Clamp, In.vTexCoord).y / 1000.f;
    float centerValue = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord).r;

    float4 sum = 0.0f;
    float weightSum = 0.0f;

    float fDepthSigma = g_BilateralBlurParams.fDepthSigma;

    // Sample from -2 to +2 (5 samples)
    [unroll]
    for (int i = -2; i <= 2; ++i)
    {
        [unroll]
        for (int j = -2; j <= 2; ++j)
        {
            float sampleDepth = g_DepthTexture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(i, j)).y / 1000.f;
            float4 sampleValue = g_Texture.Sample(LinearSampler_Clamp, In.vTexCoord, int2(i, j));

            // Compute bilateral weight
            float spatialWeight = exp(-0.5f * (i * i));
            float depthDiff = sampleDepth - centerDepth;
            float depthWeight = exp(-0.5f * (depthDiff * depthDiff) / (fDepthSigma * fDepthSigma));
            float weight = spatialWeight * depthWeight;

            sum += sampleValue * weight;
            weightSum += weight;
        }
    }
    
    Out.vColor = sum / weightSum;

    return Out;
}

technique11 DefaultTechnique
{
    pass GaussianBlur9_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GAUSSIANBLUR9_X();
    }
    pass GaussianBlur9_Y
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GAUSSIANBLUR9_Y();
    }
    pass GaussianBlur5x5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GAUSSIANBLUR_5x5();
    }
    pass GaussianBlur5_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GAUSSIANBLUR5_X();
    }
    pass GaussianBlur5_Y
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GAUSSIANBLUR5_Y();
    }
    pass RadialBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RADIALBLUR();
    }
    pass BoxBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOX();
    }

    pass Bilateral
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BILATERAL();
    }
}
