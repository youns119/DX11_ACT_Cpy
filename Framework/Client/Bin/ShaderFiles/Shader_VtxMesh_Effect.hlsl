#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"


float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_BaseTexture;
Texture2D g_MaskTexture;
Texture2D g_NoiseTexture;
Texture2D g_EmissiveTexture;
Texture2D g_DissolveTexture;
Texture2D g_DistortionTexture;

float3 g_vMulColor;
float3 g_vAddColor;

int g_iShadingOption;
float2 g_vDissolveStart;
float g_fLifeRate;

float2 g_vUVProgress;
float2 g_vUVTileCount;

float g_fMaxTrailLength;


float BOITWeight(in float z, in float zw, in float a)
{
    return pow(a, 2.f) / ((a + 0.0001f) * 3.f + (z + 0.0001f) * 0.01f);
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPosition : POSITION1;
    float4 vProjPosition : POSITION2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 matWV;
    float4x4 matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPosition = Out.vPosition;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPosition : POSITION1;
    float4 vProjPosition : POSITION2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vDepth : SV_Target1;
};

struct PS_OUT_BLENDACC
{
    float4 vDiffuseAcc : SV_Target0;
};

struct PS_OUT_REVEAL
{
    float4 vRevealage : SV_Target0;
};

struct PS_OUT_DISTORTION
{
    float4 vDistortion : SV_Target0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_BaseTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
    
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse.rgb *= g_vMulColor;
    Out.vDiffuse.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        Out.vDiffuse.a = Out.vDiffuse.a * vMaskDesc.r;
        
        if (Out.vDiffuse.a < 0.1f)
            discard;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
            
            Out.vDiffuse.a *= (g_fLifeRate / g_vDissolveStart.x);
            if (Out.vDiffuse.a < 0.1f)
                discard;
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
            Out.vDiffuse.a *= ((1 - (g_fLifeRate - g_vDissolveStart.y)) / (1 - g_vDissolveStart.y));
            if (Out.vDiffuse.a < 0.1f)
                discard;
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 2)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        float4 vEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
        vEmissionDesc *= vMaskDesc.r;
        Out.vDiffuse += vEmissionDesc;
    }
    
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT_DISTORTION PS_DISTORTION(PS_IN In)
{
    PS_OUT_DISTORTION Out = (PS_OUT_DISTORTION) 0;
    float4 vDiffuse = g_BaseTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
    
    if (vDiffuse.a < 0.1f)
        discard;

    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    Out.vDistortion = g_DistortionTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        Out.vDistortion.r = vMaskDesc.r;
    }
    
    Out.vDistortion *= (1.f - pow(g_fLifeRate, 4.f));
    
    return Out;
}

PS_OUT PS_MAIN_TRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTrailCoord;
    
    if (g_vUVProgress.x < g_fMaxTrailLength)
    {
        if (In.vTexCoord.x >= g_vUVProgress.x)
            discard;
        // In.vTexCoord = 0.0 ~ 0.2, TrailLength : 0.4, g_vUVProgress : 0.2
        // vTrailCoord = 0 ~ 1
        vTrailCoord = In.vTexCoord;
    }
    else // if (g_vUVProgress.x < 1.f)
    {
        // discard;
        if (In.vTexCoord.x > g_vUVProgress.x || In.vTexCoord.x < (g_vUVProgress.x - g_fMaxTrailLength))
            discard;
        // In.vTexCoord = 0.2 ~ 0.6, TrailLength : 0.4, g_vUVProgress : 0.6, g_vUVProgress - TrailLength : 0.2
        // vTrailCoord = 0 ~ 1
        vTrailCoord = float2((In.vTexCoord.x - (g_vUVProgress.x - g_fMaxTrailLength)) / g_fMaxTrailLength, In.vTexCoord.y);
    }
    /*
    
    if(g_vUVProgress.x < fMaxLength)
        0 ~ g_vUVProgress
    else if(g_vUVProgress.x < 1.f)
        g_vUVProgress - fMaxLength ~ g_vUVProgress
    else
        애니메이션 멈추고 디졸브 시작
     - 디퓨즈 : 디졸브 텍스쳐 적용하여 디졸브
     - 디스토션 : 알파값 빼기

    */
    
    vTrailCoord = float2(1.f - vTrailCoord.x, vTrailCoord.y);
    Out.vDiffuse = g_BaseTexture.Sample(PointSampler, vTrailCoord);
    
    Out.vDiffuse.rgb *= g_vMulColor;
    Out.vDiffuse.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, vTrailCoord);
        Out.vDiffuse.a = Out.vDiffuse.a * vMaskDesc.r;
        
        if (Out.vDiffuse.a < 0.1f)
            discard;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 2)
    {
        float4 fEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, vTrailCoord);
        Out.vDiffuse += fEmissionDesc;
    }
    

    return Out;
}

PS_OUT_DISTORTION PS_DISTORTION_TRAIL(PS_IN In)
{
    PS_OUT_DISTORTION Out = (PS_OUT_DISTORTION) 0;
    
    float2 vTrailCoord;
    
    if (g_vUVProgress.x < g_fMaxTrailLength)
    {
        if (In.vTexCoord.x >= g_vUVProgress.x)
            discard;

        vTrailCoord = In.vTexCoord;
    }
    else // if (g_vUVProgress.x < 1.f)
    {
        if (In.vTexCoord.x > g_vUVProgress.x || In.vTexCoord.x < (g_vUVProgress.x - g_fMaxTrailLength))
            discard;

        vTrailCoord = float2((In.vTexCoord.x - (g_vUVProgress.x - g_fMaxTrailLength)) / g_fMaxTrailLength, In.vTexCoord.y);
    }
    
    vTrailCoord = float2(1.f - vTrailCoord.x, vTrailCoord.y);
        
     // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    Out.vDistortion = g_DistortionTexture.Sample(LinearSampler, vTrailCoord);
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        Out.vDistortion.r = vMaskDesc.r;
    }
    
    Out.vDistortion *= (1.f - pow(g_fLifeRate, 4.f));
    
    return Out;
}

PS_OUT_BLENDACC PS_MAIN_ACC(PS_IN In)
{
    PS_OUT_BLENDACC Out = (PS_OUT_BLENDACC) 0;
    
    Out.vDiffuseAcc = g_BaseTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
    
    if (Out.vDiffuseAcc.a < 0.1f)
        discard;
    
    Out.vDiffuseAcc.rgb *= g_vMulColor;
    Out.vDiffuseAcc.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        Out.vDiffuseAcc.a = Out.vDiffuseAcc.a * vMaskDesc.r;
        
        if (Out.vDiffuseAcc.a < 0.1f)
            discard;
    }
    
    float fWeight = BOITWeight(In.vPosition.z, In.vPosition.z / In.vPosition.w, Out.vDiffuseAcc.a);
    // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
            
            fWeight *= max(1.f, (g_vDissolveStart.x - g_fLifeRate) * 2.f / g_vDissolveStart.x);
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
            
            fWeight *= max(1.f, (g_fLifeRate - g_vDissolveStart.y) * 2.f / (1 - g_vDissolveStart.y));
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 2)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        float4 vEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
        vEmissionDesc *= vMaskDesc.r;
        Out.vDiffuseAcc += vEmissionDesc;
    }
    
    
    
    Out.vDiffuseAcc = float4(Out.vDiffuseAcc.r * fWeight, Out.vDiffuseAcc.g * fWeight, Out.vDiffuseAcc.b * fWeight, Out.vDiffuseAcc.a);
    
    
    return Out;
}

PS_OUT_BLENDACC PS_MAIN_ACC_TRAIL(PS_IN In)
{
    PS_OUT_BLENDACC Out = (PS_OUT_BLENDACC) 0;
    float2 vTrailCoord;
    
    if (g_vUVProgress.x < g_fMaxTrailLength)
    {
        if (In.vTexCoord.x >= g_vUVProgress.x)
            discard;
        // In.vTexCoord = 0.0 ~ 0.2, TrailLength : 0.4, g_vUVProgress : 0.2
        // vTrailCoord = 0 ~ 1
        vTrailCoord = In.vTexCoord;
    }
    else // if (g_vUVProgress.x < 1.f)
    {
        if (In.vTexCoord.x > g_vUVProgress.x || In.vTexCoord.x < (g_vUVProgress.x - g_fMaxTrailLength))
            discard;

        vTrailCoord = float2((In.vTexCoord.x - (g_vUVProgress.x - g_fMaxTrailLength)) / g_fMaxTrailLength, In.vTexCoord.y);
    }

    vTrailCoord = float2(1.f - vTrailCoord.x, vTrailCoord.y);
    Out.vDiffuseAcc = g_BaseTexture.Sample(PointSampler, vTrailCoord);
    
    Out.vDiffuseAcc.rgb *= g_vMulColor;
    Out.vDiffuseAcc.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, vTrailCoord);
        Out.vDiffuseAcc.a = Out.vDiffuseAcc.a * vMaskDesc.r;
        
        if (Out.vDiffuseAcc.a < 0.1f)
            discard;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 2)
    {
        float4 fEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, vTrailCoord);
        Out.vDiffuseAcc += fEmissionDesc;
    }
    
    float fWeight = BOITWeight(In.vPosition.z, In.vPosition.z / In.vPosition.w, Out.vDiffuseAcc.a);
    Out.vDiffuseAcc = float4(Out.vDiffuseAcc.r * fWeight, Out.vDiffuseAcc.g * fWeight, Out.vDiffuseAcc.b * fWeight, Out.vDiffuseAcc.a);
    
    return Out;
}

PS_OUT_REVEAL PS_MAIN_REVEALAGE(PS_IN In)
{
    PS_OUT_REVEAL Out = (PS_OUT_REVEAL) 0;
    
    float4 vDiffuseAcc = g_BaseTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
    
    if (vDiffuseAcc.a < 0.1f)
        discard;
    
    vDiffuseAcc.rgb *= g_vMulColor;
    vDiffuseAcc.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        vDiffuseAcc.a = vDiffuseAcc.a * vMaskDesc.r;
        
        if (vDiffuseAcc.a < 0.1f)
            discard;
    }
    
    float fWeight = BOITWeight(In.vPosition.z, In.vPosition.z / In.vPosition.w, vDiffuseAcc.a);
    // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
            
            fWeight *= max(1.f, (g_vDissolveStart.x - g_fLifeRate) * 2.f / g_vDissolveStart.x);
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
            
            fWeight *= max(1.f, (g_fLifeRate - g_vDissolveStart.y) * 2.f / (1 - g_vDissolveStart.y));
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 2)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
        float4 vEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, float2((In.vTexCoord.x + g_vUVProgress.x) * g_vUVTileCount.x, (In.vTexCoord.y + g_vUVProgress.y) * g_vUVTileCount.y));
        vEmissionDesc *= vMaskDesc.r;
        vDiffuseAcc += vEmissionDesc;
    }
    
    vDiffuseAcc = float4(vDiffuseAcc.r * fWeight, vDiffuseAcc.g * fWeight, vDiffuseAcc.b * fWeight, vDiffuseAcc.a);
    Out.vRevealage.r = vDiffuseAcc.a * fWeight;

    
    return Out;
}

PS_OUT_REVEAL PS_MAIN_REVEALAGE_TRAIL(PS_IN In)
{
    PS_OUT_REVEAL Out = (PS_OUT_REVEAL) 0;
    
    float2 vTrailCoord;
    
    if (g_vUVProgress.x < g_fMaxTrailLength)
    {
        if (In.vTexCoord.x >= g_vUVProgress.x)
            discard;
        vTrailCoord = In.vTexCoord;
    }
    else // if (g_vUVProgress.x < 1.f)
    {
        if (In.vTexCoord.x > g_vUVProgress.x || In.vTexCoord.x < (g_vUVProgress.x - g_fMaxTrailLength))
            discard;

        vTrailCoord = float2((In.vTexCoord.x - (g_vUVProgress.x - g_fMaxTrailLength)) / g_fMaxTrailLength, In.vTexCoord.y);
    }

    vTrailCoord = float2(1.f - vTrailCoord.x, vTrailCoord.y);
    float4 vDiffuseAcc = g_BaseTexture.Sample(PointSampler, vTrailCoord);
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, vTrailCoord);
        vDiffuseAcc.a = vDiffuseAcc.a * vMaskDesc.r;
        
        if (vDiffuseAcc.a < 0.1f)
            discard;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 1)
    {
        if (g_fLifeRate <= g_vDissolveStart.x)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_vDissolveStart.x - g_fLifeRate) / g_vDissolveStart.x)
                discard;
        }
        if (g_fLifeRate >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_NoiseTexture.Sample(LinearSampler, vTrailCoord);
            if (vDissolveDesc.r < (g_fLifeRate - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 2)
    {
        float4 fEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, vTrailCoord);
        vDiffuseAcc += fEmissionDesc;
    }
    
    float fWeight = BOITWeight(In.vPosition.z, In.vPosition.z / In.vPosition.w, vDiffuseAcc.a);
    Out.vRevealage.r = vDiffuseAcc.a * fWeight;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass TrailPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TRAIL();
    }

    pass DistortionPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass DistortionPass_Trail
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_TRAIL();
    }

    pass BlendAccPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_BlendAcc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ACC();
    }

    pass BlendAccPass_Trail
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_BlendAcc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ACC_TRAIL();
    }

    pass RevealagePass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_Revealage, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_REVEALAGE();
    }

    pass RevealagePass_Trail
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_Revealage, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_REVEALAGE_TRAIL();
    }
}