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
float4 g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlBaseColor = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

uint g_iObjectID;

struct VS_IN
{
    float3      vPosition : POSITION;
    float3      vNormal : NORMAL0;
    float2      vTexCoord : TEXCOORD0;
    float3      vTangent : TANGENT0;

    uint4       vBlendIndex : BLENDINDEX0;
    float4      vBlendWeight : BLENDWEIGHT0;
};

struct VS_OUT
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL0;
    float2      vTexCoord : TEXCOORD0;
    float4      vWorldPosition : POSITION1;
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
    uint iObjectID : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 0.f, 0.f);
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
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 0.f, 0.f);
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
    
    float4 vMtrlDiffuse = g_vMtrlBaseColor;

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 0.f, 0.f);
    Out.iObjectID = g_iObjectID;

    return Out;
}


struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPosition : POSITION0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

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
    Out.vProjPosition = Out.vPosition;
    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vShadow : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(VS_OUT_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vShadow = vector(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 0.f, 0.f);

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
        GeometryShader = NULL;
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
}