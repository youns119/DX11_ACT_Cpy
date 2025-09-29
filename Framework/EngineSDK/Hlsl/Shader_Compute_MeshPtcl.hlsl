#define PI              3.141592f
#define GRAVITY         9.87f

cbuffer Constant : register(b0) // 32
{
    uint g_iNumInstance;    // 4
    float g_fTimeDelta;     // 8
    uint g_bIsLoop;         // 12
    float g_fPadding;       // 16
    
    float4 g_vPivot; // 16

    float3 g_vOriginSize; // 12
    float g_fFinalSize; // 16
}

struct Particle
{
    float4  vRight;
    float4  vUp;
    float4  vLook;
    float4  vTranslation;

    float4 vQuaternionRotation;
    
    float2  vLifeTime;
    float   fSpeed;
};

StructuredBuffer<Particle> ParticleInput : register(t0);
RWStructuredBuffer<Particle> ParticleOutput : register(u0);

float3 RotateByQuaternion(float3 v, float4 q)
{
    // Extract vector part of quaternion
    float3 u = q.xyz;

    // Scalar part of quaternion
    float s = q.w;

    // Rotation formula: v' = 2.0f * dot(u, v) * u
    //                 + (s*s - dot(u, u)) * v
    //                 + 2.0f * s * cross(u, v)
    return 2.0f * dot(u, v) * u
         + (s * s - dot(u, u)) * v
         + 2.0f * s * cross(u, v);
}

[numthreads(256, 1, 1)]
void CSMesh_Fraction(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    float4 vMoveDir = normalize(ParticleOutput[iDispatchThreadID.x].vTranslation - g_vPivot);
    
    ParticleOutput[iDispatchThreadID.x].vRight = ParticleInput[iDispatchThreadID.x].vRight;
    ParticleOutput[iDispatchThreadID.x].vUp = ParticleInput[iDispatchThreadID.x].vUp;
    ParticleOutput[iDispatchThreadID.x].vLook = ParticleInput[iDispatchThreadID.x].vLook;
    
    ParticleOutput[iDispatchThreadID.x].vTranslation += (vMoveDir * ParticleInput[iDispatchThreadID.x].fSpeed * g_fTimeDelta
     - ParticleOutput[iDispatchThreadID.x].vUp * GRAVITY * ParticleOutput[iDispatchThreadID.x].vLifeTime.y * g_fTimeDelta * ParticleOutput[iDispatchThreadID.x].fSpeed * 0.5f);
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    ParticleOutput[iDispatchThreadID.x].fSpeed = ParticleInput[iDispatchThreadID.x].fSpeed;

    if (ParticleInput[iDispatchThreadID.x].vLifeTime.x <= ParticleOutput[iDispatchThreadID.x].vLifeTime.y)
        return;
}

[numthreads(256, 1, 1)]
void CSMesh_ExplSlowDown(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    float fLifeRate = ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    
    float4 vMoveDir = normalize(ParticleOutput[iDispatchThreadID.x].vTranslation - g_vPivot);
    float4 vQuaternionRotation = ParticleInput[iDispatchThreadID.x].vQuaternionRotation;
    float fLifeSize = g_fFinalSize * fLifeRate;
    float3 vCurrentSize = fLifeRate >= 1.f ? g_vOriginSize * 2.f : g_vOriginSize * (1.f + fLifeSize);
    ParticleOutput[iDispatchThreadID.x].vRight = normalize(float4(RotateByQuaternion(ParticleOutput[iDispatchThreadID.x].vRight.xyz, vQuaternionRotation), 0.f)) * vCurrentSize.x;
    ParticleOutput[iDispatchThreadID.x].vUp = normalize(float4(RotateByQuaternion(ParticleOutput[iDispatchThreadID.x].vUp.xyz, vQuaternionRotation), 0.f)) * vCurrentSize.y;
    ParticleOutput[iDispatchThreadID.x].vLook = normalize(float4(RotateByQuaternion(ParticleOutput[iDispatchThreadID.x].vLook.xyz, vQuaternionRotation), 0.f)) * vCurrentSize.z;
    
    if (fLifeRate >= 1.f)
    {
        ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleOutput[iDispatchThreadID.x].vTranslation;
        return;
    }
    
    ParticleOutput[iDispatchThreadID.x].fSpeed = max(ParticleInput[iDispatchThreadID.x].fSpeed * (1.f - pow(fLifeRate, 2.f)), 0.f);
    ParticleOutput[iDispatchThreadID.x].vTranslation += vMoveDir * ParticleOutput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    // ParticleOutput[iDispatchThreadID.x].vTranslation = ParticleInput[iDispatchThreadID.x].vTranslation;
}

[numthreads(256, 1, 1)]
void CSMesh_Around(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    uint id = iDispatchThreadID.x;
    if (id >= g_iNumInstance)
        return;

    ParticleOutput[id].vLifeTime.x = ParticleInput[id].vLifeTime.x;
    ParticleOutput[id].vLifeTime.y += g_fTimeDelta;
    
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
    
    float fTime = ParticleOutput[id].vLifeTime.y;

    
    // 개별 진행
    float fLifeRate = fTime / ParticleOutput[id].vLifeTime.x;

    // 중심을 기준으로 회전 각도 계산
    float fAngle = (fTime * 0.5f + id * 0.15f) * 0.5f;
    float fRadius = ParticleInput[id].fSpeed;

    // 높이를 개별 랜덤처럼 보이게 흔들기
    float yOffset = id * 0.5f + sin(fTime * 2.f + id * 1.7f) * 2.0f;

    // 현재 위치 (원형 궤도 + 높이)
    float3 curOffset = float3(
        cos(fAngle) * fRadius,
        yOffset,
        sin(fAngle) * fRadius
    );
    float3 vCurPos = g_vPivot.xyz + curOffset;

    // 다음 위치로 방향 계산
    float fAngleNext = fAngle + 0.05f;
    float fYOffsetNext = id * 0.5f + sin(fTime * 2.f + id * 1.7f + 0.025f) * 2.0f;
    float3 vNextOffset = float3(
        cos(fAngleNext) * fRadius,
        fYOffsetNext,
        sin(fAngleNext) * fRadius
    );
    float3 vNextPos = g_vPivot.xyz + vNextOffset;

    float3 vDir = normalize(vNextPos - vCurPos);
    vDir *= -1;
    float3 vRight = normalize(cross(float3(0, 1, 0), vDir));
    float3 vUp = normalize(cross(vDir, vRight));

    ParticleOutput[id].vTranslation = float4(vCurPos, 1.0f);
    ParticleOutput[id].vLook = float4(vDir, 0.0f) * length(ParticleInput[id].vLook);
    ParticleOutput[id].vRight = float4(vRight, 0.0f) * length(ParticleInput[id].vRight);
    ParticleOutput[id].vUp = float4(vUp, 0.0f) * length(ParticleInput[id].vUp);
    ParticleOutput[id].fSpeed = ParticleInput[id].fSpeed; // 속도 고정
}

[numthreads(256, 1, 1)]
void CSMesh_Rise(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    
    ParticleOutput[iDispatchThreadID.x].vLifeTime.x = ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    ParticleOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;
    float fLifeRate = ParticleOutput[iDispatchThreadID.x].vLifeTime.y / ParticleInput[iDispatchThreadID.x].vLifeTime.x;
    
    float4 vMoveDir = float4(0.f, 1.f, 0.f, 0.f);
    float4 vQuaternionRotation = ParticleInput[iDispatchThreadID.x].vQuaternionRotation;
    
    ParticleOutput[iDispatchThreadID.x].vRight = float4(RotateByQuaternion(ParticleOutput[iDispatchThreadID.x].vRight.xyz, vQuaternionRotation), 0.f);
    ParticleOutput[iDispatchThreadID.x].vUp = float4(RotateByQuaternion(ParticleOutput[iDispatchThreadID.x].vUp.xyz, vQuaternionRotation), 0.f);
    ParticleOutput[iDispatchThreadID.x].vLook = float4(RotateByQuaternion(ParticleOutput[iDispatchThreadID.x].vLook.xyz, vQuaternionRotation), 0.f);
    
    ParticleOutput[iDispatchThreadID.x].fSpeed = max(ParticleInput[iDispatchThreadID.x].fSpeed * pow(fLifeRate, 2.f), ParticleInput[iDispatchThreadID.x].fSpeed);
    ParticleOutput[iDispatchThreadID.x].vTranslation += vMoveDir * ParticleOutput[iDispatchThreadID.x].fSpeed * g_fTimeDelta;
    
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
void CSMesh_Hold(uint3 iDispatchThreadID : SV_DispatchThreadID)
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
void CSMesh_Clear(uint3 iDispatchThreadID : SV_DispatchThreadID)
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