#include "../../../EngineSDK/hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;
Texture2D g_MaskTexture;

float4 g_vSrcColor = float4(1.f, 0.f, 1.f, 1.f);
float4 g_vDstColor = float4(0.f, 1.f, 1.f, 1.f);

float4 g_vWhiteColor = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vBlackColor = float4(0.f, 0.f, 0.f, 1.f);

bool g_bGamma = false;

/* for.SPRITE */
float2 g_vSpriteSize;
float2 g_vSpriteIndex;
bool g_bStretchY = false;

/* for. Ring, Hurt */
float g_fRatio;

/* for. Button */
bool g_bHovered;
float g_fThreshold;

/* for. Lock */
bool g_bLockOn;
float g_fTime;

/* for. Stretch */
float2 g_vStretchRange;
float g_fStretchFactor;

/* for. Stretch_Bar */
float g_fCur;
float g_fMax;
float g_fLast;
float g_fPrev;

bool g_bEffectStart;
float g_fEffectTime;

/* for. Fade */
float g_fAlpha;

/* for. Border */
bool g_bRightAngle;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT_DISOLVE
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float2 vMaskTexCoord : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    Out.vTexCoord = In.vTexCoord;

    return Out;
}

VS_OUT_DISOLVE VS_MAIN_DISOLVE(VS_IN In)
{
    VS_OUT_DISOLVE Out = (VS_OUT_DISOLVE) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    Out.vTexCoord = In.vTexCoord;

    float2 spriteSize = 1.0f / g_vSpriteSize;

    float2 texStart = g_vSpriteIndex * spriteSize;
    
    Out.vMaskTexCoord = (In.vTexCoord * spriteSize) + texStart;

    return Out;
}

VS_OUT VS_MAIN_SPRITE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    float2 spriteSize = 1.0f / g_vSpriteSize;

    float2 texStart = g_vSpriteIndex * spriteSize;
    
    Out.vTexCoord = (In.vTexCoord * spriteSize) + texStart;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct PS_IN_DISOLVE
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float2 vMaskTexCoord : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    //if (0.2f >= Out.vColor.a)
    //    discard;
    if (g_bGamma)
        Out.vColor = pow(Out.vColor, 1.f / 2.2f);
        
    clip(Out.vColor.a - 0.2f);

    return Out;
}

PS_OUT PS_MAIN_HURT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    Out.vColor.a *= (0.8 - g_fRatio);
    
    // 색상 변형
    Out.vColor.rgb *= g_vSrcColor.rgb;
    
    clip(Out.vColor.a - 0.0001f);

    return Out;
}

PS_OUT PS_MAIN_STRETCH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vStretchUV;
    if (g_bStretchY)
    {
        vStretchUV.x = In.vTexCoord.x;

        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.y < g_vStretchRange.x / newTexLength)
            vStretchUV.y = In.vTexCoord.y * newTexLength;
        else if (In.vTexCoord.y >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.y <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.y = g_vStretchRange.x;
        else
            vStretchUV.y = (g_vStretchRange.y + (In.vTexCoord.y - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    else
    {
        vStretchUV.y = In.vTexCoord.y;
    
        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
            vStretchUV.x = In.vTexCoord.x * newTexLength;
        else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.x = g_vStretchRange.x;
        else
            vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    Out.vColor = g_Texture.Sample(LinearSampler, vStretchUV);

    clip(Out.vColor.a - 0.2f);

    Out.vColor *= g_vSrcColor;
    
    return Out;
}

PS_OUT PS_MAIN_STRETCH_BAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vStretchUV;
    vStretchUV.y = In.vTexCoord.y;
    
    // 늘어난 UV 길이 계산
    float newTexLength = g_fStretchFactor;

    if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
        vStretchUV.x = In.vTexCoord.x * newTexLength;
    else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
        vStretchUV.x = g_vStretchRange.x;
    else
        vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    
    Out.vColor = g_Texture.Sample(LinearSampler, vStretchUV);
    
    clip(Out.vColor.a - 0.2f);

    if (In.vTexCoord.x >= g_fCur / g_fMax && In.vTexCoord.x <= g_fLast / g_fMax)
        Out.vColor = g_vDstColor;
    else if (In.vTexCoord.x > g_fLast / g_fMax)
        Out.vColor = g_vBlackColor;
    else
        Out.vColor *= g_vSrcColor;
    
    clip(Out.vColor.a - 0.2f);
    
    return Out;
}

PS_OUT PS_MAIN_STRETCH_BAR_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vStretchUV;
    vStretchUV.y = In.vTexCoord.y;
    
    // 늘어난 UV 길이 계산
    float newTexLength = g_fStretchFactor;

    if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
        vStretchUV.x = In.vTexCoord.x * newTexLength;
    else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
        vStretchUV.x = g_vStretchRange.x;
    else
        vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    
    Out.vColor = g_Texture.Sample(LinearSampler, vStretchUV);
    
    if (g_bEffectStart)
    {
        if (In.vTexCoord.x > g_fCur / g_fMax && In.vTexCoord.x < g_fPrev / g_fMax)
        {
            float fAlpha = g_fEffectTime / 0.5f; // 0.5 → 0을 1 → 0으로 변환
            Out.vColor.rgb = lerp(float3(1, 1, 1), g_vDstColor.rgb, 1 - fAlpha); // 흰색 → DstColor 보간
            Out.vColor.a *= fAlpha; // 알파값도 1 → 0으로 줄이기
        }
        else
        {
            clip(-1);
        }
    }
    
    clip(Out.vColor.a - 0.1);
    
    return Out;
}

PS_OUT PS_MAIN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    clip(Out.vColor.a - 0.2f);

    Out.vColor *= g_vSrcColor;
    
    return Out;
}

PS_OUT PS_MAIN_RING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 texColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
    
    if (g_fRatio <= 0.f)
        Out.vColor = texColor * g_vSrcColor;
    else
    {
        float2 center = float2(0.5f, 0.5f); // 중심점
        float2 offset = In.vTexCoord - center;

        // 반시계방향 각도 계산 (atan2 사용)
        float theta = atan2(-offset.x, offset.y);

        // 색상 계산 (HSV 스타일로 생각)
        theta = (theta + 3.14) / (2.0 * 3.14); // 0 ~ 1 범위로 정규화

        if (theta > g_fRatio)
        {
            Out.vColor.r = g_vSrcColor.b * g_fRatio;
            Out.vColor.g = g_vSrcColor.g;
            Out.vColor.b = g_vSrcColor.b * (1 - g_fRatio);
            Out.vColor.a = texColor.a;
        }
        else
            Out.vColor = float4(0.0f, 0.0f, 0.0f, texColor.a * 0.2f); // 어둡게
    }
    
    clip(Out.vColor.a - 0.1f);
    
    return Out;
}

PS_OUT PS_MAIN_FILTERBLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vTexelColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
    float4 vFilterColor = g_vSrcColor * (1.f - vTexelColor.a);

    Out.vColor = vTexelColor + vFilterColor;
    Out.vColor.a = vTexelColor.a; // 알파 값 유지

    return Out;
}

PS_OUT PS_MAIN_LOGOBUTTON(PS_IN_DISOLVE In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_bHovered)
    {
        Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
        Out.vColor.rgb = g_vSrcColor.rgb;

        clip(Out.vColor.a - 0.2f);
    
        float noiseValue = g_MaskTexture.Sample(LinearSampler, In.vMaskTexCoord).r;

        if (Out.vColor.a >= 0.01f)
            Out.vColor.a += noiseValue;

        Out.vColor.a = smoothstep(0.7, 0.95, Out.vColor.a);
    }

    clip(Out.vColor.a - 0.2f);

    return Out;
}

PS_OUT PS_MAIN_OPTIONBUTTON(PS_IN_DISOLVE In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_bHovered)
    {
        float4 vTexelColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
        
        float dissolveValue = g_MaskTexture.Sample(LinearSampler, In.vMaskTexCoord).r;
        
        clip(dissolveValue - g_fThreshold);
        
        Out.vColor = vTexelColor * g_vSrcColor;
    }
    else
    {
        Out.vColor.a = 0.f;
    }
    
    return Out;
}

PS_OUT PS_MAIN_BUTTON(PS_IN_DISOLVE In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_bHovered)
    {
        float2 vStretchUV;
    
        if (g_bStretchY)
        {
            vStretchUV.x = In.vTexCoord.x;

        // 늘어난 UV 길이 계산
            float newTexLength = g_fStretchFactor;

            if (In.vTexCoord.y < g_vStretchRange.x / newTexLength)
                vStretchUV.y = In.vTexCoord.y * newTexLength;
            else if (In.vTexCoord.y >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.y <= 1 - (1 - g_vStretchRange.y) / newTexLength)
                vStretchUV.y = g_vStretchRange.x;
            else
                vStretchUV.y = (g_vStretchRange.y + (In.vTexCoord.y - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
        }
        else
        {
            vStretchUV.y = In.vTexCoord.y;
    
        // 늘어난 UV 길이 계산
            float newTexLength = g_fStretchFactor;

            if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
                vStretchUV.x = In.vTexCoord.x * newTexLength;
            else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
                vStretchUV.x = g_vStretchRange.x;
            else
                vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
        }
        Out.vColor = g_Texture.Sample(LinearSampler, vStretchUV);
        Out.vColor.rgb = g_vSrcColor.rgb;

        clip(Out.vColor.a - 0.2f);
    
        float noiseValue = g_MaskTexture.Sample(LinearSampler, In.vMaskTexCoord).r;

        if (Out.vColor.a >= 0.01f)
            Out.vColor.a += noiseValue;

        Out.vColor.a = smoothstep(0.7, 0.95, Out.vColor.a);
    }
    else
    {
        Out.vColor.a = 0.f;
    }

    return Out;
}

PS_OUT PS_MAIN_INVENTORY(PS_IN_DISOLVE In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_bHovered)
    {
        float4 vTexelColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

        float dissolveValue = g_MaskTexture.Sample(LinearSampler, In.vMaskTexCoord).r;

        clip(dissolveValue - g_fThreshold);

        Out.vColor = vTexelColor * g_vSrcColor;
    }
    else
    {
        Out.vColor.rgba = 0.5f;
    }

    return Out;
}

PS_OUT PS_MAIN_LOCK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float distanceFromCenter = abs(In.vTexCoord.y - 0.5f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);

    clip(distanceFromCenter - (0.5f - g_fTime));

    return Out;
}

PS_OUT PS_MAIN_FADE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_vBlackColor;

    Out.vColor.a = g_fAlpha;

    clip(Out.vColor.a - 0.1f);

    return Out;
}

PS_OUT PS_MAIN_LOADING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);

    if (In.vTexCoord.y > 0.7f)
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    else
    {
        Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
    }
    return Out;
}

PS_OUT PS_MAIN_NOISE(PS_IN_DISOLVE In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vStretchUV;
    
    if (g_bStretchY)
    {
        vStretchUV.x = In.vTexCoord.x;

        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.y < g_vStretchRange.x / newTexLength)
            vStretchUV.y = In.vTexCoord.y * newTexLength;
        else if (In.vTexCoord.y >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.y <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.y = g_vStretchRange.x;
        else
            vStretchUV.y = (g_vStretchRange.y + (In.vTexCoord.y - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    else
    {
        vStretchUV.y = In.vTexCoord.y;
    
        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
            vStretchUV.x = In.vTexCoord.x * newTexLength;
        else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.x = g_vStretchRange.x;
        else
            vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    Out.vColor = g_Texture.Sample(LinearSampler, vStretchUV);
    Out.vColor.rgb = g_vSrcColor.rgb;

    clip(Out.vColor.a - 0.2f);
    
    float noiseValue = g_MaskTexture.Sample(LinearSampler, In.vMaskTexCoord).r;

    if (Out.vColor.a >= 0.01f)
        Out.vColor.a += noiseValue;

    Out.vColor.a = smoothstep(0.7, 0.95, Out.vColor.a);
    
    return Out;
}

PS_OUT PS_MAIN_NOISE_EDGE(PS_IN_DISOLVE In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 원래의 텍스처 색상
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
    Out.vColor.rgb = g_vSrcColor.rgb;
    
    float noiseValue = g_MaskTexture.Sample(LinearSampler, In.vMaskTexCoord).r;

    if (In.vTexCoord.x < 0.01f || In.vTexCoord.x > 0.99f || In.vTexCoord.y < 0.01f || In.vTexCoord.y > 0.99f)
        Out.vColor.a -= noiseValue;
    else
        Out.vColor.a += noiseValue;

    Out.vColor.a = smoothstep(0.5, 0.7, Out.vColor.a);
    
    return Out;
}

float RoundedBox(float2 p, float2 b, float4 r)
{
    float2 q = abs(p) - b;
    
    // 정확한 모서리 반경 보간
    float rx = lerp(lerp(r.x, r.y, (p.x + b.x) / (2.0 * b.x)),
                    lerp(r.w, r.z, (p.x + b.x) / (2.0 * b.x)),
                    (p.y + b.y) / (2.0 * b.y));

    float ry = lerp(lerp(r.x, r.w, (p.y + b.y) / (2.0 * b.y)),
                    lerp(r.y, r.z, (p.y + b.y) / (2.0 * b.y)),
                    (p.x + b.x) / (2.0 * b.x));

    float r_final = min(rx, ry);

    return length(max(q, 0.0)) - r_final;
}


PS_OUT PS_MAIN_BODRER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vStretchUV;
    if (g_bStretchY)
    {
        vStretchUV.x = In.vTexCoord.x;

        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.y < g_vStretchRange.x / newTexLength)
            vStretchUV.y = In.vTexCoord.y * newTexLength;
        else if (In.vTexCoord.y >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.y <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.y = g_vStretchRange.x;
        else
            vStretchUV.y = (g_vStretchRange.y + (In.vTexCoord.y - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    else
    {
        vStretchUV.y = In.vTexCoord.y;
    
        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
            vStretchUV.x = In.vTexCoord.x * newTexLength;
        else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.x = g_vStretchRange.x;
        else
            vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    float2 uv = vStretchUV * 2.0 - 1.0; // (-1,1) 범위로 정규화
    float2 size = float2(0.4, 0.4); // 내부 박스 크기

    
    float4 radius = float4(0.1f, 0.1f, 0.1f, 0.1f);
    
    if (g_bRightAngle)
        radius = float4(0.01f, 0.01f, 0.01f, 0.01f);
    
    float borderWidth = 0.0005f; // 얇은 테두리 두께 설정
    float falloff = 0.005f; // 부드러운 경계의 범위 (작게 설정)

    float borderDist = RoundedBox(uv, size, radius);
    float alpha = smoothstep(borderWidth + falloff, borderWidth, abs(borderDist)); // 부드럽게 페이드 효과

    Out.vColor = float4(g_vSrcColor.rgb, g_vSrcColor.a * alpha);

    clip(Out.vColor.a - 0.01f); // 클리핑 값 조절
    
    return Out;
}

PS_OUT PS_MAIN_SHOP_BORDER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vStretchUV;
    if (g_bStretchY)
    {
        vStretchUV.x = In.vTexCoord.x;

        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.y < g_vStretchRange.x / newTexLength)
            vStretchUV.y = In.vTexCoord.y * newTexLength;
        else if (In.vTexCoord.y >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.y <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.y = g_vStretchRange.x;
        else
            vStretchUV.y = (g_vStretchRange.y + (In.vTexCoord.y - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    else
    {
        vStretchUV.y = In.vTexCoord.y;
    
        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
            vStretchUV.x = In.vTexCoord.x * newTexLength;
        else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.x = g_vStretchRange.x;
        else
            vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    float2 uv = vStretchUV * 2.0 - 1.0; // (-1,1) 범위로 정규화
    float2 size = float2(0.4, 0.4); // 내부 박스 크기

    
    float4 radius = float4(0.1f, 0.1f, 0.1f, 0.1f);
    
    if (g_bRightAngle)
        radius = float4(0.01f, 0.01f, 0.01f, 0.01f);
    
    float borderWidth = 0.0001f; // 얇은 테두리 두께 설정
    float falloff = 0.002f; // 부드러운 경계의 범위 (작게 설정)

    float borderDist = RoundedBox(uv, size, radius);
    float alpha = smoothstep(borderWidth + falloff, borderWidth, abs(borderDist)); // 부드럽게 페이드 효과

    Out.vColor = float4(g_vSrcColor.rgb, g_vSrcColor.a * alpha);

    float innerBorderWidth = 0.00001f; // 내부 색칠할 영역 크기
    
    // 박스 내부인지 확인 (테두리 제외)
    if (borderDist <= -innerBorderWidth)
    {
        if (uv.y < -0.2f)  
            Out.vColor = g_vSrcColor;
    }
    
    clip(Out.vColor.a - 0.01f); // 클리핑 값 조절
    
    return Out;
}

PS_OUT PS_MAIN_OUTLINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 현재 픽셀 샘플링
    float4 color = g_Texture.Sample(LinearSampler, In.vTexCoord);
    
    float2 texSize;
    g_Texture.GetDimensions(texSize.x, texSize.y); // 텍스처 크기 가져오기
    float2 texelSize = 1.0 / texSize; // 텍셀 크기 계산

    float alpha = color.a;
    
    // 윤곽선 두께 설정
    float N = 10.0;
    
    // 주변 픽셀 샘플링 (8 방향 검사)
    float alphaLeft = g_Texture.Sample(LinearSampler, In.vTexCoord - float2(N * texelSize.x, 0)).a;
    float alphaRight = g_Texture.Sample(LinearSampler, In.vTexCoord + float2(N * texelSize.x, 0)).a;
    float alphaUp = g_Texture.Sample(LinearSampler, In.vTexCoord - float2(0, N * texelSize.y)).a;
    float alphaDown = g_Texture.Sample(LinearSampler, In.vTexCoord + float2(0, N * texelSize.y)).a;

    float alphaTopLeft = g_Texture.Sample(LinearSampler, In.vTexCoord + float2(-N * texelSize.x, N * texelSize.y)).a;
    float alphaTopRight = g_Texture.Sample(LinearSampler, In.vTexCoord + float2(N * texelSize.x, N * texelSize.y)).a;
    float alphaBotLeft = g_Texture.Sample(LinearSampler, In.vTexCoord + float2(-N * texelSize.x, -N * texelSize.y)).a;
    float alphaBotRight = g_Texture.Sample(LinearSampler, In.vTexCoord + float2(N * texelSize.x, -N * texelSize.y)).a;

    // 알파 값의 차이를 검사하여 윤곽선 생성
    float threshold = 0.3; // 차이가 이 값 이상이면 윤곽선 표시
    if (abs(alpha - alphaLeft) > threshold || abs(alpha - alphaRight) > threshold ||
        abs(alpha - alphaUp) > threshold || abs(alpha - alphaDown) > threshold ||
        abs(alpha - alphaTopLeft) > threshold || abs(alpha - alphaTopRight) > threshold ||
        abs(alpha - alphaBotLeft) > threshold || abs(alpha - alphaBotRight) > threshold)
    {
        Out.vColor = float4(0, 0, 0, 1); // 검은색 윤곽선
    }
    else
    {
        clip(-1); // 원래 색상 유지
    }
    
    return Out;
}

PS_OUT PS_MAIN_SHELL_BORDER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vStretchUV;
    if (g_bStretchY)
    {
        vStretchUV.x = In.vTexCoord.x;

        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.y < g_vStretchRange.x / newTexLength)
            vStretchUV.y = In.vTexCoord.y * newTexLength;
        else if (In.vTexCoord.y >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.y <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.y = g_vStretchRange.x;
        else
            vStretchUV.y = (g_vStretchRange.y + (In.vTexCoord.y - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    else
    {
        vStretchUV.y = In.vTexCoord.y;
    
        // 늘어난 UV 길이 계산
        float newTexLength = g_fStretchFactor;

        if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
            vStretchUV.x = In.vTexCoord.x * newTexLength;
        else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
            vStretchUV.x = g_vStretchRange.x;
        else
            vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    }
    float2 uv = vStretchUV * 2.0 - 1.0; // (-1,1) 범위로 정규화
    float2 size = float2(0.4, 0.4); // 내부 박스 크기

    
    float4 radius = float4(0.1f, 0.1f, 0.1f, 0.1f);
    
    if (g_bRightAngle)
        radius = float4(0.01f, 0.01f, 0.01f, 0.01f);
    
    float borderWidth = 0.05f; // 얇은 테두리 두께 설정
    float falloff = 0.005f; // 부드러운 경계의 범위 (작게 설정)

    float borderDist = RoundedBox(uv, size, radius);
    float alpha = smoothstep(borderWidth + falloff, borderWidth, abs(borderDist)); // 부드럽게 페이드 효과

    Out.vColor = float4(g_vSrcColor.rgb, g_vSrcColor.a * alpha);

    float innerBorderWidth = 0.00001f; // 내부 색칠할 영역 크기
    
    // 박스 내부인지 확인 (테두리 제외)
    if (borderDist <= -innerBorderWidth)
    {
        Out.vColor = g_vDstColor;
    }
    
    clip(Out.vColor.a - 0.01f); // 클리핑 값 조절
    
    return Out;
}

PS_OUT PS_MAIN_SKILL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
    
    clip(Out.vColor.a - 0.1f);
    
    if (Out.vColor.r >= 0.6f)
    {
        Out.vColor.rgb = g_vWhiteColor.rgb * Out.vColor.r;
    }
    else if(Out.vColor.g != 0.f)
    {
        Out.vColor.rgb = g_vSrcColor.rgb * Out.vColor.g;
    }
    else
    {
        Out.vColor = g_vBlackColor;
    }
    
    Out.vColor.a = g_vSrcColor.a;
    
    return Out;
}

PS_OUT PS_MAIN_GAUGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);
    clip(Out.vColor.a - 0.1f);
    
    float2 vStretchUV;
    vStretchUV.y = In.vTexCoord.y;
    
    // 늘어난 UV 길이 계산
    float newTexLength = g_fStretchFactor;

    if (In.vTexCoord.x < g_vStretchRange.x / newTexLength)
        vStretchUV.x = In.vTexCoord.x * newTexLength;
    else if (In.vTexCoord.x >= (g_vStretchRange.x / newTexLength) && In.vTexCoord.x <= 1 - (1 - g_vStretchRange.y) / newTexLength)
        vStretchUV.x = g_vStretchRange.x;
    else
        vStretchUV.x = (g_vStretchRange.y + (In.vTexCoord.x - (1 - (1 - g_vStretchRange.y) / newTexLength)) * newTexLength);
    
    if(vStretchUV.x < g_fCur / g_fMax)
        Out.vColor.rgb = g_vSrcColor.rgb;
    else
        Out.vColor.rgb = g_vDstColor.rgb;

    clip(Out.vColor.a - 0.2f);
    
    return Out;
}

technique11 DefaultTechnique
{
// 0
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
// 1
    pass BlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
// 2
    pass ColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }
// 3
    pass ColorBlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }
// 4
    pass RingPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RING();
    }
// 5
    pass FilterBlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FILTERBLEND();
    }
// 6
    pass LogoButtonPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_DISOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LOGOBUTTON();
    }
// 7
    pass OptionButtonPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_DISOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OPTIONBUTTON();
    }
// 8 
    pass ChatBubblePass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

// 9
    pass SpritePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SPRITE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }
// 10
    pass ButtonPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_DISOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BUTTON();
    }
// 11
    pass InventoryPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_DISOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INVENTORY();
    }
// 12
    pass LockPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LOCK();
    }
// 13
    pass StretchPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_STRETCH();
    }
// 14
    pass HurtPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HURT();
    }
// 15
    pass StretchBarPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_STRETCH_BAR();
    }
// 16
    pass StretchBarEffectPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Compare_Only, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_STRETCH_BAR_EFFECT();
    }
// 17
    pass NoisePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN_DISOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOISE();
    }
// 18
    pass NoiseEdgePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN_DISOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOISE_EDGE();
    }
// 19
    pass ColorNonCullPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }
//20
    pass BorderPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BODRER();
    }
//21
    pass StretchDefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_STRETCH();
    }
//22
    pass ShopBorderPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHOP_BORDER();
    }
//23
    pass OutLinePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }
//24
    pass ShellBorderPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHELL_BORDER();
    }
//25
    pass SkillPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SKILL();
    }
//26
    pass GaugePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GAUGE();
    }
}