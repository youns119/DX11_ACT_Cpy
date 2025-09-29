#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"


float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// float3�� ������ ����
float4 g_vLightDir;
float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_MetalicTexture;
Texture2D g_SmoothnessTexture;
Texture2D g_EmissiveMaskTexture;

float4 g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlBaseColor = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vEmissionColor = float4(0.f, 0.f, 0.f, 1.f);

float4 g_vCamPosition;

uint g_iObjectID;

float4x4 g_LightViewMatrices[3];
float4x4 g_LightProjMatrices[3];

float2 g_vTexCoordOffset; // �̵��� UV ������

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

    float4x4 matWV;
    float4x4 matWVP;
    
    float4x4 TransformationMatrix = In.TransformMatrix;
    float4 vPosition = mul(float4(In.vPosition, 1.0f), TransformationMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.0f), TransformationMatrix);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

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

VS_OUT VS_MAIN_CROSSPLANE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float4x4 matWV;
    float4x4 matWVP;
    
    float4x4 TransformationMatrix = In.TransformMatrix;
    float4 vPosition = mul(float4(In.vPosition, 1.0f), TransformationMatrix);
    //float4 vNormal = mul(float4(In.vNormal, 0.0f), TransformationMatrix);
    float3 normal1 = float3(0, 1, 0); // ù ��° ���� ����
    float3 normal2 = float3(1, 0, 0); // �� ��° ���� ����

    float3 vNormal = normalize(normal1 + normal2);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    // ��� ���� ����
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(float4(0.f, 1.f, 0.f, 0.f),  g_WorldMatrix));
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPosition = mul(vPosition, g_WorldMatrix);
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

    return Out;
}



//// Ǯ ��鸮�� ������Ʈ�� ���̴� ����
//// ��鸮�� ȿ���� ���� ������ sine ��� ����
//[maxvertexcount(3)]
//void GS_WobbleTriangle(triangle VS_OUT In[3], inout TriangleStream<VS_OUT> TriStream)
//{
//    // �ð� ��� ��鸲 ��
//    float time = 0.016f; // g_fTime�� �۷ι� �ð� ���Դϴ�.

//    // Ǯ ��鸲 ���� (������ ����)
//    float wobbleIntensity = 0.1f;

//    // ��鸲 �ֱ� (Ǯ�� ��鸲 �ӵ�)
//    float wobbleSpeed = 2.0f;

//    // �� ������ ���� ��鸲�� ����
//    for (int i = 0; i < 3; i++)
//    {
//        // �� ������ ��ġ�� ��鸲 ����
//        float3 offset = float3(
//            sin(time + In[i].Position.x * wobbleSpeed) * wobbleIntensity, // X���� ��鸲
//            0.0f, // Y���� ��鸲 ����
//            cos(time + In[i].Position.z * wobbleSpeed) * wobbleIntensity // Z���� ��鸲
//        );

//        // ������ ��ġ�� ��鸰 ������ ������Ʈ
//        In[i].Position.xyz += offset;

//        // ������ ������ ���
//        TriStream.Append(In[i]);
//    }

//    TriStream.RestartStrip();
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
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.iObjectID = g_iObjectID;

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

PS_OUT PS_MAIN_TestMapping(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.zyx * 0.5f + 0.5f, 0.f);
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
    
    float4 vMtrlDiffuse = g_vMtrlBaseColor;

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.w, 1.f, 0.f);
    //Out.iObjectID = g_iObjectID;

    return Out;
}

PS_OUT_FOWARD PS_MAIN_FOWARD(PS_IN In)
{
    PS_OUT_FOWARD Out = (PS_OUT_FOWARD) 0;
    
    float4 vMaterialDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (vMaterialDiffuse.a < 0.1f)
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
    pass Foward
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_FOWARD();
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
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN_NonNormalMapping();
    }

    pass CrossPlanePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_CROSSPLANE();
        GeometryShader = NULL;
        //GeometryShader = compile gs_5_0 GS_MAIN(); // debugging
        PixelShader = compile ps_5_0 PS_MAIN();
        //PixelShader = compile ps_5_0 PS_MAIN_TestMapping();
    }
}