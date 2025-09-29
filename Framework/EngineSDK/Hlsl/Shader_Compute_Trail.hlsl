#define PI              3.141592f
#define GRAVITY         9.87f

cbuffer Constant : register(b0) // 32
{
    uint     g_iNumInstance;     // 4
    float   g_fTimeDelta;       // 8
    
    float2  g_vPadding;           // 16
}

struct TrailPoint
{
    float3 vPosition;
    float2 vLifeTime;
};

StructuredBuffer<TrailPoint> TrailInput : register(t0);
RWStructuredBuffer<TrailPoint> TrailOutput : register(u0);

[numthreads(256, 1, 1)]
void CS_Trail(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    

    TrailOutput[iDispatchThreadID.x].vLifeTime.x = TrailInput[iDispatchThreadID.x].vLifeTime.x;
    TrailOutput[iDispatchThreadID.x].vLifeTime.y += g_fTimeDelta;

    if (TrailInput[iDispatchThreadID.x].vLifeTime.x <= TrailOutput[iDispatchThreadID.x].vLifeTime.y)
    {
    }
}

[numthreads(256, 1, 1)]
void CS_Clear(uint3 iDispatchThreadID : SV_DispatchThreadID)
{
    if (iDispatchThreadID.x >= g_iNumInstance)
        return;
    

    TrailOutput[iDispatchThreadID.x].vLifeTime.x = TrailInput[iDispatchThreadID.x].vLifeTime.x;
    TrailOutput[iDispatchThreadID.x].vLifeTime.y = 0.f;
}