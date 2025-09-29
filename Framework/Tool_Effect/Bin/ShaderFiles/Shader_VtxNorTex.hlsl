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
Texture2D g_DiffuseTexture[2];

float4 g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

float3 g_vBrushPosition = float3(20.f, 0.f, 20.f);
float g_fBrushRange = 10.f;

uint g_iObjectID;

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

    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPosition : POSITION0;
    float4 vProjPosition : POSITION1;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    //float2 vPickDepth : SV_Target3;
    uint iObjectID : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse[2];
    for (uint i = 0; i < 2; ++i)
        vDiffuse[i] = g_DiffuseTexture[i].Sample(LinearSampler, In.vTexCoord * 30.f);

    float4 vMask = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);

    float4 vMaterialDiffuse = vDiffuse[0] * vMask + vDiffuse[1] * (1 - vMask);
    
    //float4 vShade = max(dot(normalize(In.vNormal), normalize(g_vLightDir) * -1.f), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);

    //float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    //float4 vLook = In.vWorldPosition - g_vCamPosition;

    //float fScecular = pow(max(dot(normalize(vLook), normalize(vReflect) * -1.f), 0.f), 50.f);

    //Out.vColor = g_vLightDiffuse * vMaterialDiffuse * saturate(vShade) + fScecular * g_vLightSpecular * g_vMtrlSpecular;

    Out.vDiffuse = vMaterialDiffuse;

    /* 0 ~ 1 */   /* -1 ~ 1 */
	/* -1 -> 0, 1 -> 1 */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 0.f, 0.f);
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
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 0.f, 0.f);
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