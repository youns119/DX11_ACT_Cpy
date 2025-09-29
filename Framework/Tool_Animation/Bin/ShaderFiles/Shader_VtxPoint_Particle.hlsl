
#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D   g_Texture;
Texture2D   g_MaskTexture;
Texture2D   g_NoiseTexture;
Texture2D   g_DissolveTexture;
Texture2D   g_DistortionTexture;
Texture2D   g_EmissiveTexture;

float4      g_vCamPosition;
float3      g_vMulColor;
float3      g_vAddColor;

float       g_fFinalSize;

int3        g_vNumAtlas; // 가로, 세로, 전체 개수
float2      g_vUVInterval;
float       g_fTimeDelta;
int         g_iShadingOption;
float2      g_vDissolveStart;

bool        g_bIsLoop;

Texture2D   g_DepthTexture;

float BOITWeight(in float z, in float zw, in float a)
{
    return pow(a, 2.f) / ((a + 0.0001f) * 10.f + (z + 0.0001f) * 0.01f);
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float fPSize : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
    float fSpeed : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float fPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vRight : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
	/* 로컬스페이스내에서 움직임을 가져간다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fPSize = length(In.TransformMatrix._11_12_13);
    Out.vLifeTime = In.vLifeTime;
    Out.vRight = In.TransformMatrix._11_12_13_14;
    return Out;
}
    
struct GS_IN
{
    float4 vPosition : POSITION;
    float fPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vRight : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float fLifeLerp : TEXCOORD3;
};


struct GS_OUT_UV
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float2 vAtlasUVLT : TEXCOORD3;
    float fLifeLerp : TEXCOORD4;
};

/* Geometry Shader == 기하셰이더 */
/* */
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float fLifeTimeLerp = In[0].vLifeTime.y / In[0].vLifeTime.x;
    float fSizeLerp = (1 - fLifeTimeLerp) + (fLifeTimeLerp * g_fFinalSize);
    float fSize = (In[0].fPSize * 0.5f * fSizeLerp);
    float3 vLook = normalize(g_vCamPosition.xyz - In[0].vPosition.xyz);
    float3 vUp = float3(0.f, 1.f, 0.f);
    float3 vRight = float3(1.f, 0.f, 0.f);
    
    if (In[0].vRight.g == 0.f && In[0].vRight.b == 0.f)
    {
        vRight = normalize(cross(vUp, vLook)) * fSize;
        vUp = normalize(cross(vLook, vRight)) * fSize;
    }
    else
    {
        vUp = normalize(cross(vLook, In[0].vRight.xyz)) * fSize;
        vRight = normalize(cross(vUp, vLook)) * fSize;
    }
    

    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
	
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vProjPos = Out[0].vPosition;
    Out[0].fLifeLerp = fLifeTimeLerp;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vProjPos = Out[1].vPosition;
    Out[1].fLifeLerp = fLifeTimeLerp;

    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vProjPos = Out[2].vPosition;
    Out[2].fLifeLerp = fLifeTimeLerp;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = Out[3].vPosition;
    Out[3].fLifeLerp = fLifeTimeLerp;
	
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
    DataStream.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_UV(point GS_IN In[1], inout TriangleStream<GS_OUT_UV> DataStream)
{
    GS_OUT_UV Out[4];

    float fLifeTimeLerp = In[0].vLifeTime.y / In[0].vLifeTime.x;
    float fSizeLerp = (1 - fLifeTimeLerp) + (fLifeTimeLerp * g_fFinalSize);
    float3 vLook = g_vCamPosition.xyz - In[0].vPosition.xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * (In[0].fPSize * 0.5f * fSizeLerp);
    float3 vUp = normalize(cross(vLook, vRight)) * (In[0].fPSize * 0.5f * fSizeLerp);

    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
	
    uint iIndex = fLifeTimeLerp * g_vNumAtlas.z;
    uint iIndexX = iIndex % g_vNumAtlas.x;
    uint iIndexY = iIndex / g_vNumAtlas.x;
    float2 vAtlasUVLT = float2(iIndexX * g_vUVInterval.x, iIndexY * g_vUVInterval.y);
    float2 vAtlasUVRB = float2((iIndexX + 1) * g_vUVInterval.x, (iIndexY + 1) * g_vUVInterval.y);

    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(vAtlasUVLT.x, vAtlasUVLT.y);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].fLifeLerp = fLifeTimeLerp;
    Out[0].vProjPos = Out[0].vPosition;
    Out[0].vAtlasUVLT = vAtlasUVLT;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(vAtlasUVRB.x, vAtlasUVLT.y);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].fLifeLerp = fLifeTimeLerp;
    Out[1].vProjPos = Out[1].vPosition;
    Out[1].vAtlasUVLT = vAtlasUVLT;
    
    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(vAtlasUVRB.x, vAtlasUVRB.y);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].fLifeLerp = fLifeTimeLerp;
    Out[2].vProjPos = Out[2].vPosition;
    Out[2].vAtlasUVLT = vAtlasUVLT;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(vAtlasUVLT.x, vAtlasUVRB.y);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].fLifeLerp = fLifeTimeLerp;
    Out[3].vProjPos = Out[3].vPosition;
    Out[3].vAtlasUVLT = vAtlasUVLT;
	
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
    DataStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float fLifeLerp : TEXCOORD3;
};

struct PS_IN_UV
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float2 vAtlasUVLT : TEXCOORD3;
    float fLifeLerp : TEXCOORD4;
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
    float4 vRevealage : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vDiffuse.a <= 0.2f)
        discard;
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
	
    Out.vDiffuse.rgb *= g_vMulColor;
    Out.vDiffuse.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
        Out.vDiffuse.a = Out.vDiffuse.a * vMaskDesc.r;
        
        if (Out.vDiffuse.a <= 0.2f)
            discard;
    }

    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 2)
    {
        Out.vDiffuse.a *= 1.f - In.fLifeLerp;
        if (Out.vDiffuse.a <= 0.2f)
            discard;
        
        if (In.fLifeLerp >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
            if (vDissolveDesc.r < (In.fLifeLerp - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    if (g_iShadingOption & 2 << 3)
    {
        float4 fEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
        Out.vDiffuse.rgb += fEmissionDesc.rgb;
    }
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_UV(PS_IN_UV In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
	   
    if (Out.vDiffuse.a <= 0.2f)
        discard;
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    Out.vDiffuse.rgb *= g_vMulColor;
    Out.vDiffuse.rgb += g_vAddColor;
    
    float2 vTexcoord;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);

    if (g_vNumAtlas.z == 1)
        vTexcoord = In.vTexcoord;
    else
        vTexcoord = float2((In.vTexcoord.x - In.vAtlasUVLT.x) * g_vNumAtlas.x, (In.vTexcoord.y - In.vAtlasUVLT.y) * g_vNumAtlas.y);
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        // 잘라낸 uv(In.vTexcoord)를 하나의 큰 uv로 변경 필요
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, vTexcoord);
        Out.vDiffuse.a = Out.vDiffuse.a * vMaskDesc.r;
        
        if (Out.vDiffuse.a <= 0.2f)
            discard;
    }
    //else
    //    Out.vDiffuse.a = Out.vDiffuse.a * (vDepthDesc.y - In.vProjPos.w) * 0.5f;
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 2)
    {
        if (In.fLifeLerp >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_DissolveTexture.Sample(LinearSampler, vTexcoord);
            if (vDissolveDesc.r < (In.fLifeLerp - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 3)
    {
        float4 fEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, vTexcoord);
        Out.vDiffuse.rgb += fEmissionDesc.rgb;
    }
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BLENDACC PS_MAIN_ACC(PS_IN In)
{
    PS_OUT_BLENDACC Out = (PS_OUT_BLENDACC) 0;
    
    Out.vDiffuseAcc = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vDiffuseAcc.a <= 0.2f)
        discard;
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
	
    Out.vDiffuseAcc.rgb *= g_vMulColor;
    Out.vDiffuseAcc.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
        Out.vDiffuseAcc.a = Out.vDiffuseAcc.a * vMaskDesc.r;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 2)
    {
        if (In.fLifeLerp >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
            if (vDissolveDesc.r < (In.fLifeLerp - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    if (g_iShadingOption & 2 << 3)
    {
        float4 fEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
        Out.vDiffuseAcc.rgb += fEmissionDesc.rgb;
    }
    float fWeight = BOITWeight(In.vProjPos.z, In.vProjPos.z / In.vProjPos.w, Out.vDiffuseAcc.a);
    Out.vDiffuseAcc = float4(Out.vDiffuseAcc.r * fWeight, Out.vDiffuseAcc.g * fWeight, Out.vDiffuseAcc.b * fWeight, Out.vDiffuseAcc.a);
    
    return Out;
}

PS_OUT_BLENDACC PS_MAIN_ACC_UV(PS_IN_UV In)
{
    PS_OUT_BLENDACC Out = (PS_OUT_BLENDACC) 0;
    
    Out.vDiffuseAcc = g_Texture.Sample(LinearSampler, In.vTexcoord);
	   
    if (Out.vDiffuseAcc.a <= 0.2f)
        discard;
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    Out.vDiffuseAcc.rgb *= g_vMulColor;
    Out.vDiffuseAcc.rgb += g_vAddColor;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        // 잘라낸 uv(In.vTexcoord)를 하나의 큰 uv로 변경 필요
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, float2((In.vTexcoord.x - In.vAtlasUVLT.x) * g_vNumAtlas.x, (In.vTexcoord.y - In.vAtlasUVLT.y) * g_vNumAtlas.y));
        Out.vDiffuseAcc.a = Out.vDiffuseAcc.a * vMaskDesc.r;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 2)
    {
        if (In.fLifeLerp >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_DissolveTexture.Sample(LinearSampler, float2((In.vTexcoord.x - In.vAtlasUVLT.x) * g_vNumAtlas.x, (In.vTexcoord.y - In.vAtlasUVLT.y) * g_vNumAtlas.y));
            if (vDissolveDesc.r < (In.fLifeLerp - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    // 이미시브 적용
    if (g_iShadingOption & 2 << 3)
    {
        float4 fEmissionDesc = g_EmissiveTexture.Sample(LinearSampler, float2((In.vTexcoord.x - In.vAtlasUVLT.x) * g_vNumAtlas.x, (In.vTexcoord.y - In.vAtlasUVLT.y) * g_vNumAtlas.y));
        Out.vDiffuseAcc += fEmissionDesc;
    }
    
    float fWeight = BOITWeight(In.vProjPos.z, In.vProjPos.z / In.vProjPos.w, Out.vDiffuseAcc.a);
    Out.vDiffuseAcc = float4(Out.vDiffuseAcc.r * fWeight, Out.vDiffuseAcc.g * fWeight, Out.vDiffuseAcc.b * fWeight, Out.vDiffuseAcc.a);
    
    return Out;
}

PS_OUT_REVEAL PS_MAIN_REVEALAGE(PS_IN In)
{
    PS_OUT_REVEAL Out = (PS_OUT_REVEAL) 0;
    
    float4 vDiffuseAcc = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuseAcc.a <= 0.2f)
        discard;
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    //float2 vTexcoord;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    //vDiffuseAcc.a = vDiffuseAcc.a * (vDepthDesc.y - In.vProjPos.w) * 0.5f;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
        vDiffuseAcc.a = vDiffuseAcc.a * vMaskDesc.r;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 2)
    {
        if (In.fLifeLerp >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
            if (vDissolveDesc.r < (In.fLifeLerp - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    float fWeight = BOITWeight(In.vProjPos.z, In.vProjPos.z / In.vProjPos.w, vDiffuseAcc.a);
    Out.vRevealage.r = vDiffuseAcc.a * fWeight;
    return Out;
}

PS_OUT_REVEAL PS_MAIN_REVEALAGE_UV(PS_IN_UV In)
{
    PS_OUT_REVEAL Out = (PS_OUT_REVEAL) 0;
    
    float4 vDiffuseAcc = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
   
    //float2 vTexcoord;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    //vDiffuseAcc.a = vDiffuseAcc.a * (vDepthDesc.y - In.vProjPos.w) * 0.5f;
    
    // 마스크 적용
    if (g_iShadingOption & 1)
    {
        // 잘라낸 uv(In.vTexcoord)를 하나의 큰 uv로 변경 필요
        vector vMaskDesc = g_MaskTexture.Sample(LinearSampler, float2((In.vTexcoord.x - In.vAtlasUVLT.x) * g_vNumAtlas.x, (In.vTexcoord.y - In.vAtlasUVLT.y) * g_vNumAtlas.y));
        vDiffuseAcc.a = vDiffuseAcc.a * vMaskDesc.r;
    }
    
    // 디졸브 적용
    if (g_iShadingOption & 2 << 2)
    {
        if (In.fLifeLerp >= g_vDissolveStart.y)
        {
            vector vDissolveDesc = g_DissolveTexture.Sample(LinearSampler, float2((In.vTexcoord.x - In.vAtlasUVLT.x) * g_vNumAtlas.x, (In.vTexcoord.y - In.vAtlasUVLT.y) * g_vNumAtlas.y));
            if (vDissolveDesc.r < (In.fLifeLerp - g_vDissolveStart.y) / (1 - g_vDissolveStart.y))
                discard;
        }
    }
    
    float fWeight = BOITWeight(In.vProjPos.z, In.vProjPos.z / In.vProjPos.w, vDiffuseAcc.a) * 4.f;
    Out.vRevealage.r = vDiffuseAcc.a * fWeight;
    if (false == g_bIsLoop)
        Out.vRevealage.r *= (1.f - pow(In.fLifeLerp, 2.f));

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultParticlePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass uvSpritePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UV();
        PixelShader = compile ps_5_0 PS_MAIN_UV();
    }

    pass BlendAccPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_BlendAcc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_ACC();
    }

    pass BlendAccPass_UV
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_BlendAcc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UV();
        PixelShader = compile ps_5_0 PS_MAIN_ACC_UV();
    }

    pass RevealagePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_Revealage, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_REVEALAGE();
    }

    pass RevealagePass_UV
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_Revealage, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UV();
        PixelShader = compile ps_5_0 PS_MAIN_REVEALAGE_UV();
    }


    pass uvSpriteNonLightPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UV();
        PixelShader = compile ps_5_0 PS_MAIN_UV();
    }
}
