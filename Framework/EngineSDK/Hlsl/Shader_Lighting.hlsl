#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 g_vLightDir;
float4 g_vLightPos;
float g_fLightRange;
float2 g_vCosCutOff;

float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

Texture2D g_DiffuseTexture;
float4 g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
Texture2D g_MtrlProperty; // metalic smoothness

Texture2D g_NormalTexture;
Texture2D g_DepthTexture;
Texture2D g_ShadowMaskTexture;

float4x4 g_ProjMatrixInv;
float4x4 g_ViewMatrixInv;

float4 g_vCamPosition;

float3 g_vAttenuation; // constant linear quadratic 

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

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vAmbient : SV_Target1;
};

float3 LambertDiffuse(float3 vColor)
{
    return vColor / PI;
}

float DistributionGGX(float3 vNormal, float3 fhalfway, float fRoughness)
{
    float fAlpha = fRoughness * fRoughness;
    float fAlpha2 = fAlpha * fAlpha;
    float NoH = dot(vNormal, fhalfway);
    float NoH2 = NoH * NoH;

    float fNumerator = fAlpha2;
    float fDenominator = (NoH2 * (fAlpha2 - 1.0f) + 1.0f);
    fDenominator = PI * fDenominator * fDenominator;
    return fNumerator / fDenominator;
}

float GeometrySchlickGGX(float fNoV, float fRoughness)
{
    float r = (fRoughness + 1.0f);
    float k = (r * r) / 8.0f;

    float fNumerator = fNoV;
    float fDenominator = fNoV * (1.0f - k) + k;
    return fNumerator / fDenominator;
}

float GeometrySmith(float3 vNormal, float3 vViewDir, float3 vLightDir, float fRoughness)
{
    float fNoV = max(dot(vNormal, vViewDir), 0.0f);
    float fNoL = max(dot(vNormal, vLightDir), 0.0f);
    float fGGX_V = GeometrySchlickGGX(fNoV, fRoughness);
    float fGGX_L = GeometrySchlickGGX(fNoL, fRoughness);
    return fGGX_V * fGGX_L;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

PS_OUT_LIGHT ComputePBRLighring(float3 vInRadiance, float3 vLightDir, float3 vViewDir, float3 vNormal, float3 vAlbedo, float fMetalic, float fRoughness)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    float3 vHalfway = normalize(vLightDir + vViewDir);
    float fNoV = max(dot(vNormal, vViewDir), 0.f);
    float fHoV = max(dot(vHalfway, vViewDir), 0.1f);
    float3 F0 = lerp(0.04f, vAlbedo, fMetalic);
    
    
    // Cook-Torrance BRDF
    float fDistribution = DistributionGGX(vNormal, vHalfway, fRoughness);
    float fGeometry = GeometrySmith(vNormal, vViewDir, vLightDir, fRoughness);
    float3 vFresnel = FresnelSchlick(fHoV, F0);

    float3 kS = vFresnel;
    float3 kD = 1.0 - kS;
    kD *= (1.0 - fMetalic);

    float fNoL = max(dot(vNormal.xyz, vLightDir), 0.0f);
    float3 vNumerator = fDistribution * fGeometry * vFresnel;
    float vDenominator = max(4.0f * fNoV * fNoL, 0.1f);
    float3 vSpecular = vNumerator / vDenominator;

    float3 vDiffuse = LambertDiffuse(vAlbedo);

    // add to outgoing radiance Lo
    float3 OutRadiance = (kD * vDiffuse + vSpecular) * vInRadiance * fNoL;
    float3 vColor = OutRadiance;
    Out.vShade = float4(OutRadiance, 1.f);

    // calculate ambient
    float3 vAmbientDiffuse = kD * vAlbedo /* IBL */;
    float3 vAmbientSpecular = FresnelSchlick(max(fNoV, 0.1f), 0.f) * F0;
    Out.vAmbient = float4((vAmbientDiffuse + vAmbientSpecular) * g_vLightAmbient.rgb, 1.f);

    return Out;
}


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    float4 vAlbedoDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    clip(vAlbedoDesc.a - 0.1f);

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vMtrlPropertyDesc = g_MtrlProperty.Sample(LinearSampler, In.vTexCoord);
    float4 vPixelWorldPos = TransformNDCToWorld(In.vTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv);

    float3 vRadiance = g_vLightDiffuse.xyz;
    float3 vLightDir = normalize(-1.f * g_vLightDir.xyz);
    float3 vViewDir = normalize(g_vCamPosition.xyz - vPixelWorldPos.xyz);
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float3 vAlbedo = pow(max(0.f, vAlbedoDesc.rgb), 2.2f);
    float fMetalic = vMtrlPropertyDesc.r;
    float fRoughness = clamp(1.f - vMtrlPropertyDesc.g, 0.5f, 1.f);
    
    /* tone mapping */
    //vColor /= (vColor + 1.f);
    //vColor = saturate((vColor * (2.51f * vColor + 0.03f)) / (vColor * (2.43f * vColor + 0.59f) + 0.14f));

    Out = ComputePBRLighring(vRadiance, vLightDir, vViewDir, vNormal, vAlbedo, fMetalic, fRoughness);
    Out.vShade = lerp(0.f, Out.vShade, saturate(g_ShadowMaskTexture.Sample(LinearSampler, In.vTexCoord).r));

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    float4 vAlbedoDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    clip(vAlbedoDesc.a - 0.1f);

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vMtrlPropertyDesc = g_MtrlProperty.Sample(LinearSampler, In.vTexCoord);
    float4 vPixelWorldPos = TransformNDCToWorld(In.vTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv);

    float3 vLightDir = (g_vLightPos.xyz - vPixelWorldPos.xyz);
    float fLightDistance = length(vLightDir);
    float fAttenuation = 1.0 / (g_vAttenuation.x + g_vAttenuation.y * fLightDistance + g_vAttenuation.z * fLightDistance * fLightDistance);
    vLightDir = normalize(vLightDir);

    fAttenuation *= step(fLightDistance, g_fLightRange);
    
    float3 vRadiance = g_vLightDiffuse.xyz * fAttenuation;
    float3 vViewDir = normalize(g_vCamPosition.xyz - vPixelWorldPos.xyz);
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float3 vAlbedo = pow(max(0.f, vAlbedoDesc.rgb), 2.2f);
    float fMetalic = vMtrlPropertyDesc.r;
    float fRoughness = clamp(1.f - vMtrlPropertyDesc.g, 0.3f, 1.f);
    
    Out = ComputePBRLighring(vRadiance, vLightDir, vViewDir, vNormal, vAlbedo, fMetalic, fRoughness);

    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    float4 vAlbedoDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    clip(vAlbedoDesc.a - 0.1f);

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vMtrlPropertyDesc = g_MtrlProperty.Sample(LinearSampler, In.vTexCoord);
    float4 vPixelWorldPos = TransformNDCToWorld(In.vTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv);

    float3 vLightDir = (g_vLightPos.xyz - vPixelWorldPos.xyz);
    float fLightDistance = length(vLightDir);
    float fAttenuation = 1.0 / (g_vAttenuation.x + g_vAttenuation.y * fLightDistance + g_vAttenuation.z * fLightDistance * fLightDistance);
    vLightDir = normalize(vLightDir);

    float fCosDir = dot(normalize(vLightDir), normalize(g_vLightDir.xyz));
    float fIntensityByCutOff = saturate((fCosDir - g_vCosCutOff.y) / (g_vCosCutOff.x - g_vCosCutOff.y));
    clip(fIntensityByCutOff);

    fAttenuation *= step(fLightDistance, g_fLightRange) * fIntensityByCutOff;
    float3 vRadiance = g_vLightDiffuse.xyz * fAttenuation;
    float3 vViewDir = normalize(g_vCamPosition.xyz - vPixelWorldPos.xyz);
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float3 vAlbedo = pow(max(0.f, vAlbedoDesc.rgb), 2.2f);
    float fMetalic = vMtrlPropertyDesc.r;
    float fRoughness = clamp(1.f - vMtrlPropertyDesc.g, 0.3f, 1.f);
    
    Out = ComputePBRLighring(vRadiance, vLightDir, vViewDir, vNormal, vAlbedo, fMetalic, fRoughness);
    

    //float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    //float4 vNormal = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f));

    //float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexCoord);

    //float4 vWorldPos = TransformNDCToWorld(In.vTexCoord, vDepthDesc, g_ProjMatrixInv, g_ViewMatrixInv);

    //float4 vLightDir = vWorldPos - g_vLightPos;
    //float fLength = length(vLightDir);

    //float fCosDir = dot(normalize(vLightDir), normalize(g_vLightDir));

    //float fIntensityByCutOff = saturate((fCosDir - g_vCosCutOff.y) / (g_vCosCutOff.x - g_vCosCutOff.y));
    //clip(fIntensityByCutOff);
    ////if (fIntensityByCutOff < 0.f)
    ////{
    ////    return Out;
    ////}

    //float fAttenuation = pow(saturate((g_fLightRange - fLength) / g_fLightRange), 0.5f);
    
    //float4 vShade = saturate(max(dot(normalize(vLightDir * -1.f), vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);

    ////Out.vShade = g_vLightDiffuse * vShade * fAttenuation;
    //Out.vShade = g_vLightDiffuse * vShade * fAttenuation * fIntensityByCutOff;

    //float4 vReflect = reflect(normalize(vLightDir), normalize(vNormal));
    //float4 vLook = vWorldPos - g_vCamPosition;

    //Out.vAmbient = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect * -1.f), normalize(vLook)), 0.f), 50.f) * fAttenuation * fIntensityByCutOff;

    return Out;
}

technique11 DefaultTechnique
{
    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }
    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }
    pass Spot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }
}
