#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

/* 이 메시(정점들에)에 영향을 주는 뼈 행렬들 */
float4x4 g_BoneMatrices[256];

// float3로 변경할 예전
float4 g_vLightDir;
float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_NoiseTexture;
Texture2D g_MetalicTexture;

Texture2D g_SmoothnessTexture;
Texture2D g_EmissiveMaskTexture;

float4 g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlBaseColor = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vEmissionColor = float4(0.f, 0.f, 0.f, 1.f);

float4 g_vCamPosition;

float4x4 g_LightViewMatrices[3];
float4x4 g_LightProjMatrices[3];

float g_fDissolveRate;
float g_fSwellAmount;
float g_fDissolveDarknessRate;  

float g_fDamagedIntensity;

float g_fRimPower = 3.f;
float4 g_vRimColor = float4(1.f, 1.f, 1.f, 1.f);

uint g_iObjectID = 0;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float3 vTangent : TANGENT0;

    uint4 vBlendIndex : BLENDINDEX0;
    float4 vBlendWeight : BLENDWEIGHT0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPosition : POSITION1;
    float4 vProjPosition : POSITION2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 matWV;
    float4x4 matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fBlendWeightW = 1 - In.vBlendWeight.x - In.vBlendWeight.y - In.vBlendWeight.z;
    
    float4x4 BoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * float(In.vBlendWeight.x) +
        g_BoneMatrices[In.vBlendIndex.y] * float(In.vBlendWeight.y) +
        g_BoneMatrices[In.vBlendIndex.z] * float(In.vBlendWeight.z) +
        g_BoneMatrices[In.vBlendIndex.w] * fBlendWeightW;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
        
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPosition = mul(vPosition, g_WorldMatrix);
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);
    //Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f); // cross( normal, normal ) = normal

    return Out;
}


VS_OUT VS_SWELL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 matWV;
    float4x4 matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fBlendWeightW = 1 - In.vBlendWeight.x - In.vBlendWeight.y - In.vBlendWeight.z;
    
    float4x4 BoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * float(In.vBlendWeight.x) +
        g_BoneMatrices[In.vBlendIndex.y] * float(In.vBlendWeight.y) +
        g_BoneMatrices[In.vBlendIndex.z] * float(In.vBlendWeight.z) +
        g_BoneMatrices[In.vBlendIndex.w] * fBlendWeightW;                
    
    float displacementAmount = 1;
    if (g_fDissolveRate < 1.8f)
        displacementAmount = (sin(g_fSwellAmount) * ((In.vTexCoord.x % 0.2f) + (In.vTexCoord.y % 0.2f)) + 1.f);
      
    float4 vPosition = mul(float4(In.vPosition * displacementAmount, 1.f), BoneMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPosition = mul(vPosition, g_WorldMatrix);
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);
    //Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f); // cross( normal, normal ) = normal

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPosition : POSITION1;
    float4 vProjPosition : POSITION2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vMaterialProperty : SV_Target3; // emissive intensity, smoothness, metalic, 
    float4 vEmissionColor : SV_Target4;
    uint iObjectID : SV_TARGET5;
};

float4 Sample_Normal(SamplerState Sampler, float2 vTexCoord)
{
    float4 vNormal = g_NormalTexture.Sample(Sampler, vTexCoord).bgra;

    return vNormal;
}
float4 Sample_MtrlProperty(SamplerState Sampler, float2 vTexCoord)
{
    float4 vMtrlProperty = 0.f;
    vMtrlProperty.r = g_MetalicTexture.Sample(LinearSampler, vTexCoord).r;
    vMtrlProperty.g = g_SmoothnessTexture.Sample(LinearSampler, vTexCoord).g;
    vMtrlProperty.b = g_EmissiveMaskTexture.Sample(LinearSampler, vTexCoord).b;

    return vMtrlProperty;
}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    clip(vDiffuse.a - 0.1f);

    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord);
    float4 vMtrlProperty = Sample_MtrlProperty(LinearSampler, In.vTexCoord);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */

    Out.vDiffuse = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.vMaterialProperty = vMtrlProperty;
    Out.vEmissionColor = g_vEmissionColor;
    Out.iObjectID = g_iObjectID;

    return Out;
}

PS_OUT PS_MAIN_NonNormalMapping(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.iObjectID = g_iObjectID;

    return Out;
}

struct PS_OUT_FOWARD
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_FOWARD PS_MAIN_FOWARD(PS_IN In)
{
    PS_OUT_FOWARD Out = (PS_OUT_FOWARD) 0;
    
    float4 vMaterialDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (vMaterialDiffuse.a < 0.3f)
        discard;
    
    float4 vShade = max(dot(normalize(In.vNormal), normalize(g_vLightDir) * -1.f), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);

    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    float4 vLook = In.vWorldPosition - g_vCamPosition;

    float fScecular = pow(max(dot(normalize(vLook), normalize(vReflect) * -1.f), 0.f), 50.f);

    Out.vColor = g_vLightDiffuse * vMaterialDiffuse * saturate(vShade) + fScecular * g_vLightSpecular * g_vMtrlSpecular;

    return Out;
}

PS_OUT PS_MAIN_BaseColor(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_vMtrlBaseColor;
    clip(vDiffuse.a - 0.1f);

    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord);
    float4 vMtrlProperty = Sample_MtrlProperty(LinearSampler, In.vTexCoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */

    Out.vDiffuse = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.vMaterialProperty = vMtrlProperty;
    Out.vEmissionColor = g_vEmissionColor;
    Out.iObjectID = g_iObjectID;

    return Out;
}

PS_OUT PS_MAIN_Dissolve(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord).rgba * g_fDissolveDarknessRate;
    float4 vDesolve = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord).r;
    float4 vMtrlProperty = Sample_MtrlProperty(LinearSampler, In.vTexCoord);
    
    if (g_fDissolveRate < 2.f)   
        vDiffuse.rgb = 0.f;            

    if (vDesolve.r > g_fDissolveRate)
        discard;
     
    clip(vDiffuse.a - 0.1f);

    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */

    Out.vDiffuse = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.vMaterialProperty = vMtrlProperty;
    Out.vEmissionColor = g_vEmissionColor;
    Out.iObjectID = g_iObjectID;

    return Out;
}

PS_OUT PS_MAIN_MonsterDamaged(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vMtrlProperty = Sample_MtrlProperty(LinearSampler, In.vTexCoord);
    
    clip(vDiffuse.a - 0.1f);

    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    
    float3 flashColor = lerp(vDiffuse.rgb, float3(1.0f, 1.0f, 1.0f), g_fDamagedIntensity);
    
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */
    vDiffuse.rgb = flashColor;
    vDiffuse.b = vDiffuse.b * 0.5f;    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.vMaterialProperty = vMtrlProperty;
    Out.vEmissionColor = g_vEmissionColor;
    Out.iObjectID = g_iObjectID;
   
    return Out;
}

PS_OUT PS_MAIN_RimLight(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord).bgra;
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, NormalWorldMatrix));
    
    float3 vView = normalize(g_vCamPosition.xyz - In.vWorldPosition.xyz);
    
    float fRim = 1.0 - dot(vNormal, vView);
    fRim = pow(fRim, g_fRimPower);
    float3 fRimLight = g_vRimColor.rgb * fRim;
    //float fRimLight = fRim * 0.5f;
    
    Out.vDiffuse = vDiffuse;
    if(fRim < 0.5f)
        Out.vDiffuse.rgb += fRimLight;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.iObjectID = g_iObjectID;
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : POSITION0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

    float fBlendWeightW = 1 - In.vBlendWeight.x - In.vBlendWeight.y - In.vBlendWeight.z;
    
    float4x4 BoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * float(In.vBlendWeight.x) +
        g_BoneMatrices[In.vBlendIndex.y] * float(In.vBlendWeight.y) +
        g_BoneMatrices[In.vBlendIndex.z] * float(In.vBlendWeight.z) +
        g_BoneMatrices[In.vBlendIndex.w] * fBlendWeightW;

    float4 vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
        
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    return Out;
}

struct GS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vViewPosition : POSITION0;
    uint iArrayIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9 /* 3*3 */)]
void GS_MAIN_SHADOW(triangle VS_OUT_SHADOW In[3], inout TriangleStream<GS_OUT_SHADOW> DataStream)
{
    GS_OUT_SHADOW Out[3];

    for (uint i = 0; i < 3; ++i) // num cascade
    {
        for (uint j = 0; j < 3; ++j)
        {
            Out[j].vViewPosition = mul(In[j].vPosition, g_LightViewMatrices[i]);
            Out[j].vPosition = mul(Out[j].vViewPosition, g_LightProjMatrices[i]);
            Out[j].iArrayIndex = i;
            DataStream.Append(Out[j]);
        }
        DataStream.RestartStrip();
    }
}

struct PS_OUT_SHADOW
{
    float1 vShadow : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(GS_OUT_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    //Out.vShadow = vector(In.vViewPosition.z, 0.f, 1.f, 0.f);
    Out.vShadow = saturate(In.vViewPosition.z / 1000.f);
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

    pass NonNormalMapping
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_NonNormalMapping();
    }

    pass FOWARDPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOWARD();
    }

    pass ShadowPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = compile gs_5_0 GS_MAIN_SHADOW();
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass BaseColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BaseColor();
    }

    pass DissolvePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    }

    pass EnemyDamagedPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MonsterDamaged();
    }

    pass RimLightPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RimLight();
    }

    pass NonCullingPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
