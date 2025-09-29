#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"


float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// float3로 변경할 예전
float4 g_vLightDir;
float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_MetalicTexture;
Texture2D g_SmoothnessTexture;
Texture2D g_EmissiveMaskTexture;
Texture2D g_NoiseTexture;

float4 g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlBaseColor = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vEmissionColor = float4(0.f, 0.f, 0.f, 1.f);

float4 g_vCamPosition;

uint g_iObjectID = 0;

float4x4 g_LightViewMatrices[3];
float4x4 g_LightProjMatrices[3];

float2 g_vTexCoordOffset; // 이동할 UV 오프셋

float g_fDissolveRate;
float g_fSwellAmount;
float g_fDissolveDarknessRate;

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
    //float4 vPosition : POSITION;
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

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);
    //Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f); // cross( normal, normal ) = normal
    return Out;
}

//[maxvertexcount(3)]
//void GS_MAIN(triangle VS_OUT In[3], inout TriangleStream<VS_OUT> DataStream)
//{

//    //Out[0].vPosition.y += 2.f;
//    //Out[1].vPosition.y += 2.f;
//    //Out[2].vPosition.y += 2.f;
  
//    DataStream.Append(In[0]);
//    DataStream.Append(In[1]);
//    DataStream.Append(In[2]);
//}

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
    float4 vDiffuse : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vDepth : SV_Target2;
    float4 vMaterialProperty : SV_Target3; // emissive intensity, smoothness, metalic, 
    float4 vEmissionColor : SV_Target4;
    uint iObjectID : SV_Target5;
};

float4 Sample_Normal(SamplerState Sampler, float2 vTexCoord)
{
    return g_NormalTexture.Sample(Sampler, vTexCoord).bgra;
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
    
    float3 vNormal = Get_Normal_Red(g_NormalTexture, LinearSampler, In.vTexCoord);
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

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (vMtrlDiffuse.a < 0.1f)
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

PS_OUT PS_MAIN_BaseColor(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_vMtrlBaseColor;
    clip(vDiffuse.a - 0.1f);

    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord);
    float4 vMtrlProperty = Sample_MtrlProperty(LinearSampler, In.vTexCoord);
    
    float3 vNormal = Get_Normal_Red(g_NormalTexture, LinearSampler, In.vTexCoord);
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

PS_OUT PS_MAIN_BaseColorNonNormal(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vMtrlDiffuse = g_vMtrlBaseColor;

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.iObjectID = g_iObjectID;

    return Out;
}


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

PS_OUT PS_MAIN_UV_WRAPPING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 TexCoord = In.vTexCoord * 10.f;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, TexCoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.vPickDepth = float2(Out.vDepth.x, 1.f);
    Out.iObjectID = g_iObjectID;

    return Out;
}

PS_OUT PS_MAIN_UV_WRAPPING_EXCEPTNORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 TexCoord = In.vTexCoord * 10.f;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, TexCoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.vPickDepth = float2(Out.vDepth.x, 1.f);
    Out.iObjectID = g_iObjectID;

    return Out;
}

PS_OUT PS_MAIN_BASECOLOR_UV_TRANSFORM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexCoord = In.vTexCoord + g_vTexCoordOffset;
    float4 vDiffuse = g_vMtrlBaseColor;
    clip(vDiffuse.a - 0.1f);
    

    
    float4 vNormalDesc = Sample_Normal(LinearSampler, vTexCoord);
    float4 vMtrlProperty = Sample_MtrlProperty(LinearSampler, vTexCoord);
    
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

    float4 vMtrlDiffuse = g_vMtrlBaseColor;

    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord).rgba;
    float4 vDesolve = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord).r;
    float4 vNormalDesc = Sample_Normal(LinearSampler, In.vTexCoord);
    float4 vMtrlProperty = Sample_MtrlProperty(LinearSampler, In.vTexCoord);

    if (vDesolve.r > g_fDissolveRate)
        discard;

    if (vDesolve.r > g_fDissolveRate + 0.2f)
        vDiffuse.rgb = float3(1.0f, 1.0f, 1.0f);
    
    clip(vDiffuse.a - 0.1f);
    
    float3 lightPurple = float3(0.8f, 0.6f, 1.0f);
    //float3 white = float3(1.0f, 1.0f, 1.0f);

    float3 interpolatedColor = lerp(vDiffuse.rgb, lightPurple, g_fDissolveDarknessRate);
    
    vDiffuse.rgb *= interpolatedColor;

    float3 vNormal = Get_Normal_Red(g_NormalTexture, LinearSampler, In.vTexCoord);
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, NormalWorldMatrix));

    Out.vDiffuse = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    Out.vMaterialProperty = vMtrlProperty;
    Out.vEmissionColor = g_vEmissionColor;
    Out.iObjectID = g_iObjectID;
        
    return Out;
}


/* for.shadow */
struct VS_OUT_SHADOW
{
    float4 vPosition : POSITION0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
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
    float fShadow : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(GS_OUT_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.fShadow = saturate(In.vViewPosition.z / 1000.f);
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass BaseColorNoneNormalPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_BaseColorNonNormal();
    }
    pass NonNormalMapping // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_NonNormalMapping();
    }
    pass Foward // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_FOWARD();
    }
    pass UV_Wrapping // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_UV_WRAPPING();
    }
    pass UV_Wrapping_ExceptNormal //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_UV_WRAPPING_EXCEPTNORMAL();
    }

    pass ShadowPass //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = compile gs_5_0 GS_MAIN_SHADOW();
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass BaseColor //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_BaseColor();
    }

    pass BaseColorUVTransform //8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_BASECOLOR_UV_TRANSFORM();
    }

    pass NonCulling //9
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Dissolve //10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    }
}