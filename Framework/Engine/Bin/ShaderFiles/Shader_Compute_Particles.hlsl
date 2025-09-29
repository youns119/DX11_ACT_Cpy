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
    // �Ǻ� ������ ������ �ð��� �������� ���������
    float4 vTranslation = -vPivotDir * (1 - fClamp)
    // �帧 ������ ������ �ð��� �������� ����������
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
    
    // g_vPivot�� Look �������� ����Ͽ� Right, Up, Look ����
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
    
    // 1. ���̴� �����ϰ� �ʺ� �پ����� ������� ��
    //   - ��ǻƮ ���̴����� ���� ���� �����ϰ� ���� ���̴����� ���� ���ؼ� ������� ������� �ɵ�
    //   - ���� ������� 8 ~ 20
    //   - ������ 30 ������ �پ��� �����
    // 2. Look ����? �ƹ�ư �Ʒ� �������� õõ�� ������
    //   - �ӵ� : 9.5 ~ 10.5
    // 3. ��ü ��ƼŬ�� Cone ���¸� ��� ����
    //   - Explosion�̵� pivot�� ������ ��ƼŬ ���� ��ġ���� ���� ��ƾ� �ҵ�
    // 4. ���� : ���� ���� �ʹ� & ���ǿ� 0, �߰��� 1(pow���)
    // 5. ���̿� ���� ���̵� ȿ��
    //   - �Ȱ��� �˾Ƽ� �ذ��� �ַ���?    
    
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