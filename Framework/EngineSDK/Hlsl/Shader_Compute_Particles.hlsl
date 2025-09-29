#define PI              3.141592f
#define GRAVITY         9.87f

cbuffer Constant : register(b0) // 32
{
    uint     g_iNumInstance;     // 4
    float   g_fTimeDelta;       // 8
    int     g_bIsLoop;          // 12
    int     g_bIsLoopEnd;       // 16
    
    float4  g_vPivot;           // 16
}

struct Particle
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;

    float2 vLifeTime;
    float  fSpeed;
};

StructuredBuffer<Particle> ParticleInput : register(t0);
RWStructuredBuffer<Particle> ParticleOutput : register(u0);

[numthreads(256, 1, 1)]
void CS_Drop(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    ParticleOutput[iDispatchThreadID.x].vTranslation.x = ParticleInput[iDispatchThreadID.x].vTranslation.x;
    ParticleOutput[iDispatchThreadID.x].vTranslation.z = ParticleInput[iDispatchThreadID.x].vTranslation.z;
    ParticleOutput[iDispatchThreadID.x].vTranslation.y -= ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if(1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation.y = ParticleInput[iDispatchThreadID.x].vTranslation.y;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_Rise(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    ParticleOutput[iDispatchThreadID.x].vTranslation.x = ParticleInput[iDispatchThreadID.x].vTranslation.x;
    ParticleOutput[iDispatchThreadID.x].vTranslation.z = ParticleInput[iDispatchThreadID.x].vTranslation.z;
    ParticleOutput[iDispatchThreadID.x].vTranslation.y += ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation.y = ParticleInput[iDispatchThreadID.x].vTranslation.y;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_Explosion(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    float4 vMoveDir = normalize(ParticleOutput[iDispatchThreadID.x].vTranslation - g_vPivot);
    // ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vRight = -vMoveDir * length(ParticleInput[iDispatchThreadID.x].vRight);
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].vTranslation += vMoveDir * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop && 0 == g_bIsLoopEnd)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_Fraction(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    float4 vMoveDir = normalize(ParticleOutput[iDispatchThreadID.x].vTranslation - g_vPivot);
    
    // ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vRight = -vMoveDir * length(ParticleInput[iDispatchThreadID.x].vRight);
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].vTranslation += (vMoveDir * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta
     - ParticleOutput[iDispatchThreadID.x].vUp * GRAVITY * ParticleOutput[iDispatchThreadID.x].vLifeTime.y * g_fTimeDelta * ParticleOutput[iDispatchThreadID.x].fSpeed * 0.5f);
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_Hold(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
        else
            return;
    }
}


[numthreads(256, 1, 1)]
void CS_WaterRise(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if(iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    float fTranslation = sin(ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x * PI * 4.f) * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vTranslation.x += fTranslation;
    ParticleOutput[iDispatchThreadID.x].vTranslation.y += ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vTranslation.z += fTranslation;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;
    
    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {        
        if(1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation.y = ParticleInput[iDispatchThreadID.x].vTranslation.y;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_WaterRiseEnv(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    float fTranslation = sin(ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x * PI * 4.f) * 0.25f * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vTranslation.x += fTranslation;
    ParticleOutput[iDispatchThreadID.x].vTranslation.y += ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vTranslation.z += fTranslation;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;
    
    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation.y = ParticleInput[iDispatchThreadID.x].vTranslation.y;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_Charging(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    float4 vMoveDir = normalize(g_vPivot - ParticleOutput[iDispatchThreadID.x].vTranslation);
    // ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vRight = -vMoveDir * length(ParticleInput[iDispatchThreadID.x].vRight);
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].vTranslation += vMoveDir * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_AshFlow(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    float fLifeRate = ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleOutput[iDispatchThreadID.x].vLifeTime.x;
    float4 vPivotDir = normalize(g_vPivot - ParticleOutput[iDispatchThreadID.x].vTranslation);
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed -= g_fTimeDelta;
    
    float fClamp = clamp(fLifeRate * 4.f, 0.f, 1.f);
    // 피봇 방향의 영향은 시간이 지날수록 사라지도록
    float4 vTranslation = -vPivotDir * (1 - fClamp)
    // 흐름 방향의 영향은 시간이 지날수록 강해지도록
    + float4(sin(ParticleOutput[iDispatchThreadID.x].vTranslation.y), cos(ParticleOutput[iDispatchThreadID.x].vTranslation.z), sin(ParticleOutput[iDispatchThreadID.x].vTranslation.x), 0.f) * fClamp;
    
    ParticleOutput[iDispatchThreadID.x].vTranslation.x += vTranslation.x * ParticleOutput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vTranslation.y += vTranslation.y * ParticleOutput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vTranslation.z += vTranslation.z * ParticleOutput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    
    ParticleOutput[iDispatchThreadID.x].vRight = -normalize(vTranslation) * length(ParticleInput[iDispatchThreadID.x].vRight);
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_CrossSpread(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    // g_vPivot을 Look 방향으로 사용하여 Right, Up, Look 재계산
    float3 vLook = g_vPivot.xyz;
    ParticleOutput[iDispatchThreadID.x].vLook = float4(vLook, 0.f);
    ParticleOutput[iDispatchThreadID.x].vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook)), 0.f);
    ParticleOutput[iDispatchThreadID.x].vUp = float4(normalize(cross(vLook, ParticleOutput[iDispatchThreadID.x].vRight.xyz)), 0.f);
        
    if (iDispatchThreadID.x < uint(g_iNumInstance * 0.25f))
        ParticleOutput[iDispatchThreadID.x].vTranslation += ParticleOutput[iDispatchThreadID.x].vRight * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    else if (iDispatchThreadID.x < uint(g_iNumInstance * 0.5f))
        ParticleOutput[iDispatchThreadID.x].vTranslation -= ParticleOutput[iDispatchThreadID.x].vRight * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    else if (iDispatchThreadID.x < uint(g_iNumInstance * 0.75f))
        ParticleOutput[iDispatchThreadID.x].vTranslation += ParticleOutput[iDispatchThreadID.x].vUp * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    else
        ParticleOutput[iDispatchThreadID.x].vTranslation -= ParticleOutput[iDispatchThreadID.x].vUp * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;

    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed * max((1 - ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x * 1.5f), 0.f);

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
        }
        else
            return;
    }
}


[numthreads(256, 1, 1)]
void CS_GodRay(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    // 1. 높이는 일정하고 너비가 줄어들었다 길어졌다 함
    //   - 컴퓨트 쉐이더에서 벡터 길이 조절하고 기하 쉐이더에서 길이 구해서 사이즈로 써먹으면 될듯
    //   - 시작 사이즈는 8 ~ 20
    //   - 사이즈 30 찍으면 줄어들게 만들기
    // 2. Look 방향? 아무튼 아래 방향으로 천천히 움직임
    //   - 속도 : 9.5 ~ 10.5
    // 3. 전체 파티클이 Cone 형태를 띄게 생성
    //   - Explosion이되 pivot을 무조건 파티클 생성 위치보다 위로 잡아야 할듯
    // 4. 알파 : 수명에 따라 초반 & 막판에 0, 중간이 1(pow사용)
    // 5. 깊이에 따라 페이드 효과
    //   - 안개가 알아서 해결해 주려나?    
    
    float4 vMoveDir = normalize(ParticleOutput[iDispatchThreadID.x].vTranslation - g_vPivot);
    float fLifeRate = ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    
    ParticleOutput[iDispatchThreadID.x].vUp = -vMoveDir * 80.f;
    
    
    float fSizeRate = fLifeRate;
    if (fLifeRate > 0.5f)
        fSizeRate = 1.f - fLifeRate;
    
    if(0 == iDispatchThreadID.x % 2)
        fLifeRate *= -1.f;
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight * (6.f * max((iDispatchThreadID.x / (float) g_iNumInstance), 1.f)) * fLifeRate;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].vTranslation += vMoveDir * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
    {
        if (1 == g_bIsLoop)
        {
            ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
            ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
        }
        else
            return;
    }
}

[numthreads(256, 1, 1)]
void CS_ExplSlowDown(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    float4 vMoveDir = normalize(ParticleOutput[iDispatchThreadID.x].vTranslation - g_vPivot);
    // ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vRight = -vMoveDir * length(ParticleInput[iDispatchThreadID.x].vRight);
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    float fLifeRate = ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed * pow(2.f * (1.f - fLifeRate), 3.f);
    
    ParticleOutput[iDispatchThreadID.x].vTranslation += vMoveDir * ParticleOutput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
        return;
}

[numthreads(256, 1, 1)]
void CS_ExplScaleGradient(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    float4 vMoveDir = normalize(ParticleOutput[iDispatchThreadID.x].vTranslation - g_vPivot);
    // ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    
    float fLifeRate = ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    float fScaleSize = 0.f;
    
    if(fLifeRate <= 0.5f)
        fScaleSize = pow(fLifeRate * 2.f, 2.f);
    else
        fScaleSize = pow((1.f - fLifeRate) * 2.f, 2.f);
    
    ParticleOutput[iDispatchThreadID.x].vRight = -vMoveDir * length(ParticleInput[iDispatchThreadID.x].vRight) * fScaleSize;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp * fScaleSize;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed * pow(2.f * (1.f - fLifeRate), 3.f);
    
    ParticleOutput[iDispatchThreadID.x].vTranslation += vMoveDir * ParticleOutput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
        return;
}


[numthreads(256, 1, 1)]
void CS_Clear(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;
}