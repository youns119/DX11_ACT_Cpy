
SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
};

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
};

SamplerState LinearSampler_Clamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
    AddressW = clamp;
};

SamplerState PointSampler_Border
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = border;
    AddressV = border;
    AddressW = border;
    BorderColor = float4(1.f, 0.0f, 0.0f, 0.0f);
};

SamplerState LinearSampler_Border
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = border;
    AddressV = border;
    AddressW = border;
    BorderColor = float4(1.0f, 0.0f, 0.0f, 0.0f);
};

RasterizerState RS_Default
{
    CullMode = BACK;
	
};
RasterizerState RS_Wireframe
{
    FillMode = WIREFRAME;
};

RasterizerState RS_Cull_CW
{
    FillMode = SOLID;
    CullMode = FRONT;
};

RasterizerState RS_Cull_NONE
{
    FillMode = SOLID;
    CullMode = NONE;
};

DepthStencilState DSS_Default
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_Fade
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = ALWAYS;
};

/* WBOIT용 */
DepthStencilState DSS_Compare_Only
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_Skip_Z
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState DSS_OnlyCompare
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

BlendState BS_Default
{
    BlendEnable[0] = FALSE;                                                                                                                                                                                                                           
};

/* WBOIT용 */
BlendState BS_BlendAcc
{
    BlendEnable[0] = TRUE;

    BlendOp = ADD;
    SrcBlend = ONE;
    DestBlend = ONE;
};

/* WBOIT용 */
BlendState BS_Revealage
{
    BlendEnable[0] = TRUE;

    BlendOp = ADD;
    SrcBlend = ZERO;
    DestBlend = INV_SRC_COLOR;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = TRUE;

    BlendOp = ADD;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;

	/*
	SrcColor(rgba), DestColor(rgba)

	SrcColor.rgb * SrcColor.a + DestColor.rgb * (1.f - SrcColor.a)
	*/		
};

BlendState BS_Blend
{
    BlendEnable[0] = TRUE;
    BlendOp[0] = Add;
    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;

    BlendEnable[1] = TRUE;
    BlendOp[1] = Add;
    SrcBlend[1] = ONE;
    DestBlend[1] = ONE;
	/*
	SrcColor(rgba), DestColor(rgba)

	SrcColor.rgb * 1 + DestColor.rgb * 1


	*/
};

bool IsBetween(float2 vQuery, float2 vBegin, float2 vEnd, float2 vCenter = float2(0.5f, 0.5f))
{
    vBegin = normalize(vBegin);
    vEnd = normalize(vEnd);

    //3.
    float3 vQueryVector = normalize(float3(vQuery - vCenter, 0.f));
    float3 vBeginVector = normalize(float3(vBegin, 0.f));
    float3 vEndVector = normalize(float3(vEnd, 0.f));

    float fCrossBeginEnd = cross(vBeginVector, vEndVector).z;
    float fCrossBeginQuery = cross(vBeginVector, vQueryVector).z;

    if (fCrossBeginEnd >= 0.f)
    {
        return fCrossBeginQuery > 0.f && dot(vQueryVector.xy, vBegin) > dot(vEnd, vBegin);
    }
    else
    {
        return !(fCrossBeginQuery < 0.f && dot(vQueryVector.xy, vBegin) > dot(vEnd, vBegin));
    }
}

float4 TransformNDCToWorld(float2 vTexCoord, float4 vDepthDesc, float4x4 ProjMatrixInv, float4x4 ViewMatrixInv)
{
    float4 vWorldPos;
    
    /* 투영 공간 위치 (TexCoord -> Clip Space ) */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w */
    vWorldPos.x = vTexCoord.x * 2.f - 1.f;
    vWorldPos.y = vTexCoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    float fViewZ = vDepthDesc.y;
    vWorldPos = vWorldPos * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, ProjMatrixInv);
     
	/* 로컬위치 * 월드행렬 */
    /* 월드 위치 */
    vWorldPos = mul(vWorldPos, ViewMatrixInv);

    return vWorldPos;
}

float3 Get_Normal_Red(Texture2D NormalTexture, SamplerState Sampler, float2 vTexCoord)
{
    return normalize(NormalTexture.Sample(Sampler, vTexCoord).bgr * 2.f - 1.f);
}

static const float PI = 3.141592653589793238462643383279502884197169f;
