#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"


float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

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

float3 g_vBendFactor; // 꺾임 정도( 흔들림 세기 방향 ) 

uint g_iObjectID;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
    row_major float4x4 TransformMatrix : WORLD;
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
    
    float4x4 TransformationMatrix = In.TransformMatrix;
    float4 vPosition = mul(float4(In.vPosition, 1.0f), TransformationMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.0f), TransformationMatrix);
    
    float2 vWindResult = (vPosition.y * g_vBendFactor.xz);
    
    vPosition = mul(vPosition, g_WorldMatrix);
    vPosition.xz += vWindResult.xy;
    float4 vWorldPos = vPosition;
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPosition = vWorldPos;
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);
    //Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f); // cross( normal, normal ) = normal
    return Out;
}

VS_OUT VS_MAIN_CROSSPLANE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float4x4 TransformationMatrix = In.TransformMatrix;
    float4 vPosition = mul(float4(In.vPosition, 1.0f), TransformationMatrix);
    
    // 정점 노말은 항상 위쪽
    float3 vNormal = float3(0.f, 1.f, 0.f);

    float2 vWindResult = (vPosition.y * g_vBendFactor.xz);
    
    vPosition = mul(vPosition, g_WorldMatrix);
    vPosition.xz += vWindResult.xy;
    float4 vWorldPos = vPosition;

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(vNormal, 0.f), g_WorldMatrix));
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPosition = vWorldPos;
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

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
    float4 vDiffuse : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vDepth : SV_Target2;
    float4 vMaterialProperty : SV_Target3; // emissive intensity, smoothness, metalic, 
    float4 vEmissionColor : SV_Target4;
    uint iObjectID : SV_Target5;
};

struct PS_OUT_NON_LIGHT
{
    /* 사실 똑같아요 의미를 위해 변수명만 달라요 */
    float4 vFinal : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vDepth : SV_Target2;
    float4 vMaterialProperty : SV_Target3; // emissive intensity, smoothness, metalic, 
    float4 vEmissionColor : SV_Target4;
    uint iObjectID : SV_Target5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    clip(vMtrlDiffuse.a - 0.1f);

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord).bgra;

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.iObjectID = g_iObjectID;

    return Out;
}

PS_OUT PS_MAIN_CROSSPLANE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 알파 컷
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    clip(vMtrlDiffuse.a - 0.1f);

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord).bgra;
    float3 vNormalTS = normalize(vNormalDesc.xyz * 2.f - 1.f); // 탄젠트 스펭스

    // 탄젠트 -> 월드
    float3x3 TBN = float3x3(
        normalize(In.vTangent.xyz),
        normalize(In.vBinormal.xyz),
        normalize(In.vNormal.xyz)
    );

    float3 vNormalWorld = normalize(mul(vNormalTS, TBN));

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormalWorld * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);

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
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.iObjectID = g_iObjectID;

    return Out;
}


PS_OUT PS_MAIN_UV_WRAPPING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 TexCoord = In.vTexCoord * 10.f;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, TexCoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, TexCoord);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; /* Tangent Space */
    float3x3 NormalWorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); /* Tangent To World */
    vNormal = normalize(mul(vNormal, NormalWorldMatrix)); /* World Space */

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.vPickDepth = float2(Out.vDepth.x, 1.f);
    //Out.iObjectID = g_iObjectID;

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


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass BaseColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_BaseColor();
    }

    pass UV_Wrapping
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_UV_WRAPPING();
    }

    pass UV_Wrapping_ExceptNormal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_UV_WRAPPING_EXCEPTNORMAL();
    }
    pass BaseColorNonCullingPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_BaseColor();
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

    pass CrossPlanePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_CROSSPLANE();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        //PixelShader = compile ps_5_0 PS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_CROSSPLANE();
    }
}