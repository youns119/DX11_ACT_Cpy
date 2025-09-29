#include "pch.h"
#include "Player.h"

#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "InputBuffer.h"
#include "InputGate.h"

#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"
#include "StateMachine_Player.h"
#include "StateMachine_PlayerUpper.h"
#include "Weapon_Player.h"
#include "Hand_Player.h"
#include "Shell_Item.h"
#include "Shell_World.h"
#include "Shell_Part.h"
#include "ShellSkill_RollOut.h"
#include "ActiveSkill.h"
#include "Item_HeartKelp.h"
#include "Magnet.h"
#include "Enemy.h"

#include "UI.h"
#include "UI_Player.h"
#include "UI_Compass.h"
#include "UI_Manager.h"
#include "UI_SkillEffect.h"
#include "Inventory.h"

#include "Event_JumpLaunchStart.h"
#include "Event_Transition.h"
#include "Event_NormalAttackInit.h"
#include "Event_NormalAttackStart.h"
#include "Event_NormalAttackEnd.h"
#include "Event_HammerAttackStart.h"
#include "Event_ChargeAttackStart.h"
#include "Event_ChargeAttackEnd.h"
#include "Event_HammerChargeAttackStart.h"
#include "Event_HammerChargeAttackEnd.h"
#include "Event_ChargeOn.h"
#include "Event_ChargeOff.h"
#include "Event_DashAttackStart.h"
#include "Event_DashAttackEnd.h"
#include "Event_HammerDashAttackStart.h"
#include "Event_SwipeAttackInit.h"
#include "Event_SwipeAttackStart.h"
#include "Event_SwipeAttackEnd.h"
#include "Event_PlungeAttackStart.h"
#include "Event_PlungeAttackEnd.h"
#include "Event_PlungeAttackImpact.h"
#include "Event_HammerSwipeAttackStart.h"
#include "Event_HammerSwipeAttackEnd.h"
#include "Event_ParryAttackInit.h"
#include "Event_ParryAttackStart.h"
#include "Event_ParryAttackEnd.h"
#include "Event_ParryAttackImpact.h"
#include "Event_ParryAttackFinish.h"
#include "Event_BackStepAttackInit.h"
#include "Event_BackStepAttackStart.h"
#include "Event_BackStepAttackEnd.h"
#include "Event_BackStepAttack_Sound.h"
#include "Event_BackStepAttackFinish.h"
#include "Event_EvadeNakedStart.h"
#include "Event_EvadeNakedEnd.h"
#include "Event_EvadeMediumStart.h"
#include "Event_EvadeHeavyStart.h"
#include "Event_EvadeOn.h"
#include "Event_EvadeOff.h"
#include "Event_BackStepStart.h"
#include "Event_BackStepOn.h"
#include "Event_BackStepOff.h"
#include "Event_WeaponColliderOn.h"
#include "Event_WeaponColliderOff.h"
#include "Event_HandColliderOn.h"
#include "Event_HandColliderOff.h"
#include "Event_ParryOn.h"
#include "Event_ParryOff.h"
#include "Event_Skill_Fizzle.h"
#include "Event_Skill_RollOut.h"
#include "Event_Skill_RollOut_BackFlip.h"
#include "Event_Hammer_Change.h"
#include "Event_GrappleAttack_Launch.h"
#include "Event_GrappleAttack_Impact.h"
#include "Event_GrappleAttack_BackFlip.h"
#include "Event_GrappleAttack_BackFlip2.h"
#include "Event_GrappleAttackFinish.h"
#include "Event_FinishAttackEnd.h"
#include "Event_FinishAttackExit.h"
#include "Event_FinishAttackFinish.h"
#include "Event_ActiveSkill.h"
#include "Event_MantisPunch_AutoLock.h"
#include "Event_MantisPunch_Teleport.h"
#include "Event_MantisPunch_Impact.h"
#include "Event_MantisPunch_PostImpact.h"
#include "Event_MantisPunch_TimeSlow.h"
#include "Event_MantisPunch_TimeSlow2.h"
#include "Event_MantisPunchFinish.h"
#include "Event_Urchin_Play.h"
#include "Event_Urchin_Stop.h"
#include "Event_Urchin_Dodge.h"
#include "Event_Urchin_Launch.h"
#include "Event_Skill_Fizzle.h"
#include "Event_Item_HeartKelp.h"
#include "Event_GetUp_Front.h"
#include "Event_GetUp_Back.h"
#include "Event_HeavyDamage.h"
#include "Event_NormalDamage.h"

/* For Effect */
#include "Event_SwimBubble.h"
#include "Event_SandBurst.h"
#include "Event_AttackOne.h"
#include "Event_Effect_ChargeAtt.h"
#include "Event_Effect_ChargingSeq00.h"
#include "Event_Effect_ChargingSeq01.h"
#include "Event_Effect_ChargingSeq02.h"
#include "Event_Effect_BeforeDash.h"
#include "Event_Effect_AfterDash.h"
#include "Event_Effect_Hammer.h"
#include "Event_Effect_MantisPunch_Player.h"
#include "Event_Effect_Plunge.h"
#include "Event_Effect_FallDown.h"
#include "Event_Effect_Fork.h"
#include "Event_Effect_Finish.h"

// Part Effect
#include "Effect_Textured_Rope.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

// Interface
#include "IParried.h"
#include "Interactive_Bounceable.h"

// 임시
#include "ShellHouse.h"
#include "Level_Loading.h"
#include "CinematicCamera.h"
#include "Remains.h"

// Manager
#include "PlayerManager.h"
#include "Projectile_Manager.h"
#include "Skill_DataBase.h"
#include "Theme_Manager.h"
#include "Enemy_Manager.h"

#include "Collider_Sphere.h"
#include "Shell_DataBase.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : super(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (pDesc == nullptr)
        return E_FAIL;

    pDesc->GameObjectDesc.fSpeedPerSec = 10.f;
    pDesc->GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

    // Transition
    {
        CPlayerManager::Load_Transition(pDesc->GameObjectDesc.WorldMatrix);
    }

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    // Load_PlayerData
    CPlayerManager::Load_PlayerData(*this);

    if (FAILED(Ready_Component()) ||
        FAILED(Ready_PartObject()))
        return E_FAIL;

    // Load_EquipData
    CPlayerManager::Load_EquipData(*this);

    if (FAILED(Ready_Event()) ||
        FAILED(Ready_StateMachine()) ||
        FAILED(Ready_Sound()))
        return E_FAIL;

    // 디버깅용
#ifdef _DEBUG

    UnLock_ActionSkill(ACTIONSKILL::ACTIONSKILL_FINISH);
    UnLock_ActionSkill(ACTIONSKILL::ACTIONSKILL_FISHING);
    UnLock_ActionSkill(ACTIONSKILL::ACTIONSKILL_HAMMER);

    UnLock_ActiveSkill(ACTIVESKILL::ACTIVESKILL_URCHIN);
    UnLock_ActiveSkill(ACTIVESKILL::ACTIVESKILL_MANTISPUNCH);

    m_tPlayerStat.ePrimaryActiveSkill = ACTIVESKILL::ACTIVESKILL_URCHIN;
    m_tPlayerStat.eSecondaryActiveSkill = ACTIVESKILL::ACTIVESKILL_MANTISPUNCH;

#endif

    m_pInputGate = CInputGate::Create();
    m_pInputBuffer = CInputBuffer::Create(m_tPlayerStat.fInputBufferTime);

    m_mapGrappleLayer.emplace(L"Layer_InteractionObject", static_cast<_uint>(COLLISION_GROUP::GRAPPLENEEDLE));
    m_mapGrappleLayer.emplace(L"Layer_Monster", static_cast<_uint>(COLLISION_GROUP::MONSTER));
    m_mapGrappleLayer.emplace(L"Layer_Shell", static_cast<_uint>(COLLISION_GROUP::SHELL));

    m_fBubbleGenTime = 5.f;
    m_fBubbleGenTimeAcc = 0.f;

    if (m_tPlayerStat.bTeleport)
    {
        m_tPlayerStat.bTeleport = false;
        MoonShell_Interact(false);
    }

    UI_Setting();

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    // InputGate
    m_pInputGate->Update(fTimeDelta);
    // InputBuffer
    m_pInputBuffer->Update(fTimeDelta);

    // Part_Camera Fix 여부에 따른 입력 수행
    {
//#ifdef _DEBUG
        // 리스폰 디버깅용
        if (m_pGameInstance->Key_Down(KEY::F7))
        {
            Rest();

            m_tPlayerStat.bDeath = false;
            m_pStateMachine->ChangeState(L"Idle");
        }
        
        // 슈퍼울트라치트모드
        if (m_pGameInstance->Key_Down(KEY::F4))
        {
            if (!m_tPlayerStat.bCheat)
            {
                m_pStateMachine->ChangeState(L"SuperUltraCheatMode");
                m_pUpperStateMachine->ChangeState(L"None");

                Rest();
            }
            else
                m_pStateMachine->ChangeState(L"Idle");
        }

        Sibal();

//#endif

        //if (m_pPlayerCineCamera->Get_Active())
        if(m_pPlayerCineCamera->IsCurrCam())
        {
            if(m_pPlayerCineCamera->Get_InputEnable() || m_tPlayerStat.bInputEnable)
                Input_Control(fTimeDelta);
        }
        //else if ((Get_PlayerCamera()->Get_IsFixed() && Get_PlayerCamera()->Get_Active()))
        else if ((Get_PlayerCamera()->Get_IsFixed() && Get_PlayerCamera()->IsCurrCam()))
        {
            if(m_tPlayerStat.bInputEnable)
                Input_Control(fTimeDelta);
        }
    }

    m_pMovementCom->Priority_Update(fTimeDelta);

    super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    // Movement
    {
        _vector vPrevPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        m_pMovementCom->Update(fTimeDelta);
        _vector vCurrPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        _vec3 vDist = vCurrPosition - vPrevPosition;

        COLLISION_DESC CollisionDesc = m_pPhysicsControllerCom->Move(vDist, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);

        if (CollisionDesc.Is_CollisionDown())
        {
            m_tPlayerStat.bInAir = false;
            m_pMovementCom->Set_InAir(false);
        }
        else
        {
            m_tPlayerStat.bInAir = true;
            m_pMovementCom->Set_InAir(true);
        }

        if (CollisionDesc.Is_CollisionSide())
        {
            if (m_tPlayerStat.bRolling)
                Rolling_Collide();
            else if (m_tPlayerStat.bGrappling)
                Grappling_Collide();
        }
    }

    // StateMachine
    m_pStateMachine->Update(fTimeDelta);
    m_pUpperStateMachine->Update(fTimeDelta);

    // Model
    m_pModelCom->Play_Animation(fTimeDelta, m_tPlayerStat.fAnimSpeed);

    // Collider
    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

    // AutoLock
    if (m_bAutoLock)
        AutoLock_Movement(fTimeDelta);

    // Grapple
    Check_GrappleTarget();
    GrappleAim_Movement();

    // Hammer
    Hammer_SlotAnim();

    // Finish
    Check_FinishTarget();
    Finish_Movement(fTimeDelta);

    // Debuff
    Check_Debuff(fTimeDelta);

    // Respawn
    Respawn(fTimeDelta);
    
    // Sound
    m_pGameInstance->Set_ListenerPositoin(m_pTransformCom->Get_State(CTransform::STATE::POSITION));

    super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Simulate_Physics(m_pPhysicsControllerCom);

#ifdef _DEBUG

    // DebugComponent 등록
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsControllerCom);

    if (m_pGrappleTarget)
    {
        _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        _vec3 vDst = static_cast<CTransform*>(m_pGrappleTarget->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
        _vec3 vDir = vDst - vPos;
        m_pGameInstance->Add_DebugRender_Ray(vPos, vDir, false, _float4(1.f, 0.f, 1.f, 0.f));
    }

#endif

    // Movement
    m_pMovementCom->Late_Update(fTimeDelta);

    Call_BubbleEnvEffect(fTimeDelta);
    Decrease_RadialBlur(fTimeDelta);

    if (m_bRender && !m_bHide)
    {
        // Renderer 등록 및 Physics 등록
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
    }

    if (m_bRender)
        super::Late_Update(fTimeDelta);
}

void CPlayer::Final_Update(_float fTimeDelta)
{
    Calculate_CombinedMatrix();

    super::Final_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        _uint iShaderPass = 0;
        if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DEFAULT;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR;

            if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
            return E_FAIL;

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CPlayer::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin((_uint)SHADER_PASS_ANIMMESH::SHADOW);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

CCamera_Player* CPlayer::Get_PlayerCamera()
{
    return m_pPlayerSpringArm->Get_PlayerCamera();
}

const FShellStat* CPlayer::Get_PlayerShellStat_Const() const
{
    if (m_pPlayerShell == nullptr)
        return nullptr;

    return m_pPlayerShell->Get_ShellStat_Const();
}

const FWeaponStat* CPlayer::Get_PlayerWeaponStat_Const() const
{
    if (m_pPlayerWeapon == nullptr)
        return nullptr;

    return m_pPlayerWeapon->Get_WeaponStat_Const();
}

const FShellStat* CPlayer::Get_PlayerWeaponShellStat_Const() const
{
    if (m_pPlayerWeapon == nullptr)
        return nullptr;

    return m_pPlayerWeapon->Get_ShellStat_Const();
}

CTransform* CPlayer::Get_LockOnTransform() const
{
    CTransform* pTransform = static_cast<CTransform*>(m_pPlayerSpringArm->Get_LockOnTarget()->Find_Component(g_strTransformTag));
    if (pTransform == nullptr)
        return nullptr;

    return pTransform;
}

void CPlayer::Sibal()
{
    if (m_pGameInstance->Key_Press(KEY::RCONTORL))
    {
        if (m_pGameInstance->Key_Down(KEY::NUM_1))
        {
            UnLock_ActionSkill(ACTIONSKILL::ACTIONSKILL_TELEPORT);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_2))
        {
            UnLock_ActionSkill(ACTIONSKILL::ACTIONSKILL_HAMMER);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_3))
        {
            UnLock_ActionSkill(ACTIONSKILL::ACTIONSKILL_FISHING);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_4))
        {
            UnLock_ActionSkill(ACTIONSKILL::ACTIONSKILL_FINISH);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_5))
        {
            UnLock_ActiveSkill(ACTIVESKILL::ACTIVESKILL_URCHIN);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_6))
        {
            UnLock_ActiveSkill(ACTIVESKILL::ACTIVESKILL_MANTISPUNCH);
        }
    }
    else
    {
        if (m_pGameInstance->Key_Down(KEY::NUM_1))
        {
            _float fHP = CShell_DataBase::Get().Get_Stat(SHELL::SHELL_HOME).fShellMaxHP;
            Equip_Shell(SHELL::SHELL_HOME, fHP);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_2))
        {
            _float fHP = CShell_DataBase::Get().Get_Stat(SHELL::SHELL_SODACAN).fShellMaxHP;
            Equip_Shell(SHELL::SHELL_SODACAN, fHP);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_3))
        {
            _float fHP = CShell_DataBase::Get().Get_Stat(SHELL::SHELL_MIRRORBALL).fShellMaxHP;
            Equip_Shell(SHELL::SHELL_MIRRORBALL, fHP);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_4))
        {
            _float fHP = CShell_DataBase::Get().Get_Stat(SHELL::SHELL_DUCK).fShellMaxHP;
            Equip_Shell(SHELL::SHELL_DUCK, fHP);
        }
        if (m_pGameInstance->Key_Down(KEY::NUM_5))
        {
            _float fHP = CShell_DataBase::Get().Get_Stat(SHELL::SHELL_TENNISBALL).fShellMaxHP;
            Equip_Shell(SHELL::SHELL_TENNISBALL, fHP);
        }
    }
}

void CPlayer::Move_Player(_fvector vPos)
{
    _vec3 vTargetPos = vPos;

    _vector vPrevPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    m_pTransformCom->Locate(vTargetPos._float3());
    _vector vCurrPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    _vec3 vDist = vCurrPosition - vPrevPosition;

    COLLISION_DESC CollisionDesc = m_pPhysicsControllerCom->Move(vDist, 0.005f, 0.01f);
}

void CPlayer::Set_AutoLock(_bool bAutoLock, _float fAutoDist)
{
    m_bAutoLock = bAutoLock;

    if (m_bAutoLock)
    {
        m_pAutoLockTarget = m_pPlayerSpringArm->Get_AutoLockTarget(fAutoDist);
        if (m_pAutoLockTarget != nullptr)
        {
            m_pAutoLockTransform = static_cast<CTransform*>(m_pAutoLockTarget->Find_Component(g_strTransformTag));
            if (m_pAutoLockTransform != nullptr)
                m_pMovementCom->Set_RotateToDirection(false);
        }
        else m_bAutoLock = !m_bAutoLock;
    }
    else
    {
        m_pAutoLockTarget = nullptr;
        m_pAutoLockTransform = nullptr;
        m_pMovementCom->Set_RotateToDirection(true);
    }
}

void CPlayer::UnLock_LockOn()
{
    m_pPlayerSpringArm->UnLock_LockOn();

    m_tPlayerStat.bLockOn = false;
}

void CPlayer::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
{
    // Invincible 일 때 무적
    if (m_tPlayerStat.bParry || m_tPlayerStat.bInvincible || m_tPlayerStat.bDeath)
        return;

    if (m_tPlayerStat.bRolling && eDamage != DAMAGE::DAMAGE_SUPER)
    {
        CTransform* pTransform = static_cast<CTransform*>(pDamager->Find_Component(g_strTransformTag));
        _vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
        _vec3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

        _vec3 vDir = (vMyPos - vPos).Normalized();
        vDir.y = 0.f;

        m_pMovementCom->Reset_Velocity();
        m_pMovementCom->Add_Force((vMyPos + vDir)._vector(), 20.f);
        m_pMovementCom->Set_RotateToDirection(true);

        m_pTransformCom->LookAt(vDir._vector());

        return;
    }

    if (m_tPlayerStat.bUseItem || m_tPlayerStat.bGrappleAim)
    {
        m_pUpperStateMachine->ChangeState(L"None");

        if(m_tPlayerStat.bUseItem)
            UnUse_Item();
    }

    // 등껍질의 방어 계수만큼이 플레이어의 방어 계수가 됨
    // 방어 계수 1당 데미지 경감 1퍼센트
    // 등껍질로 방어하면 등껍질의 내구도가 대신닳음
    _bool bShellGuard{};
    _float fFinalDamage{};
    if (m_tPlayerStat.bHasShell && m_tPlayerStat.bGuard)
    {
        bShellGuard = true;

        fFinalDamage = fDamage - m_tPlayerStat.iResistance * 0.2f;
        if (fFinalDamage <= 1.f) fFinalDamage = 1.f;
    }
    else
        fFinalDamage = fDamage - fDamage * ((_float)m_tPlayerStat.iDefense / 100.f);

    Attacked(bShellGuard, fFinalDamage, eDamage, pDamager);
}

void CPlayer::Grabbed(const _float4x4* pGrabSocketMatrix, const _float4x4* pParentMatrix)
{
    m_pStateMachine->ChangeState(L"Grabbed");
    m_pUpperStateMachine->ChangeState(L"None");

    m_pGrabSocketMatrix = pGrabSocketMatrix;
    m_pParentWorldMatrix = pParentMatrix;

    _uint iRand = rand() % 3;
    switch (iRand)
    {
    case 0:
        Play_PlayerSound(L"Grabbed1");
        break;
    case 1:
        Play_PlayerSound(L"Grabbed2");
    break;
    case 2:
        Play_PlayerSound(L"Grabbed3");
        break;
    default:
        break;
    }
}

void CPlayer::UnGrab(_float fPower, DAMAGE eDamage, CGameObject* pDamager, _fvector vDir)
{
    m_tPlayerStat.bGrabbed = false;

    _vec3 vPos = _mat(m_CombinedWorldMatrix).position;
    m_pTransformCom->Locate(vPos._float3());
    m_pPhysicsControllerCom->Set_Position(vPos);

    _vec3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

    _vec3 vTossDir{};
    if (_vec3(vDir).Length() != 0.f)
        vTossDir = _vec3(vDir).Normalized();
    else
    {
        CTransform* pTransform = static_cast<CTransform*>(pDamager->Find_Component(g_strTransformTag));
        vTossDir = (_vec3(pTransform->Get_State(CTransform::STATE::POSITION)) - vMyPos).Normalized() * -1.f;
    }

    _uint iDamage = static_cast<_uint>(eDamage);
    switch (iDamage)
    {
    case (_uint)DAMAGE::DAMAGE_NORMAL :
    {
        vTossDir.y = 0.f;
        m_pTransformCom->LookAt((vMyPos + vTossDir)._vector());
        m_pMovementCom->Add_Force(vTossDir._vector(), fPower);

        m_pStateMachine->ChangeState(L"NormalAttacked");
    }
    break;
    case (_uint)DAMAGE::DAMAGE_HEAVY:
    {
        vTossDir.y  = 0.f;
        m_pTransformCom->LookAt((vMyPos + vTossDir)._vector());

        vTossDir.y += 2.f;
        m_pMovementCom->Add_Force(vTossDir._vector(), fPower);

        m_pStateMachine->ChangeState(L"HeavyAttacked_Front");
    }
    break;
    case (_uint)DAMAGE::DAMAGE_SUPER:
    {
        vTossDir.y += 4.f;
        m_pTransformCom->LookAt((vMyPos + vTossDir)._vector());
        m_pMovementCom->Add_Force(vTossDir._vector(), fPower);

        m_pStateMachine->ChangeState(L"SuperAttacked");
    }
    break;
    default :
        break;
    }

    m_pGrabSocketMatrix = nullptr;
    m_pParentWorldMatrix = nullptr;

    m_pColliderCom->Set_Active(true);
}

void CPlayer::Player_WeaponCollider(_bool bCollider)
{
    m_pPlayerWeapon->Collider_Toggle(bCollider);
}

void CPlayer::Player_HandCollider(_bool bCollider)
{
    m_pPlayerHand->Collider_Toggle(bCollider);
}

void CPlayer::Player_CheckLockOn()
{
    // LockOn인지 확인하고 상태 전환
    // LockOn O -> LockOn으로 상태전환
    // LockOn X -> Idle로 상태전환
    if (m_pPlayerSpringArm->Get_IsLockOn() && !m_tPlayerStat.bLockOn)
    {
        m_pStateMachine->ChangeState(L"LockOn"); 
        m_tPlayerStat.bLockOn = true;
    }
    else if (!m_pPlayerSpringArm->Get_IsLockOn() && m_tPlayerStat.bLockOn)
    {
        m_pStateMachine->ChangeState(L"Idle");
        m_tPlayerStat.bLockOn = false;
    }
}

HRESULT CPlayer::Equip_Shell(SHELL eShell, _float fHP)
{
    Equip_Shell_NoSound(eShell, fHP);
    Play_PlayerSound(L"Shell_Equip");

    return S_OK;
}

HRESULT CPlayer::Equip_Shell_NoSound(SHELL eShell, _float fHP)
{
    // 새로운 등껍질 입는 함수
    if (m_tPlayerStat.bHasShell)
        UnEquip_Shell();

    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->Hide_Interaction();

    CShell_Part::DESC tShellDesc{};
    tShellDesc.ShellItemDesc.ItemDesc.GameObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
    tShellDesc.ShellItemDesc.eShell = eShell;
    tShellDesc.ShellItemDesc.fShellCurrHP = fHP;
    tShellDesc.pOwner = this;

    if (FAILED(Add_PartObject
    (
        L"Part_Shell",
        LEVEL_STATIC,
        L"Prototype_GameObject_Shell_Part",
        &tShellDesc
    )))
        return E_FAIL;

    m_pPlayerShell = static_cast<CShell_Part*>(Find_PartObject({ L"Part_Shell" }));
    m_tPlayerStat.bHasShell = true;

    m_pPlayerShellStat = m_pPlayerShell->Get_ShellStat();
    m_tPlayerStat.iDefense = static_cast<_uint>(m_pPlayerShellStat->fShellDefense);

    return S_OK;
}

HRESULT CPlayer::UnEquip_Shell()
{
    UnEquip_Shell_NoSound();
    m_pGameInstance->Play_Sound(L"Player_Shell_UnEquip.wav", (_uint)SOUND_CHANNEL::PLAYER_EFFECT, 0.4f);

    return S_OK;
}

HRESULT CPlayer::UnEquip_Shell_NoSound()
{
    // 현재 등껍질 벗는 함수

    // 등껍질 없으면 넘어감
    if (!m_tPlayerStat.bHasShell)
        return S_OK;

    _float4x4 matTransform = m_pPlayerShell->Get_CombinedMatrix();
    SHELL eShell = m_pPlayerShell->Get_ShellStat_Const()->eShell;
    _float fShellCurrHP = m_pPlayerShell->Get_ShellStat_Const()->fShellCurrHP;

    CShell_Item::DESC tShellDesc{};
    XMStoreFloat4x4(&tShellDesc.ItemDesc.GameObjectDesc.WorldMatrix, XMLoadFloat4x4(&matTransform));
    tShellDesc.eShell = eShell;
    tShellDesc.fShellCurrHP = fShellCurrHP;
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_STATIC,
        TEXT("Prototype_GameObject_Shell_World"),
        m_pGameInstance->Get_CurrentLevelIndex(),
        TEXT("Layer_Shell"),
        &tShellDesc
    )))
        return E_FAIL;

    Delete_Shell();

    return S_OK;
}

HRESULT CPlayer::Equip_Weapon(_uint iLevel, _float fDamage, SHELL eShell, _float fHP)
{
    // Weapon
    {
        CWeapon_Player::DESC WeaponDesc{};

        WeaponDesc.PartObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
        WeaponDesc.pOwner = this;
        WeaponDesc.iLevel = iLevel;
        WeaponDesc.fDamage = fDamage;

        if (FAILED(Add_PartObject(L"Part_Weapon", LEVEL_STATIC, L"Prototype_GameObject_Weapon_Player", &WeaponDesc)))
            return E_FAIL;

        m_pPlayerWeapon = static_cast<CWeapon_Player*>(Find_PartObject({ L"Part_Weapon" }));
        m_pPlayerWeapon->Add_Socket(CWeapon_Player::SOCKET::SOCKET_HAND, "../Bin/DataFiles/PartObjectData/Fork_Hand.data");
        m_pPlayerWeapon->Add_Socket(CWeapon_Player::SOCKET::SOCKET_CLAW, "../Bin/DataFiles/PartObjectData/Fork_Claw.data");
        m_pPlayerWeapon->Add_Socket(CWeapon_Player::SOCKET::SOCKET_CLAW_END, "../Bin/DataFiles/PartObjectData/Fork_Claw_End.data");
        m_pPlayerWeapon->Add_Socket(CWeapon_Player::SOCKET::SOCKET_FINGER, "../Bin/DataFiles/PartObjectData/Fork_Finger.data");
        m_pPlayerWeapon->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_HAND);

        if (eShell != SHELL::SHELL_END)
            m_pPlayerWeapon->Attach_Shell_NoSound(eShell, fHP);
    }

    // Hand
    {
        CHand_Player::DESC HandDesc{};

        HandDesc.PartObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
        HandDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Hand_Grip_R");
        HandDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_Hand", LEVEL_STATIC, L"Prototype_GameObject_Hand_Player", &HandDesc)))
            return E_FAIL;

        m_pPlayerHand = static_cast<CHand_Player*>(Find_PartObject({ L"Part_Hand" }));
        m_pPlayerHand->Set_Damage(m_pPlayerWeapon->Get_WeaponStat_Const()->fDamage);
    }

    return S_OK;
}

void CPlayer::Grapple_Aim()
{
    if (m_tPlayerStat.bGrappleAim)
    {
        m_pPlayerSpringArm->Zoom(m_pPlayerSpringArm->Get_DefaultArmLength() * 0.6f);
        m_pMovementCom->Set_RotateToDirection(false);
    }
    else
    {
        m_pPlayerSpringArm->Zoom(m_pPlayerSpringArm->Get_DefaultArmLength());
        m_pMovementCom->Set_RotateToDirection(true);
    }
}

void CPlayer::Grapple_Hit()
{
    IGrapple* pGrapple = dynamic_cast<IGrapple*>(m_pGrappleTarget);
    if (pGrapple == nullptr)
        return;

    _uint iType = pGrapple->Get_Type();
    m_pGameInstance->Stop_Sound((_uint)SOUND_CHANNEL::PROJECTILE_EFFECT);

    switch (iType)
    {
    case (_uint)COLLISION_GROUP::GRAPPLENEEDLE :
    {
        m_pStateMachine->ChangeState(L"Grapple_Loop");
        Play_PlayerSound(L"Grapple_Hit");
    }
    break;
    case (_uint)COLLISION_GROUP::MONSTER :
    {
        m_pStateMachine->ChangeState(L"Grapple_AttackStart");

        m_pGameInstance->Call_Lag(0.1f, 0.5f);
        //Player_CameraShake_Position(0.05f, 15.f, 0.8f);
        Get_PlayerCineCamera()->PositionShake(0.05f, 15.f, 0.8f);
    }
    break;
    case (_uint)COLLISION_GROUP::SHELL :
    {
        m_pStateMachine->ChangeState(L"Grapple_Shell");
        Play_PlayerSound(L"Grapple_Hit");
    }
    break;
    default:
        break;
    }

    _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    _vec3 vTargetPos = pGrapple->Get_GrapplePos();
    _vec3 vDir = (vTargetPos - vPos)._vector();

    m_pMovementCom->Reset_Velocity();
    m_pTransformCom->LookAt((vPos + vDir)._vector());

    static_cast<CEffect_Textured_Rope*>(m_PartObjects.find(L"Part_Rope")->second)->Set_Info(vTargetPos._vector());
    Call_GrappleEffect(vTargetPos._vector());
}

void CPlayer::Grapple_Launch()
{
    _vec3 vPos = m_pColliderCom->Get_BoundingCenter();
    _vec3 vTargetPos = static_cast<CCollider*>(m_pGrappleTarget->Find_Component(L"Com_Collider"))->Get_BoundingCenter();
    _vector vDir = (vTargetPos - vPos)._vector();

    m_pMovementCom->Activate(true);
    m_pMovementCom->FlyMode(true);
    m_pMovementCom->Reset_Velocity();
    m_pMovementCom->Set_ForceFriction(0.f);
    m_pMovementCom->Add_Force(vDir, m_tPlayerStat.fGrappleForce);

    m_pPhysicsControllerCom->Set_UserData(true);

    Apply_RadialBlur(1.5f);

    m_pPlayerCineCamera->Stop_Cinematic();
}

void CPlayer::UnGrapple()
{
    IGrapple* pGrapple = dynamic_cast<IGrapple*>(m_pGrappleTarget);
    if (pGrapple == nullptr)
        return;

    _uint iType = pGrapple->Get_Type();

    switch (iType)
    {
    case (_uint)COLLISION_GROUP::GRAPPLENEEDLE :
    {
    }
    break;
    case (_uint)COLLISION_GROUP::MONSTER:
    {
        static_cast<CEnemy*>(m_pGrappleTarget)->Set_Hooked(false);
    }
    break;
    case (_uint)COLLISION_GROUP::SHELL:
    {
        static_cast<CShell_World*>(m_pGrappleTarget)->UnLaunch();
    }
    break;
    default :
        break;
    }

    Reset_XZRotation();

    m_pMovementCom->FlyMode(false);
    m_pMovementCom->Set_ForceFriction(m_tPlayerStat.fForceFriction);

    m_pGrappleTarget = nullptr;
    m_tPlayerStat.bGrapplable = false;
    m_tPlayerStat.bGrappleAim = false;
    m_tPlayerStat.bGrappling = false;

    m_pPhysicsControllerCom->Set_UserData(false);
    static_cast<CEffect_Textured_Rope*>(m_PartObjects.find(L"Part_Rope")->second)->Clear_Info();
}

HRESULT CPlayer::Use_HeartKelp()
{
    // 하트켈프 사용시 아이템 생성하는 함수
    CItem_HeartKelp::DESC tItemDesc{};
    tItemDesc.ItemDesc.GameObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
    tItemDesc.pOwner = this;

    if (FAILED(Add_PartObject
    (
        L"Part_Item",
        LEVEL_STATIC,
        L"Prototype_GameObject_Item_HeartKelp",
        &tItemDesc
    )))
        return E_FAIL;

    m_pItem = static_cast<CItem_HeartKelp*>(Find_PartObject({ L"Part_Item" }));
    static_cast<CItem_HeartKelp*>(m_pItem)->Add_Socket(CItem_HeartKelp::SOCKET::SOCKET_FINGER, "../Bin/DataFiles/PartObjectData/HeartKelp_Finger.data");
    static_cast<CItem_HeartKelp*>(m_pItem)->MoveToSocket(CItem_HeartKelp::SOCKET::SOCKET_FINGER);

    return S_OK;
}

void CPlayer::UnUse_Item()
{
    Delete_PartObject({ L"Part_Item" });
    m_pItem = nullptr;
}

_bool CPlayer::Upgrade_Weapon()
{
    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    _uint iStanilessRelic = pUIManager->Get_StainlessRelic();

    _uint iUpgradeNeed = m_pPlayerWeapon->Get_WeaponStat_Const()->iLevel;
    if (iStanilessRelic < iUpgradeNeed)
        return false;

    m_pPlayerCineCamera->Play_Cinematic(L"UpgradeFork");

    iStanilessRelic -= iUpgradeNeed;
    m_pStateMachine->ChangeState(L"CutScene_UpgradeFork");
    m_pPlayerWeapon->Upgrade_Weapon();

    return true;
}

void CPlayer::MoonShell_Interact(_bool bEnter)
{
    Rest();

    if (bEnter)
    {
        m_pPlayerCineCamera->Play_Cinematic(L"Shelleport");
        m_pStateMachine->ChangeState(L"CutScene_MoonShellEnter");
    }
    else
    {
        CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
        pUIManager->FadeIn();

        m_pTransformCom->Locate(m_tPlayerStat.vShellPos);
        m_pPhysicsControllerCom->Set_Position(_vec3(m_tPlayerStat.vShellPos));

        _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

        _vec3 vDir = vPos + (_vec3(m_tPlayerStat.vShellLook) * -1.f).Normalized();
        m_pTransformCom->LookAt(vDir._vector());

        _vec3 vArmDir = (_vec3(m_tPlayerStat.vShellLook)).Normalized();
        m_pPlayerSpringArm->Reset_Rotation(vArmDir._vector());
        m_pPlayerSpringArm->Reset_Position(vPos._vector());

        m_pPlayerCineCamera->Stop_Cinematic();
        m_pStateMachine->ChangeState(L"CutScene_MoonShellExit");

        CTheme_Manager::Load_Theme_With_Sheleport_FromStatic(m_tPlayerStat.iShellNum);
    }
}

void CPlayer::Change_PrimaryActiveSkill(ACTIVESKILL eActiveSkill)
{
    m_tPlayerStat.ePrimaryActiveSkill = eActiveSkill;
    CUI* pUISkill = static_cast<CUI*>(Find_PartObject({ L"Part_UI", L"Part_UI_PlayerRight", L"Part_UI_BigSkill" }));

    if (eActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
    {
        pUISkill->Set_IsActive(false);
    }
    else
    {
        pUISkill->Set_IsActive(true);
        pUISkill->Change_TextureNum((_uint)eActiveSkill);
    }  
}

void CPlayer::Change_SecondaryActiveSkill(ACTIVESKILL eActiveSkill)
{
    m_tPlayerStat.eSecondaryActiveSkill = eActiveSkill;
    CUI* pUISkill = static_cast<CUI*>(Find_PartObject({ L"Part_UI", L"Part_UI_PlayerRight", L"Part_UI_SmallSkill" }));
    
    if (eActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
    {
        pUISkill->Set_IsActive(false);
    }
    else
    {
        pUISkill->Set_IsActive(true);
        pUISkill->Change_TextureNum((_uint)eActiveSkill);
    }
}

void CPlayer::UnLock_ActionSkill(ACTIONSKILL eActionSkill)
{
    CSkill_DataBase::Get().UnLock_ActionSkill(eActionSkill);
}

void CPlayer::UnLock_ActiveSkill(ACTIVESKILL eActiveSkill)
{
    CSkill_DataBase::Get().UnLock_ActiveSkill(eActiveSkill);

    if (m_tPlayerStat.ePrimaryActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
        Change_PrimaryActiveSkill(eActiveSkill);
    else if (m_tPlayerStat.eSecondaryActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
        Change_SecondaryActiveSkill(eActiveSkill);
}

void CPlayer::Use_ActiveSkill()
{
    m_ActiveSkills[static_cast<_uint>(m_tPlayerStat.ePrimaryActiveSkill)]->Use_Skill();
}

void CPlayer::Change_Hammer()
{
    if (m_tPlayerStat.bHasShell)
    {
        if (m_pPlayerWeapon->Get_WeaponStat_Const()->bHasShell)
        {
            SHELL ePlayerShell = m_pPlayerShellStat->eShell;
            SHELL eWeaponShell = m_pPlayerWeapon->Get_ShellStat_Const()->eShell;
            _float fPlayerShellHP = m_pPlayerShellStat->fShellCurrHP;
            _float fWeaponShellHP = m_pPlayerWeapon->Get_ShellStat_Const()->fShellCurrHP;

            Delete_PartObject({ L"Part_Shell" });

            Delete_Shell();
            Equip_Shell(eWeaponShell, fWeaponShellHP);

            m_pPlayerWeapon->Attach_Shell(ePlayerShell, fPlayerShellHP);
        }
        else
        {
            SHELL ePlayerShell = m_pPlayerShellStat->eShell;
            _float fPlayerShellHP = m_pPlayerShellStat->fShellCurrHP;

            Delete_Shell();
            m_pPlayerWeapon->Attach_Shell(ePlayerShell, fPlayerShellHP);
        }
    }
    else if (m_pPlayerWeapon->Get_WeaponStat_Const()->bHasShell)
    {
        SHELL eWeaponShell = m_pPlayerWeapon->Get_ShellStat_Const()->eShell;
        _float fWeaponShellHP = m_pPlayerWeapon->Get_ShellStat_Const()->fShellCurrHP;

        m_pPlayerWeapon->Deattach_Shell();
        Equip_Shell(eWeaponShell, fWeaponShellHP);
    }
}

void CPlayer::Reset_Rolling()
{
    m_pRollingTarget = nullptr;
}

void CPlayer::Reset_Finish()
{
    m_tPlayerStat.bFinishAttack = false;
    m_pFinishTarget = nullptr;
}

void CPlayer::Heal(_float fHeal)
{
    m_tPlayerStat.fCurrHP += fHeal;
    if (m_tPlayerStat.fCurrHP >= m_tPlayerStat.fMaxHP)
        m_tPlayerStat.fCurrHP = m_tPlayerStat.fMaxHP;
}

void CPlayer::Heal_Umami(_float fDamage)
{
    _float fHeal = fDamage * 0.01f;
    m_tPlayerStat.fCurrSkillPoint += fHeal;
    
    m_tPlayerStat.fCurrSkillPoint = m_tPlayerStat.fCurrSkillPoint >= m_tPlayerStat.fMaxSkillPoint ? m_tPlayerStat.fMaxSkillPoint : m_tPlayerStat.fCurrSkillPoint;
}

void CPlayer::Calculate_Stat()
{
    m_tPlayerStat.fMaxHP = (_float)m_tPlayerStat.iVitality * 5.f;
    m_tPlayerStat.fCurrHP = m_tPlayerStat.fMaxHP;
}

void CPlayer::Rest()
{
    m_tPlayerStat.fCurrHP = m_tPlayerStat.fMaxHP;
    m_tPlayerStat.fCurrSkillPoint = m_tPlayerStat.fMaxSkillPoint;

    m_tPlayerStat.iHookCount = m_tPlayerStat.iMaxHookCount;
    m_tPlayerStat.iHeartKelpCount = m_tPlayerStat.iMaxHeartKelpCount;
}

void CPlayer::Feared(_float fGauge)
{
    if (m_tPlayerStat.bFeared || m_tPlayerStat.bDeath)
        return;

    m_tPlayerStat.fFearGauge += fGauge;
    m_fDebuffLatency = 2.f;
    if (m_tPlayerStat.fFearGauge >= m_tPlayerStat.fFearMaxGauge)
    {
        m_tPlayerStat.bFeared = true;
        m_tPlayerStat.fFearGauge = m_tPlayerStat.fFearMaxGauge;
        Call_FearEffect();

        Play_PlayerSound(L"Feared_Voice");
        Play_PlayerSound_Loop(L"Feared_Loop");
    }

    if (m_tPlayerStat.bFeared)
    {
        m_PlayerState.set(PLAYERSTATE_DEBUFF_FEAR);

        if (m_tPlayerStat.bHasShell)
        {
            m_pStateMachine->ChangeState(L"ShellEnter");
            m_pUpperStateMachine->ChangeState(L"None");
        }
        else
        {
            _float fSpeed = m_pMovementCom->Get_HorizontalSpeed();
            if(fSpeed > 0.f)
                m_pStateMachine->ChangeState(L"Walk");
            else m_pStateMachine->ChangeState(L"Idle");
            
            m_pUpperStateMachine->ChangeState(L"Guard");
        }
    }
}

void CPlayer::Instant_Damage(_float fDamage)
{
    // Invincible 일 때 무적
    if (m_tPlayerStat.bParry || m_tPlayerStat.bInvincible || m_tPlayerStat.bDeath)
        return;

    if (m_tPlayerStat.bUseItem || m_tPlayerStat.bGrappleAim)
    {
        m_pUpperStateMachine->ChangeState(L"None");

        if (m_tPlayerStat.bUseItem)
            UnUse_Item();
    }

    _float fFinalDamage = fDamage - fDamage * ((_float)m_tPlayerStat.iDefense / 100.f);
    m_tPlayerStat.fCurrHP -= fFinalDamage;
}

void CPlayer::Shell_Damage(_float fDamage)
{
    if (m_pPlayerShell != nullptr)
    {
        m_pPlayerShell->Damaged(fDamage, DAMAGE::DAMAGE_NORMAL, nullptr);
        if (m_pPlayerShell->Get_ShellStat_Const()->fShellCurrHP <= 0.f)
            Delete_Shell();
    }
}

void CPlayer::KnockBack(_float fPower, DAMAGE eDamage, CGameObject* pDamager, _fvector vDir)
{
    _uint iDamage = static_cast<_uint>(eDamage);

    _vec3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    _vec3 vTargetDir{};
    
    if (_vec3(vDir).Length() != 0.f)
        vTargetDir = _vec3(vDir).Normalized();
    else
    {
        CTransform* pTransform = static_cast<CTransform*>(pDamager->Find_Component(g_strTransformTag));
        vTargetDir = (_vec3(pTransform->Get_State(CTransform::STATE::POSITION)) - vMyPos).Normalized();
    }

    m_pMovementCom->Reset_Velocity();

    switch (iDamage)
    {
    case (_uint)DAMAGE::DAMAGE_NORMAL :
    {
    }
    break;
    case (_uint)DAMAGE::DAMAGE_HEAVY :
    {
        vTargetDir.y = 0.f;

        _vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
        vLook.y = 0.f;

        _bool bIsFront{};
        _float fDot = _vec3::Dot(vLook, vTargetDir);
        if (fDot > 0) bIsFront = true;
        else bIsFront = false;

        if (bIsFront)
        {
            m_pStateMachine->ChangeState(L"HeavyAttacked_Back");
            m_pTransformCom->LookAt(vMyPos._vector() + vTargetDir._vector());
        }
        else
        {
            m_pStateMachine->ChangeState(L"HeavyAttacked_Front");
            m_pTransformCom->LookAt(vMyPos._vector() + vTargetDir._vector() * -1.f);
        }

        vTargetDir *= -1.f;
        vTargetDir.y += 2.f;
        m_pMovementCom->Add_Force(vTargetDir._vector(), fPower);
    }
    break;
    case (_uint)DAMAGE::DAMAGE_SUPER :
    {
        vTargetDir *= -1.f;
        vTargetDir.y += 2.f;

        m_pStateMachine->ChangeState(L"SuperAttacked");

        m_pTransformCom->LookAt(vMyPos._vector() + vTargetDir._vector());
        m_pMovementCom->Add_Force(vTargetDir._vector(), fPower);
    }
    break;
    default :
        break;
    }
}

void CPlayer::Input_Control(_float fTimeDelta)
{
//#ifdef _DEBUG

    if (m_tPlayerStat.bCheat)
    {
        Cheat_Control(fTimeDelta);
        return;
    }

//#endif

    // Part_Camera 값 받아오기
    _mat matTransform = m_pPlayerSpringArm->Get_PlayerCamera()->Get_CombinedMatrix();

    _vector vLook = _vec4(matTransform.look)._vector();
    _vector vRight = _vec4(matTransform.right)._vector();

    vLook = XMVectorSetY(vLook, 0.f);
    vRight = XMVectorSetY(vRight, 0.f);

    // 기본 움직임 조작
    // 기본 움직임 조작 가능 여부 체크
    {
        if (m_pGameInstance->Key_Press(KEY::W))
            m_pMovementCom->Add_InputDir(vLook);
        if (m_pGameInstance->Key_Press(KEY::S))
            m_pMovementCom->Add_InputDir(-vLook);
        if (m_pGameInstance->Key_Press(KEY::D))
            m_pMovementCom->Add_InputDir(vRight);
        if (m_pGameInstance->Key_Press(KEY::A))
            m_pMovementCom->Add_InputDir(-vRight);

        if (m_tPlayerStat.bMoveInputEnable)
        {
            _float3 vDir = m_pMovementCom->Get_InputDir();
            m_pMovementCom->Add_HorizontalVelocity(XMLoadFloat3(&vDir), fTimeDelta);
        }
    }

    if (m_tPlayerStat.bUseItem)
    {
        Lerp_MaxSpeed(-fTimeDelta);
        return;
    }

    // 달리기 / 회피
    {
        // InputGate에 키 값 저장
        if (m_pGameInstance->Key_Down(KEY::LSHIFT))
            m_pInputGate->Add_Input(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::LSHIFT), m_tPlayerStat.fRunTimeThreshold);

        // Tap인지 Hold인지 판단해서 Tap이면 InputBuffer에 키 값 저장
        if (m_pGameInstance->Key_Up(KEY::LSHIFT))
        {
            if (m_pInputGate->Tap(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::LSHIFT)))
                m_pInputBuffer->Add_Input(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::LSHIFT));
            else if (m_pInputGate->Hold(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::LSHIFT))) {}
        }

        // 쉬프트 누르면 MaxSpeed 증가, 아니면 초기화
        if (m_pGameInstance->Key_Press(KEY::LSHIFT) && m_tPlayerStat.bRunnable && !m_tPlayerStat.bGuard)
            Lerp_MaxSpeed(fTimeDelta);
        else if (m_tPlayerStat.bRunnable || m_tPlayerStat.bGuard || m_tPlayerStat.bGrappleAim)
            Lerp_MaxSpeed(-fTimeDelta);

        // 마지막 버퍼 확인해서 있으면 Evade 상태로 전환
        if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::LSHIFT)))
        {
            if (!m_tPlayerStat.bInAir || m_tPlayerStat.bCanAirAction)
            {
                if (m_pMovementCom->Has_Input())
                    m_pStateMachine->ChangeState(L"Evade");
                else m_pStateMachine->ChangeState(L"BackStep");
                m_pUpperStateMachine->ChangeState(L"None");
            }
        }
    }

    // 공중제어
    {
        if (m_pGameInstance->Key_Down(KEY::SPACE))
        {
            // 공중 상태가 아니면 Space 키 누를 시 점프
            if (!m_tPlayerStat.bInAir)
                m_pStateMachine->ChangeState(L"JumpLaunch");
            // 공중에 있는 상태에서 Space 키 누르면 수영
            else if (!m_tPlayerStat.bSwimming && m_tPlayerStat.bSwimmable)
                m_pStateMachine->ChangeState(L"Swimming");

            if(m_tPlayerStat.bSwimmable)
                m_pMovementCom->Set_MaxFallingSpeed(m_tPlayerStat.fSwimmingFallingSpeed);
        }
        // 수영하다가 Space 키 떼면 Airbone
        else if (m_pGameInstance->Key_Up(KEY::SPACE))
        {
            m_pMovementCom->Set_MaxFallingSpeed(m_tPlayerStat.fMaxFallingSpeed);

            if(m_tPlayerStat.bInAir && m_tPlayerStat.bSwimming)
                m_pStateMachine->ChangeState(L"Airbone");
        }
        // Space 계속 누르고 있으면 Airbone으로 전환하자마자 Swimming으로 상태 전환
        else if (m_pGameInstance->Key_Press(KEY::SPACE) &&
            m_pModelCom->Get_CurrAnimation()->Get_Blendable() && m_tPlayerStat.bSwimmable)
            m_pStateMachine->ChangeState(L"Swimming");
    }

    // 락온
    {
        // 스프링암에서 락온 체크한뒤 플레이어 락온 체크
        if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::DIM_MB))
        {
            m_pPlayerSpringArm->Check_LockOnTarget();
            Player_CheckLockOn();
        }
    }

    // 공격
    if(m_pPlayerWeapon->Get_Active())
    {
        // InputBuffer에 입력 저장 : 마우스 왼클릭
        if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::DIM_LB))
            m_pInputBuffer->Add_Input(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB);

        if (m_pFinishTarget != nullptr && CSkill_DataBase::Get().Get_ActionSkill(ACTIONSKILL::ACTIONSKILL_FINISH).bUnLocked)
        {
            if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB))
            {
                m_pStateMachine->ChangeState(L"FinishAttack");

                _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
                XMStoreFloat3(&m_vFinishStart, vPos);
            }
        }
        // Parry 했을 때
        else if (m_tPlayerStat.bCanParryAttack)
        {
            if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB))
                m_pStateMachine->ChangeState(L"ParryAttack");
        }
        else if (m_tPlayerStat.bCanBackStepAttack)
        {
            if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB))
            {
                CTransform* pTransform = static_cast<CTransform*>(m_pBackStepTarget->Find_Component(g_strTransformTag));
                _vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
                _vec3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
                vPos.y = 0.f;

                m_pTransformCom->LookAt((vMyPos + vPos)._vector());
                m_pMovementCom->Reset_Velocity();

                m_pStateMachine->ChangeState(L"BackStepAttack");
            }
        }
        // 공중 상태일 때 SwipeAttack
        else if (m_tPlayerStat.bInAir && m_tPlayerStat.bCanAirAction)
        {
            if (m_tPlayerStat.iComboCount == 0)
            {
                if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB))
                    m_pStateMachine->ChangeState(L"SwipeAttack");
            }
            else if (m_pGameInstance->Mouse_Press(MOUSEKEYSTATE::DIM_LB) && m_tPlayerStat.bCharging)
            {
                m_tPlayerStat.iComboCount = 0;
                m_pStateMachine->ChangeState(L"PlungeAttack");
            }
        }
        // 달리기 상태일 때 DashAttack
        else if (m_tPlayerStat.bRunning)
        {
            if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB))
                m_pStateMachine->ChangeState(L"DashAttack");
        }
        // 첫 공격 -> 달리기나 공중 상태가 아니면 무조건 차지공격으로 일단 시작하고 InputGate에 키값 저장
        // 일반 상태일 때 ChargeAttack / NormalAttack
        else if (m_tPlayerStat.iComboCount == 0)
        {
            if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB))
                m_pStateMachine->ChangeState(L"ChargeAttack");
        }
        // Charging 중일 때 Tap이면 NormalAttack으로 전환
        else if (m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::DIM_LB) && m_tPlayerStat.bCharging)
        {
            if (!m_tPlayerStat.bHasHammer)
                m_tPlayerStat.iComboCount = 0;
            m_pStateMachine->ChangeState(L"NormalAttack");
        }

        // Transition 가능일 때 NormalAttack 상태 전환
        if (m_tPlayerStat.bTransition)
        {
            if (m_pInputBuffer->Is_FirstInput(INPUTTYPE::INPUTTYPE_MOUSE, MOUSEKEYSTATE::DIM_LB))
                m_pStateMachine->ChangeState(L"NormalAttack");
        }
    }

    // 가드 / 패링
    if(!m_tPlayerStat.bFeared)
    {
        if (m_pGameInstance->Mouse_Press(MOUSEKEYSTATE::DIM_RB) && !m_tPlayerStat.bGuard)
        {
            if(m_tPlayerStat.bHasShell)
                m_pStateMachine->ChangeState(L"ShellEnter");
            else
                m_pUpperStateMachine->ChangeState(L"Guard");
        }
        else if (!m_pGameInstance->Mouse_Press(MOUSEKEYSTATE::DIM_RB) && m_tPlayerStat.bGuard)
        {
            if (m_tPlayerStat.bHasShell)
                m_pStateMachine->ChangeState(L"ShellParry");
            else
            {
                m_pStateMachine->ChangeState(L"Parry");
                m_pUpperStateMachine->ChangeState(L"None");
            }
        }
    }

    // 등껍질 / 상호작용
    if(!m_tPlayerStat.bFeared)
    {
        // 상호작용 / 등껍질 벗기
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            // E키 누르면 bInteract 변수 활성화되면서 상호작용 가능
            if (!m_tPlayerStat.bInteract)
                m_tPlayerStat.bInteract = true;

            // 등껍질 갖고있는 상태면 InputGate에 키값 저장
            if (m_tPlayerStat.bHasShell)
                m_pInputGate->Add_Input(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::E), 1.f);
        }
        else
        {
            // E키를 누른 상태가 아니고 bInteract가 활성화되어있으면 비활성화
            if (m_tPlayerStat.bInteract)
                m_tPlayerStat.bInteract = false;

            // 등껍질 갖고있는 상태
            if (m_tPlayerStat.bHasShell)
            {
                // E키 떼면 Tap인지 검사 -> 키 뗐을 때 InputGate 값 비워주기 위함
                if (m_pGameInstance->Key_Up(KEY::E))
                {
                    if (m_pInputGate->Tap(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::E))) {};
                }
                // E키 뗀 상태 아니면 Hold 검사해서 true면 현재 갖고 있는 등껍질 UnEquip
                else if (m_pInputGate->Hold(INPUTTYPE::INPUTTYPE_KEYBOARD, static_cast<_uint>(KEY::E)))
                    UnEquip_Shell();
            }
        }

        // 등껍질 스킬
        if (m_pGameInstance->Key_Down(KEY::LCONTROL) && m_tPlayerStat.bHasShell)
        {
            if (m_tPlayerStat.fCurrSkillPoint >= m_pPlayerShell->Get_SkillPoint())
            {
                _wstring strSkill = Get_PlayerShellStat_Const()->strSkillName;
                m_pStateMachine->ChangeState(strSkill);
            }
        }
    }

    // 갈고리
    if (m_pPlayerWeapon->Get_Active() && !m_tPlayerStat.bFeared)
    {
        if (m_pGameInstance->Key_Press(KEY::F) && !m_tPlayerStat.bGrappleAim)
            m_pUpperStateMachine->ChangeState(L"Grapple_Aim");
        else if (!m_pGameInstance->Key_Press(KEY::F) && m_tPlayerStat.bGrappleAim)
        {
            m_pUpperStateMachine->ChangeState(L"None");

            //if (m_tPlayerStat.iHookCount > 0)
            //{
                //m_pStateMachine->ChangeState(L"Grapple_Throw");
                Grapple_Throw();
            //}
        }
    }

    // 아이템
    if(m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_BEACH)
    {
        // 하트켈프
        if (m_pGameInstance->Key_Down(KEY::R))
            if (m_tPlayerStat.iHeartKelpCount > 0)
                m_pUpperStateMachine->ChangeState(L"Item_HeartKelp");
    }

    // 해머
    if(m_pPlayerWeapon->Get_Active())
    {
        if (CSkill_DataBase::Get().Get_ActionSkill(ACTIONSKILL::ACTIONSKILL_HAMMER).bUnLocked)
        {
            if (m_pGameInstance->Key_Down(KEY::V))
            {
                if (m_tPlayerStat.bHasShell || m_pPlayerWeapon->Get_WeaponStat_Const()->bHasShell)
                {
                    if (m_tPlayerStat.bCanHammerAnim)
                    {
                        m_pModelCom->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, true);
                        m_pModelCom->SetUp_SlotAnimation(ANIMSLOT::SLOT_HAMMER, m_pModelCom->Get_AnimationIndex("KirkHammer"), false, 0.15f);
                    }
                }
            }
        }
    }

    // 액티브 스킬
    if(!m_tPlayerStat.bFeared)
    {
        if (m_tPlayerStat.ePrimaryActiveSkill!= ACTIVESKILL::ACTIVESKILL_END && 
            m_ActiveSkills[static_cast<_uint>(m_tPlayerStat.ePrimaryActiveSkill)] != nullptr)
        {
            if (m_pGameInstance->Key_Down(KEY::Q))
            {
                if (m_tPlayerStat.fCurrSkillPoint >=
                    CSkill_DataBase::Get().Get_ActiveSkill(m_tPlayerStat.ePrimaryActiveSkill).fSkillPoint)
                {
                    CUI_SkillEffect* pUISkillEffect = static_cast<CUI_SkillEffect*>(Find_PartObject({ L"Part_UI", L"Part_UI_PlayerRight", L"Part_UI_SkillEffect"}));
                    pUISkillEffect->Show();

                    _wstring strSkill = CSkill_DataBase::Get().Get_ActiveSkill(m_tPlayerStat.ePrimaryActiveSkill).strActiveSkill;
                    m_pStateMachine->ChangeState(strSkill);
                }
            }
        }
    }

//#ifdef _DEBUG

    // 디버깅
    {
        //// 피격 테스트
        //if (m_pGameInstance->Key_Down(KEY::B))
        //    Damaged(10.f, DAMAGE::DAMAGE_HEAVY, this);

        // 카메라 쉐이킹 테스트
        //if (m_pGameInstance->Key_Down(KEY::P))
        //    Player_CameraShake_Position(0.05f, 20.f, 0.3f);
        //if (m_pGameInstance->Key_Down(KEY::O))
        //    Player_CameraShake_Rotation(0.01f, 20.f, 0.3f);

        // 시네마틱 카메라 테스트
        //if (m_pGameInstance->Key_Down(KEY::I))
        //{
        //    if (!m_pPlayerCineCamera->Get_Playing())
        //    {
        //        m_pPlayerCineCamera->Play(L"Test", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
        //        m_pGameInstance->Camera_Transition(L"CineCamera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
        //        //m_pPlayerCineCamera->Play(L"Test2", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
        //    }
        //    else
        //        m_pPlayerCineCamera->Stop();
        //}

        //// 체력 디버깅용
        //if (m_pGameInstance->Key_Press(KEY::O))
        //    m_tPlayerStat.fCurrHP -= 1.f;
        //if (m_pGameInstance->Key_Press(KEY::P))
        //    m_tPlayerStat.fCurrHP += 1.f;

        //// 등껍질 체력 디버깅용
        //if (m_pGameInstance->Key_Press(KEY::O))
        //    m_pPlayerShell->Damaged(1.f);
        //else if (m_pGameInstance->Key_Press(KEY::P))
        //    m_pPlayerShell->Damaged(-1.f);

        //// 스킬 포인트 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::I))
        //    m_tPlayerStat.iCurrSkillPoint++;

        //// 하트켈프 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::O))
        //    m_tPlayerStat.iHeartKelpCount++;

        //// 무기 업그레이드 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::P))
        //    Upgrade_Weapon();

        //// 문쉘 상호작용 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::O))
        //    m_pStateMachine->ChangeState(L"CutScene_MoonShellEnter");
        //if (m_pGameInstance->Key_Down(KEY::P))
        //    m_pStateMachine->ChangeState(L"CutScene_MoonShellExit");

        //// 디버프 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::P))
        //    Feared(5.f);

        //if (m_pGameInstance->Key_Down(KEY::P))
        //    m_pModelCom->Get_CurrAnimation()->Set_Active(true);

        //// 사망 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::P))
        //{
        //    Instant_Damage(999.f);
        //    m_pStateMachine->ChangeState(L"NormalAttacked");
        //}

        // 컷씬 디버깅용
        if (m_pGameInstance->Key_Down(KEY::O))
            m_pStateMachine->ChangeState(L"ParryAttack");
        if (m_pGameInstance->Key_Down(KEY::P))
            m_pStateMachine->ChangeState(L"BackStepAttack");

        // 렌더 On/Off 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::O))
        //    m_bRender = !m_bRender;

        //// 이펙트 테스트
        //if (m_pGameInstance->Key_Down(KEY::F4))
        //    Call_ParryEffect();

        //// 등껍질 데미지 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::P))
        //    Shell_Damage(999.f);

        //// 데미지 디버깅용
        //if (m_pGameInstance->Key_Down(KEY::P))
        //    Attacked(false, 999.f, DAMAGE::DAMAGE_NORMAL, this);
    }

//#endif
}

void CPlayer::Cheat_Control(_float fTimeDelta)
{
    // Part_Camera 값 받아오기
    _mat matTransform = m_pPlayerSpringArm->Get_PlayerCamera()->Get_CombinedMatrix();

    _vector vLook = _vec4(matTransform.look)._vector();
    _vector vRight = _vec4(matTransform.right)._vector();
    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);

    // 기본 움직임 조작
    // 기본 움직임 조작 가능 여부 체크
    {
        _vector vDir{};

        vLook = XMVectorSetY(vLook, 0.f);
        vRight = XMVectorSetY(vRight, 0.f);

        if (m_pGameInstance->Key_Press(KEY::W))
            m_pMovementCom->Add_InputDir(vLook);
        if (m_pGameInstance->Key_Press(KEY::S))
            m_pMovementCom->Add_InputDir(-vLook);
        if (m_pGameInstance->Key_Press(KEY::D))
            m_pMovementCom->Add_InputDir(vRight);
        if (m_pGameInstance->Key_Press(KEY::A))
            m_pMovementCom->Add_InputDir(-vRight);

        if (m_pGameInstance->Key_Press(KEY::SPACE))
            m_pMovementCom->Add_InputDir(vUp);
        if (m_pGameInstance->Key_Press(KEY::LCONTROL))
            m_pMovementCom->Add_InputDir(-vUp);

        if (m_tPlayerStat.bMoveInputEnable)
        {
            _float3 vDir = m_pMovementCom->Get_InputDir();
            m_pMovementCom->Add_HorizontalVelocity(XMLoadFloat3(&vDir), fTimeDelta * 1.5f);
            m_pMovementCom->Add_VerticalVelocity(XMLoadFloat3(&vDir), fTimeDelta * 1.5f);
        }
    }

    // 가속 / 감속
    {
        if (m_pGameInstance->Key_Down(KEY::LSHIFT))
        {
            _float fRatio = m_tPlayerStat.fMaxSpeed * m_tPlayerStat.fCheatMaxSpeedRatio;
            m_pMovementCom->Set_Accel(fRatio);
            m_pMovementCom->Set_Decel(fRatio * 0.5f);
            m_pMovementCom->Set_MaxSpeed(fRatio);
        }
        else if (m_pGameInstance->Key_Up(KEY::LSHIFT))
        {
            _float fRatio = m_tPlayerStat.fMaxSpeed * m_tPlayerStat.fCheatSpeedRatio;
            m_pMovementCom->Set_Accel(fRatio);
            m_pMovementCom->Set_Decel(fRatio * 0.5f);
            m_pMovementCom->Set_MaxSpeed(fRatio);
        }
    }
}

void CPlayer::Call_ParryEffect()
{
    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);
    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION) + vUp * 0.5f;
    _vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    m_pGameInstance->Call_Lag(0.08f, 0.15f);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_RED, vPos, vLook, false, nullptr);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_BLUE, vPos, vLook, false, nullptr);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_YELLOW, vPos, vLook, false, nullptr);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_REDBIGONE, vPos, vLook, false, nullptr);

    _vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
    vLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vLook, vLook, false, nullptr);
}

void CPlayer::Call_BackStepEffect()
{
    m_pGameInstance->Call_Lag(0.08f, 0.15f);
}

void CPlayer::Call_BubbleEnvEffect(_float fTimeDelta)
{
    m_fBubbleGenTimeAcc += fTimeDelta;
    if (m_fBubbleGenTimeAcc >= m_fBubbleGenTime)
    {
        m_fBubbleGenTimeAcc = 0.f;
        _vector vPos{}, vUp{};
        vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, XMVectorSetY(vPos, 0.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
    }
}

void CPlayer::Call_GrappleEffect(_fvector vPos)
{
    _vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROPEACTION_BLUEBIG, vPos, vLook, false, nullptr);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vLook, false, nullptr);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROPEACTION_DIAMOND, vPos, vLook, false, nullptr);
    _vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
    vLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vLook, vLook, false, nullptr);
}


void CPlayer::Call_DamagedEffect(IDamaged::DAMAGE iDamageType)
{
    _vector vCamPos{}, vCamLook{};
    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);
    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION) + vUp * 0.5f;
    _vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    switch (iDamageType)
    {
    case IDamaged::DAMAGE::DAMAGE_NORMAL:
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_SPRT_DAMAGED_REDSPARK, vPos, vLook, false, nullptr);
        break;
    case IDamaged::DAMAGE::DAMAGE_HEAVY:
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DAMAGED_REDSPARK, vPos, vLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_SPRT_DAMAGED_REDSPARK, vPos, vLook, false, nullptr);
        break;
    case IDamaged::DAMAGE::DAMAGE_SUPER:
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DAMAGED_BLOOD, vPos, vLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DAMAGED_REDSPARK, vPos, vLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_SPRT_DAMAGED_REDSPARK, vPos, vLook, false, nullptr);
        break;
    default:
        break;
    }
    vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
    vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
    CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook + vUp * 0.5f, vCamLook, false, nullptr);
}


void CPlayer::Call_FearEffect()
{
    if (nullptr != m_pTearEffect)
        Disable_FearEffect();

    _vector vOffsetPos{}, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };

    vOffsetPos = m_pTransformCom->Get_State(CTransform::STATE::UP) * 0.5f;
    m_pTearEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DEBUF_FEAR, vOffsetPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
} 

void CPlayer::Disable_FearEffect()
{
    if (nullptr != m_pTearEffect)
        m_pTearEffect->Clear_Info(0.f);
}

void CPlayer::Call_GrappleHitEffect()
{
    _vector vTargetPos{}, vPos{}, vLook{}, vZeroPos{};
    vTargetPos = static_cast<CTransform*>(m_pGrappleTarget->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
    vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    vLook = XMVectorSetW(XMVector3Normalize(vTargetPos - vPos), 0.f);

    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_ROPEACTION_DAMAGEIMPACT, vPos, vLook, false, nullptr);
}

void CPlayer::AutoLock_Movement(_float fTimeDelta)
{
    _vector vPos = m_pAutoLockTransform->Get_State(CTransform::STATE::POSITION);
    XMVectorSetY(vPos, 0.f);

    m_pMovementCom->TurnToDirection(vPos, fTimeDelta);
}

void CPlayer::Lerp_MaxSpeed(_float fTimeDelta)
{
    if (m_tPlayerStat.bHasShell && m_tPlayerStat.bGuard)
        return;

    m_tPlayerStat.fRunTimeAcc += fTimeDelta;
    if (m_tPlayerStat.fRunTimeAcc > m_tPlayerStat.fRunTimeThreshold)
        m_tPlayerStat.fRunTimeAcc = m_tPlayerStat.fRunTimeThreshold;
    else if (m_tPlayerStat.fRunTimeAcc < 0.f)
        m_tPlayerStat.fRunTimeAcc = 0.f;

    _float fRatio = m_tPlayerStat.fRunTimeAcc / m_tPlayerStat.fRunTimeThreshold;
    _float fRatioDist = m_tPlayerStat.fRunMaxSpeedRatio - 1.f;

    m_pMovementCom->Set_MaxSpeed(m_tPlayerStat.fMaxSpeed * (1 + fRatioDist * fRatio));
}

void CPlayer::GrappleAim_Movement()
{
    if (m_tPlayerStat.bGrappleAim)
    {
        _float3 vRotation = static_cast<CTransform*>(m_pPlayerSpringArm->Find_Component(g_strTransformTag))->Get_Rotation();
        m_pTransformCom->Rotate(0.f, vRotation.y, 0.f);
    }
}

void CPlayer::Finish_Movement(_float fTimeDelta)
{
    if (m_pFinishTarget != nullptr && m_tPlayerStat.bFinishAttack)
    {
        m_fFinishLerp += fTimeDelta;
        if (m_fFinishLerp >= m_tPlayerStat.fFinishTime)
            m_fFinishLerp = m_tPlayerStat.fFinishTime;

        if (m_fFinishLerp <= m_tPlayerStat.fFinishTime * 0.5f)
            m_pPhysicsControllerCom->Set_UserData(true);
        else
            m_pPhysicsControllerCom->Set_UserData(false);

        _vec3 vStart = _vec3(m_vFinishStart);
        _vec3 vEnd = static_cast<CCollider*>(m_pFinishTarget->Find_Component(L"Com_Collider"))->Get_BoundingCenter();

        _vec3 vPoint = CUtils::Lerp_Parabolic(vStart._vector(), vEnd._vector(), 10.f, m_fFinishLerp / m_tPlayerStat.fFinishTime);
        Move_Player(vPoint._vector());
    }
    else if (m_fFinishLerp != 0.f)
        m_fFinishLerp = 0.f;
}

void CPlayer::Delete_Shell()
{
    Delete_PartObject({ L"Part_Shell" });

    m_pPlayerShell = nullptr;
    m_tPlayerStat.bHasShell = false;

    m_pPlayerShellStat = nullptr;
    m_tPlayerStat.iDefense = 0;
}

void CPlayer::Grapple_Throw()
{
    _float4 vOrigin = _mat(m_pPlayerWeapon->Get_CombinedMatrix()).position._float4();

    CProjectile_Manager* pManager = static_cast<CProjectile_Manager*>(m_pGameInstance->Find_GameObject
    (
        m_pGameInstance->Get_CurrentLevelIndex(),
        L"Layer_Projectile"
    ));

    FProjectileStat tProjectileStat{};

    tProjectileStat.vSrc = _float3(vOrigin.x, vOrigin.y, vOrigin.z);
    tProjectileStat.fForce = m_tPlayerStat.fGrappleForce;

    _vec3 vPos{};
    if (m_tPlayerStat.bGrapplable)
    {
        IGrapple* pGrapple = dynamic_cast<IGrapple*>(m_pGrappleTarget);
        if (pGrapple == nullptr)
            return;

        vPos = pGrapple->Get_GrapplePos();
        tProjectileStat.vDst = vPos._float3();

        _uint iType = pGrapple->Get_Type();

        if (iType == static_cast<_uint>(COLLISION_GROUP::GRAPPLENEEDLE))
        {
            m_pStateMachine->ChangeState(L"Grapple_Throw");
            m_pPlayerCineCamera->Play_Cinematic(L"Grapple");
        }
        if (iType == static_cast<_uint>(COLLISION_GROUP::MONSTER))
        {
            if (m_tPlayerStat.iHookCount <= 0)
                return;

            static_cast<CEnemy*>(m_pGrappleTarget)->Hooked();
            //m_tPlayerStat.iHookCount--;
            CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
            pUIManager->Get_ShopItemInventory()->Delete(2, 1);

            m_pStateMachine->ChangeState(L"Grapple_Throw");
            m_pPlayerCineCamera->Play_Cinematic(L"Grapple");
        }
        else if (iType == static_cast<_uint>(COLLISION_GROUP::SHELL))
        {
            m_pStateMachine->ChangeState(L"Grapple_Throw");
            static_cast<CShell_World*>(m_pGrappleTarget)->Stop();
        }
        else
            m_pStateMachine->ChangeState(L"Grapple_Throw");
    }
    else
    {
        _matrix matTransform = XMLoadFloat4x4(&m_pPlayerSpringArm->Get_PlayerCamera()->Get_CombinedMatrix());

        _vec3 vCamPos = _mat(matTransform).position;
        _vec3 vCamLook = _mat(matTransform).look;

        vPos = vCamPos + vCamLook.Normalized() * m_tPlayerStat.fGrappleDist * 0.7f;
    }

    tProjectileStat.vDst = vPos._float3();

    _float fDist = (_vec3(tProjectileStat.vSrc) - _vec3(tProjectileStat.vDst)).Length();
    tProjectileStat.fLifeTime = fDist / m_tPlayerStat.fGrappleForce + 2.f;

    pManager->Spawn(PROJECTILE::PROJECTILE_GRAPPLE, 1, tProjectileStat);
}

void CPlayer::Grapple_Boost()
{
    Reset_XZRotation();

    m_pStateMachine->ChangeState(L"Rising");
    m_tPlayerStat.bGrappling = false;

    m_pMovementCom->FlyMode(false);
    m_pMovementCom->Reset_Velocity();
    m_pMovementCom->Set_ForceFriction(m_tPlayerStat.fForceFriction);

    _vector vDir = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
    
    m_pMovementCom->Add_Force(vDir, m_tPlayerStat.fGrappleBoost * 0.5f);
    m_pMovementCom->Add_Force(XMVectorSetY(XMVectorZero(), 1.f), m_tPlayerStat.fGrappleBoost);

    m_pPhysicsControllerCom->Set_UserData(false);
}

void CPlayer::Check_GrappleTarget()
{
    if (m_tPlayerStat.bGrappling)
        return;

    m_pGrappleTarget = nullptr;
    m_tPlayerStat.bGrapplable = false;

    //_vector vPosOrigin = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    _vector vPosOrigin = m_pColliderCom->Get_BoundingCenter()._vector();

    list<CGameObject*> listObject;
    if (CSkill_DataBase::Get().Get_ActionSkill(ACTIONSKILL::ACTIONSKILL_FISHING).bUnLocked)
    {
        for (auto& Layer : m_mapGrappleLayer)
            m_pPlayerSpringArm->Get_PlayerCamera()->Get_CulledTargetList(Layer.first, Layer.second, listObject, vPosOrigin);
    }
    else
    {
        _wstring strLayer = L"Layer_InteractionObject";
        m_pPlayerSpringArm->Get_PlayerCamera()->Get_CulledTargetList(strLayer, static_cast<_uint>(COLLISION_GROUP::GRAPPLENEEDLE), listObject, vPosOrigin);
    }

    _float fPoint = FLT_MAX;
    _float fDist = FLT_MAX;
    for (auto& Object : listObject)
    {
        _float fCurrPoint{};

        IGrapple* pGrapple = dynamic_cast<IGrapple*>(Object);
        if (pGrapple == nullptr)
            return;

        _uint iType = pGrapple->Get_Type();
        switch (iType)
        {
        case (_uint)COLLISION_GROUP::MONSTER:
        {
            if (static_cast<CEnemy*>(Object)->IsCapsized())
                return;

            fCurrPoint += 10.f;
        }
        break;
        case (_uint)COLLISION_GROUP::GRAPPLENEEDLE:
        {
            fCurrPoint += 20.f;
        }
        break;
        case (_uint)COLLISION_GROUP::SHELL:
        {
            fCurrPoint += 30.f;
        }
        break;
        default:
            break;
        }

        _vec3 vTargetPos = pGrapple->Get_GrapplePos();
        _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

        _float fCurrDist = _vec3(vTargetPos - vPos).Length();
        if (fCurrDist <= m_tPlayerStat.fGrappleCheckDist)
        {
            _float4x4 matView = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW);
            _float4x4 matProj = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ);
            _float2 vViewSize = _float2
            (
                static_cast<_float>(m_pGameInstance->Get_ViewportSize().x),
                static_cast<_float>(m_pGameInstance->Get_ViewportSize().y)
            );

            _float2 vScreenPos = CUtils::Get_WorldToScreenPos
            (
                vTargetPos._vector(),
                XMLoadFloat4x4(&matView),
                XMLoadFloat4x4(&matProj),
                vViewSize
            );

            vScreenPos = _float2(fabs(vScreenPos.x - vViewSize.x * 0.5f), fabs(vScreenPos.y - vViewSize.y * 0.5f));
            _float fScreenDist = _vec2(vScreenPos).Length();

            if (fCurrDist > m_tPlayerStat.fGrappleDist)
                fCurrPoint += 5000.f;

            fCurrPoint += fScreenDist;
            //fCurrPoint += fCurrDist;
           
            if (fCurrPoint <= fPoint)
            {
                m_pGrappleTarget = Object;
                fPoint = fCurrPoint;

                if (fCurrDist <= m_tPlayerStat.fGrappleDist)
                    m_tPlayerStat.bGrapplable = true;
                else m_tPlayerStat.bGrapplable = false;
            }
        }
    }
}

void CPlayer::Hammer_SlotAnim()
{
    if (m_tPlayerStat.bCanHammerAnim)
    {
        if (m_pModelCom->Is_SlotAnimActive(ANIMSLOT::SLOT_HAMMER))
        {
            if (m_pModelCom->Get_SlotAnimationIndex(ANIMSLOT::SLOT_HAMMER) == m_pModelCom->Get_AnimationIndex("Idle") &&
                m_pModelCom->Is_SlotState(ANIMSLOT::SLOT_HAMMER) == (_uint)(CModel::STATE::ANIMATION))
                m_pModelCom->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, false);
            else if (m_pModelCom->Get_SlotAnimationIndex(ANIMSLOT::SLOT_HAMMER) == m_pModelCom->Get_AnimationIndex("KirkHammer") &&
                m_pModelCom->Get_SlotAnimation(ANIMSLOT::SLOT_HAMMER)->Get_Blendable())
            {
                if (m_tPlayerStat.bHasHammer)
                {
                    m_pModelCom->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, true);
                    m_pModelCom->SetUp_SlotAnimation(ANIMSLOT::SLOT_HAMMER, m_pModelCom->Get_AnimationIndex("HammerIdle"), true, 0.15f);
                }
                else
                    m_pModelCom->SetUp_SlotAnimation(ANIMSLOT::SLOT_HAMMER, m_pModelCom->Get_AnimationIndex("Idle"), false, 0.15f);
            }
        }
        else if (m_tPlayerStat.bHasHammer)
        {
            m_pModelCom->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, true);
            m_pModelCom->SetUp_SlotAnimation(ANIMSLOT::SLOT_HAMMER, m_pModelCom->Get_AnimationIndex("HammerIdle"), true, 0.15f);
        }
    }
    else
    {
        if (m_pModelCom->Is_SlotAnimActive(ANIMSLOT::SLOT_HAMMER))
        {
            if (m_pModelCom->Get_SlotAnimationIndex(ANIMSLOT::SLOT_HAMMER) == m_pModelCom->Get_AnimationIndex("KirkHammer"))
                m_pModelCom->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, false);
            else if (m_tPlayerStat.bHasHammer)
                m_pModelCom->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, false);
        }
    }
}

void CPlayer::Magnet_Pulled()
{
    _vec3 vMagnetPos = m_pMagnetTransform->Get_State(CTransform::STATE::POSITION);
    _vec3 vMagnetUp = m_pMagnetTransform->Get_State(CTransform::STATE::UP);
    vMagnetPos = vMagnetPos + vMagnetUp * -15.f;

    if (!m_tPlayerStat.bMagnetPulled)
    {
        m_fMagnetLerp += 0.03f;
        if (m_fMagnetLerp >= 1.f) m_fMagnetLerp = 1.f;

        _float fLerp = 1 + sinf(XMConvertToRadians(-90.f + 90.f * m_fMagnetLerp));

        //_vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        _vec3 vPos = m_pColliderCom->Get_BoundingCenter();
        vPos = XMVectorLerp(vPos._vector(), vMagnetPos._vector(), fLerp);
        m_pPhysicsControllerCom->Set_Position(vPos);

        m_pTransformCom->Locate(vPos._float3());

        _float fDist = (vMagnetPos - vPos).Length();
        if (fDist <= 1.f)
        {
            m_tPlayerStat.bMagnetPulled = true;
            m_tPlayerStat.bMoveInputEnable = false;

            m_fMagnetLerp = 0.1f;
        }

        m_pPlayerSpringArm->Zoom(m_pPlayerSpringArm->Get_DefaultArmLength() * 2.f);
    }
    else
    {
        m_pTransformCom->Locate(vMagnetPos._float3());
        m_pPhysicsControllerCom->Set_Position(vMagnetPos);
    }
}

void CPlayer::Magnet_UnPulled()
{
    m_tPlayerStat.bMagnetPulling = false;
    m_tPlayerStat.bMagnetPulled = false;
    m_tPlayerStat.bMoveInputEnable = true;
    m_pMagnetTransform = nullptr;
    m_fMagnetLerp = 0.1f;

    m_pMovementCom->Set_GravityScale(m_tPlayerStat.fGravityScale);

    m_pPlayerSpringArm->Zoom(m_pPlayerSpringArm->Get_DefaultArmLength());
}

void CPlayer::Check_FinishTarget()
{
    if (!CSkill_DataBase::Get().Get_ActionSkill(ACTIONSKILL::ACTIONSKILL_FINISH).bUnLocked)
        return;

    if (m_PlayerState.test(PLAYERSTATE_FINISHATTACK) || m_tPlayerStat.bFinishAttack)
        return;

    m_pFinishTarget = nullptr;

    list<CGameObject*> listEnemy = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Monster");

    for (auto& Enemy : listEnemy)
    {
        CEnemy* pEnemy = static_cast<CEnemy*>(Enemy);
        CCollider* pCollider = static_cast<CCollider*>(pEnemy->Find_Component(L"Com_Collider"));

        _float fDist = m_tPlayerStat.fFinishDist;
        if (pEnemy->IsCapsized() && !pEnemy->IsFinished() && pCollider->Is_Active())
        {
            _vec3 vEnemyPos = pCollider->Get_BoundingCenter();
            _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

            _float fTargetDist = (vEnemyPos - vPos).Length();
            if (fTargetDist < fDist)
                m_pFinishTarget = Enemy;
        }
    }
}

void CPlayer::Grappling_Collide()
{
    if (m_tPlayerStat.bGrappling && m_pGrappleTarget != nullptr)
    {
        IGrapple* pGrapple = dynamic_cast<IGrapple*>(m_pGrappleTarget);
        if (pGrapple == nullptr)
            return;

        _uint iType = pGrapple->Get_Type();
        switch (iType)
        {
        case (_uint)COLLISION_GROUP::GRAPPLENEEDLE:
        case (_uint)COLLISION_GROUP::MONSTER :
        {
            _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
            _vec3 vTargetPos = static_cast<CTransform*>(m_pGrappleTarget->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
            _vector vDir = (vTargetPos - vPos)._vector();

            m_pMovementCom->Activate(true);
            m_pMovementCom->FlyMode(true);
            m_pMovementCom->Reset_Velocity();
            m_pMovementCom->Set_ForceFriction(0.f);
            m_pMovementCom->Add_Force(vDir, m_tPlayerStat.fGrappleForce);
            Call_GrappleHitEffect();
        }
        break;
        default :
            break;
        }
    }
}

void CPlayer::Rolling_Collide()
{
    if (m_tPlayerStat.bRolling)
    {
        CShellSkill_RollOut* pSkill = static_cast<CShellSkill_RollOut*>(m_pPlayerShell->Get_ShellSkill());
        pSkill->Cancel_Skill();

        m_pMovementCom->Set_RotateToDirection(false);
        m_pStateMachine->ChangeState(L"ShellSkill_RollOut_BackFlip");

        CGameInstance::GetInstance()->Call_Lag(0.01f, 0.3f);
        Player_CameraShake_Position(0.05f, 25.f, 0.8f);
        Player_CameraShake_Rotation(0.05f, 25.f, 0.8f);

        _vector vCamPos{}, vCamLook{}, vPos{}, vLook{}, vUp{}, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
        vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
        vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);
        vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION) + vUp;
        vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
        vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);

        if (m_pRollingTarget != nullptr)
        {
            _float fDamage = pSkill->Get_SkillDamage();

            IDamaged* pDamaged = dynamic_cast<IDamaged*>(m_pRollingTarget);
            if (pDamaged == nullptr)
                return;

            pDamaged->Damaged(fDamage, DAMAGE::DAMAGE_HEAVY, this);   
            m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_ROLL_DAMAGE, vPos, vLook, false, nullptr);
        }
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROLL_DAMAGE, vPos, vZeroLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vZeroLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook, vCamLook, false, nullptr);

        CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_LOOP);
        Play_PlayerSound(L"Shell_RollOut_Impact");

        Reset_Rolling();
    }
}

void CPlayer::Attacked(_bool bShellGuard, _float fDamage, DAMAGE eDamage, CGameObject* pDamager)
{
    CTransform* pTransform = static_cast<CTransform*>(pDamager->Find_Component(g_strTransformTag));

    _vec3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    _vec3 vTargetDir = (_vec3(pTransform->Get_State(CTransform::STATE::POSITION)) - _vec3(vMyPos)).Normalized();
    vTargetDir.y = 0.f;

    if (bShellGuard)
    {
        m_pPlayerShell->Damaged(fDamage, eDamage, pDamager);
        if (m_pPlayerShellStat->fShellCurrHP == 0.f)
        {
            Delete_Shell();
            m_pUpperStateMachine->ChangeState(L"None");
            m_pStateMachine->ChangeState(L"NormalAttacked");
        }
        else 
            m_pStateMachine->ChangeState(L"ShellAttacked");

        switch (static_cast<_uint>(eDamage))
        {
            case (_uint)DAMAGE::DAMAGE_HEAVY :
            {
                m_pMovementCom->Add_Force(vTargetDir._vector() * -1.f, 8.f);

                m_pGameInstance->Call_Lag(0.1f, 0.1f);
                Player_CameraShake_Rotation(0.02f, 15.f, 0.4f);

                break;
            }
            default :
                break;
        }
    }
    else
    {
        m_tPlayerStat.fCurrHP -= fDamage;

        if (m_tPlayerStat.bInAir)
            eDamage = DAMAGE::DAMAGE_HEAVY;

        Call_DamagedEffect(eDamage);
        switch (static_cast<_uint>(eDamage))
        {
        case (_uint)DAMAGE::DAMAGE_NORMAL :
        {
            m_pGameInstance->Call_Lag(0.08f, 0.08f);
            Player_CameraShake_Rotation(0.03f, 18.f, 0.4f);

            m_pStateMachine->ChangeState(L"NormalAttacked");
        }
        break;
        case (_uint)DAMAGE::DAMAGE_HEAVY :
        {
            KnockBack(30.f, DAMAGE::DAMAGE_HEAVY, pDamager);

            m_pGameInstance->Call_Lag(0.05f, 0.1f);
            Player_CameraShake_Rotation(0.06f, 20.f, 0.8f);
        }
        break;
        case (_uint)DAMAGE::DAMAGE_SUPER :
        {
            KnockBack(40.f, DAMAGE::DAMAGE_SUPER, pDamager);

            m_pGameInstance->Call_Lag(0.02f, 0.2f);
            Player_CameraShake_Rotation(0.08f, 25.f, 1.f);
        }
        default :
            break;
        }
    }
}

void CPlayer::Check_Debuff(_float fTimeDelta)
{
    if (m_fDebuffLatency != 0.f)
    {
        m_fDebuffLatency -= fTimeDelta;
        if (m_fDebuffLatency <= 0.f)
            m_fDebuffLatency = 0.f;
    }

    if (m_fDebuffLatency <= 0.f)
    {
        if (m_tPlayerStat.fFearGauge != 0.f)
        {
            m_tPlayerStat.fFearGauge -= fTimeDelta;
            if (m_tPlayerStat.fFearGauge <= 0.f)
                m_tPlayerStat.fFearGauge = 0.f;
        }

        if (m_tPlayerStat.bFeared && m_tPlayerStat.fFearGauge <= 0.f)
        {
            m_PlayerState.reset(PLAYERSTATE_DEBUFF_FEAR);
            m_tPlayerStat.bFeared = false;

            if (m_tPlayerStat.bHasShell)
                m_pStateMachine->ChangeState(L"Idle");
            else
                m_pStateMachine->ChangeState(L"Parry");
            m_pUpperStateMachine->ChangeState(L"None");

            Disable_FearEffect();

            m_pGameInstance->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_LOOP);
        }
    }
}

void CPlayer::Respawn(_float fTimeDelta)
{
    if (!m_tPlayerStat.bDeath)
        return;

    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

    _float m_fPrevAcc = m_fRespawnAcc;
    m_fRespawnAcc += fTimeDelta;
    if (m_fRespawnAcc >= m_tPlayerStat.fRespawnTime - 1.f && m_fPrevAcc < m_tPlayerStat.fRespawnTime - 1.f)
    {
        pUIManager->Set_FadeSpeed(1.f);
        pUIManager->FadeOut();
    }

    if (m_fRespawnAcc >= m_tPlayerStat.fRespawnTime)
    {
        _uint iShellLevel = m_tPlayerStat.iShellLevel;
        if (iShellLevel == 0)
            return;

        Delete_Shell();
        m_pPlayerWeapon->Deattach_Shell();

        m_tPlayerStat.bTeleport = true;
        m_fRespawnAcc = 0.f;

        //_vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        //CRemains* pRemain = static_cast<CRemains*>(m_pGameInstance->Find_GameObject((_uint)LEVELID::LEVEL_STATIC, L"Layer_Remains", 0));
        //
        //pRemain->Drop(vPos._float3(), m_pGameInstance->Get_CurrentLevelIndex());
        
        CGameObject* _pEnemyManager = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Manager").front();
        if(_pEnemyManager)
            static_cast<CEnemy_Manager*>(_pEnemyManager)->Respawn_All_Enemies();

        if (iShellLevel == m_pGameInstance->Get_CurrentLevelIndex())
            MoonShell_Interact(false);
        else
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVELID)iShellLevel));
    }
}

void CPlayer::Decrease_RadialBlur(_float fTimeDelta)
{
    _float fRadialIntensity = m_pGameInstance->Get_RadialBlurParams().fIntensity;
    if (fRadialIntensity > 0.f)
    {
        fRadialIntensity -= fTimeDelta;
        if (fRadialIntensity < 0.f)
            fRadialIntensity = 0.f;

        RADIALBLUR_PARAMS tRadialBlur{};
        tRadialBlur.fIntensity = fRadialIntensity;

        m_pGameInstance->Set_RadialBlurParams(tRadialBlur);
    }
}

void CPlayer::Calculate_CombinedMatrix()
{
    if (m_tPlayerStat.bGrabbed)
    {
        _matrix SocketMatrix = XMLoadFloat4x4(m_pGrabSocketMatrix);

        for (size_t i = 0; i < 3; ++i)
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

        XMStoreFloat4x4(&m_CombinedWorldMatrix, SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
    }
    else
        XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Reset_PlayerStat()
{
    m_tPlayerStat.bInputEnable = true;
    m_tPlayerStat.bMoveInputEnable = true;

    m_tPlayerStat.fAnimSpeed = 1.f;

    m_pMovementCom->Set_Accel(m_tPlayerStat.fAccel);
    m_pMovementCom->Set_Decel(m_tPlayerStat.fDecel);
    m_pMovementCom->Set_MaxSpeed(m_tPlayerStat.fMaxSpeed);
    m_pMovementCom->Set_MaxFallingSpeed(m_tPlayerStat.fMaxFallingSpeed);
    m_pMovementCom->Set_RotationScale(m_tPlayerStat.fRotationScale);
    m_pMovementCom->Set_GravityScale(m_tPlayerStat.fGravityScale);

    m_tPlayerStat.bTransition = false;
    m_tPlayerStat.fAnimSpeed = 1.f;

    m_tPlayerStat.bRunning = false;
    m_tPlayerStat.bRunnable = true;
    m_tPlayerStat.fRunMaxSpeedRatio = 1.5f;
    m_tPlayerStat.fRunTimeAcc = 0.f;

    m_tPlayerStat.bInAir = false;
    m_tPlayerStat.bCanAirAction = true;;
    m_pMovementCom->Set_InAirControlRatio(m_tPlayerStat.fInAirControlRatio);

    m_tPlayerStat.bSwimming = false;
    m_tPlayerStat.bSwimmable = true;
    m_tPlayerStat.fSwimmingMaxTime = 2.f;
    m_tPlayerStat.fSwimmingTimeAcc = 0.f;

    m_tPlayerStat.bLockOn = false;
    m_pPlayerSpringArm->UnLock_LockOn();

    m_tPlayerStat.bEvade = false;
    m_tPlayerStat.bBackStep = false;
    m_tPlayerStat.bCanBackStepAttack = false;

    m_tPlayerStat.bGrabbed = false;

    m_tPlayerStat.bSuperArmor = false;

    m_tPlayerStat.bCharging = false;
    m_tPlayerStat.bDashStart = false;
    m_tPlayerStat.iComboCount = 0;

    m_tPlayerStat.bGuard = false;
    m_tPlayerStat.bParry = false;
    m_tPlayerStat.bCanParryAttack = false;

    m_tPlayerStat.bInvincible = false;

    m_tPlayerStat.bInteract = false;

    m_tPlayerStat.bGrappleAim = false;
    m_tPlayerStat.bGrapplable = false;
    m_tPlayerStat.bGrappling = false;

    m_tPlayerStat.bUseItem = false;

    m_tPlayerStat.bMagnetPulling = false;
    m_tPlayerStat.bMagnetPulled = false;

    m_pInputBuffer->Clear();
    m_pInputGate->Clear();
}

void CPlayer::Clear_Buffer()
{
    m_tPlayerStat.bInteract = false;

    m_pInputBuffer->Clear();
    m_pInputGate->Clear();
}

void CPlayer::Player_CameraShake_Position(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
    m_pPlayerSpringArm->Get_PlayerCamera()->PositionShake(fAmplitude, fFrequency, fDuration, bLoop);
}

void CPlayer::Player_CameraShake_Rotation(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
    m_pPlayerSpringArm->Get_PlayerCamera()->RotationShake(fAmplitude, fFrequency, fDuration, bLoop);
}

void CPlayer::Apply_RadialBlur(_float fIntensity)
{
    RADIALBLUR_PARAMS tRadialBlur{};
    tRadialBlur.fIntensity = fIntensity;

    m_pGameInstance->Set_RadialBlurParams(tRadialBlur);
}

void CPlayer::Player_TimeSlow(_bool bSlow, _float fTimeRatio)
{
    if (bSlow)
    {
        m_pGameInstance->Call_Lag(fTimeRatio, 1.f, true);

        m_pPlayerSpringArm->Set_IsLag(false);
        m_pPlayerCineCamera->Set_IsLag(false);
        static_cast<CUI_Player*>(Find_PartObject({ L"Part_UI" }))->Set_IsLag(false);
    }
    else
    {
        m_pGameInstance->Reset_Lag();

        m_pPlayerSpringArm->Set_IsLag(true);
        m_pPlayerCineCamera->Set_IsLag(true);
        static_cast<CUI_Player*>(Find_PartObject({ L"Part_UI" }))->Set_IsLag(true);
    }
}

void CPlayer::Reset_XZRotation()
{
    _vec3 vRotation = m_pTransformCom->Get_Rotation();
    
    m_pTransformCom->Rotate(0.f, vRotation.y, 0.f);
}

void CPlayer::Play_PlayerSound(const _wstring& strSound)
{
    auto iter = m_mapSound.find(strSound);
    if (iter == m_mapSound.end())
        return;

    m_pGameInstance->Stop_Sound(iter->second.iChannel);
    m_pGameInstance->Play_Sound(iter->second.strSound, iter->second.iChannel, iter->second.fVolume);
}

void CPlayer::Play_PlayerSound_Loop(const _wstring& strSound)
{
    auto iter = m_mapSound.find(strSound);
    if (iter == m_mapSound.end())
        return;

    m_pGameInstance->Stop_Sound(iter->second.iChannel);
    m_pGameInstance->Play_Sound_Loop(iter->second.strSound, iter->second.iChannel, iter->second.fVolume);
}

void CPlayer::UI_Setting()
{
    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->Set_PlayerStat(&m_tPlayerStat);
    pUIManager->Set_PlayerShellStat_Ptr(&m_pPlayerShellStat);

    pUIManager->Set_MenuUI(Find_PartObject({ L"Part_UI",  L"Part_UI_Menu" }));
    pUIManager->Set_StatusUI(Find_PartObject({ L"Part_UI",  L"Part_UI_Menu", L"Part_UI_Status"}));
    pUIManager->Set_InventoryUI(Find_PartObject({ L"Part_UI",  L"Part_UI_Menu", L"Part_UI_Inventory"}));
}

HRESULT CPlayer::Add_Sound(const _wstring strSoundName, const _wstring strSoundFile, SOUND_CHANNEL eChannel, _float fVolume)
{
    auto iter = m_mapSound.find(strSoundName);
    if (iter != m_mapSound.end())
        return E_FAIL;

    FPlayerSound tSound{};
    tSound.strSound = strSoundFile;
    tSound.iChannel = (_uint)eChannel;
    tSound.fVolume = fVolume;

    m_mapSound.emplace(strSoundName, tSound);

    return S_OK;
}

void CPlayer::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case(_uint)COLLISION_GROUP::SPONGE :
    {
        m_pStateMachine->ChangeState(L"BackFlip");
    }
    break;
    case (_uint)COLLISION_GROUP::MAGNET:
    {
        CUI_Compass* pUICompass = static_cast<CUI_Compass*>(Find_PartObject({ L"Part_UI", L"Part_UI_Compass" }));
        pUICompass->Set_Broken(true);
    }
    break;
    case (_uint)COLLISION_GROUP::MONSTER :
    {
        CEnemy* pEnemy = static_cast<CEnemy*>(pOther);
        if (pEnemy == nullptr)
            return;

        if (m_tPlayerStat.bGrappling)
        {
            Reset_XZRotation();

            m_pStateMachine->ChangeState(L"Grapple_Attack");
            //pEnemy->Set_Hooked(false);

            m_pPhysicsControllerCom->Set_UserData(false);
            static_cast<CEffect_Textured_Rope*>(m_PartObjects.find(L"Part_Rope")->second)->Clear_Info();

            //CGameInstance::GetInstance()->Call_Lag(0.1f, 0.5f);
        }
        else if (m_tPlayerStat.bRolling)
        {
            if (m_pRollingTarget != nullptr)
            {
                _vec3 vOriginPos = static_cast<CTransform*>(m_pRollingTarget->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
                _vec3 vTargetPos = static_cast<CTransform*>(pOther->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);

                _float fOriginDist = Calculate_Distance_From(vOriginPos._vector());
                _float fTargetDist = Calculate_Distance_From(vTargetPos._vector());

                if (fTargetDist > fOriginDist)
                    m_pRollingTarget = pOther;
            }
            else
                m_pRollingTarget = pOther;
        }
    }
    break;
    default :
        break;
    }
}

void CPlayer::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::TRIGGER:
    {
    }
    case (_uint)COLLISION_GROUP::PORTAL:
    {
        if (m_tPlayerStat.bInteract)
        {
            m_tPlayerStat.bInteract = false;

            CShellHouse* pShellHouse = static_cast<CShellHouse*>(pOther);
            if (pShellHouse == nullptr)
                return;

            if (pShellHouse->Check_Activated())
            {
                m_pMovementCom->Reset_Velocity();

                CShellHouse::DATA tShellHouseData = pShellHouse->Get_Data();
                m_tPlayerStat.iShellLevel = tShellHouseData.iLevelIndex;
                m_tPlayerStat.iShellNum = tShellHouseData.SheleportData.iSheleportNum;
                m_tPlayerStat.vShellPos = tShellHouseData.vPosition;
                m_tPlayerStat.vShellLook = tShellHouseData.vLook;
                m_tPlayerStat.vShellRight = tShellHouseData.vRight;

                m_pTransformCom->Locate(m_tPlayerStat.vShellPos);
                m_pPhysicsControllerCom->Set_Position(_vec3(m_tPlayerStat.vShellPos));

                _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

                _vec3 vDir = vPos + (_vec3(m_tPlayerStat.vShellLook) + _vec3(m_tPlayerStat.vShellRight) * 1.7f).Normalized();
                m_pTransformCom->LookAt(vDir._vector());

                MoonShell_Interact(true);
            }
        }
    }
    break;
    case (_uint)COLLISION_GROUP::MONSTER_HIT:
    {
        if (m_tPlayerStat.bGrappling)
            UnGrapple();
        else if (m_tPlayerStat.bParry)
        {
            IParried* pParried = dynamic_cast<IParried*>(pOther);
            if (pParried == nullptr)
                return;

            CTransform* pTransform = static_cast<CTransform*>(pParried->Get_Parried()->Find_Component(g_strTransformTag));
            _vec3 vParriedPos = pTransform->Get_State(CTransform::STATE::POSITION);
            _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

            _vec3 vDir = (vParriedPos - vPos).Normalized();
            vDir.y = 0.f;

            m_pTransformCom->LookAt((vPos + vDir)._vector());

            Call_ParryEffect();
            pParried->Parried();

            m_tPlayerStat.bCanParryAttack = true;
            m_tPlayerStat.bParry = false;

            Play_PlayerSound(L"Parry_Success");
        }
        else if (m_tPlayerStat.bBackStep)
        {
            IParried* pParried = dynamic_cast<IParried*>(pOther);
            if (pParried == nullptr)
                return;

            m_pBackStepTarget = pParried->Get_Parried();
            pParried->Parried();

            Call_BackStepEffect();
            m_tPlayerStat.bCanBackStepAttack = true;
            m_tPlayerStat.bBackStep = false;

            Play_PlayerSound(L"Parry_Success");
        }
    }
    break;
    case (_uint)COLLISION_GROUP::SHELL:
    {
        if (m_tPlayerStat.bGrappling)
        {
            m_pGrappleTarget = nullptr;
            m_pGameInstance->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_LOOP);
            static_cast<CEffect_Textured_Rope*>(m_PartObjects.find(L"Part_Rope")->second)->Clear_Info();
        }

        if (m_tPlayerStat.bInteract || m_tPlayerStat.bGrappling)
        {
            m_tPlayerStat.bInteract = false;

            CShell_World* pShellWorld = static_cast<CShell_World*>(pOther);
            SHELL eShell = pShellWorld->Get_ShellStat_Const()->eShell;
            _float fHP = pShellWorld->Get_ShellStat_Const()->fShellCurrHP;

            Equip_Shell(eShell, fHP);
            m_pStateMachine->ChangeState(L"ShellEquip");

            m_pGameInstance->Delete_GameObject
            (
                m_pGameInstance->Get_CurrentLevelIndex(),
                L"Layer_Shell",
                pShellWorld
            );

        }
    }
    break;
    case (_uint)COLLISION_GROUP::GRAPPLENEEDLE:
    {
        if (m_tPlayerStat.bGrappling &&
            m_pGrappleTarget == pOther)
            Grapple_Boost();

        static_cast<CEffect_Textured_Rope*>(m_PartObjects.find(L"Part_Rope")->second)->Clear_Info();
    }
    break;
    case (_uint)COLLISION_GROUP::MAGNET:
    {
        if (m_tPlayerStat.bHasShell && m_tPlayerStat.bGuard && m_pPlayerShellStat->bMagnet)
        {
            if (!m_tPlayerStat.bMagnetPulling)
            {
                m_tPlayerStat.bMagnetPulling = true;
                m_pMovementCom->Set_GravityScale(0.f);

                m_pMagnetTransform = static_cast<CTransform*>(pOther->Find_Component(g_strTransformTag));

                Play_PlayerSound(L"Magnet_Pulled");
            }
            else
                Magnet_Pulled();
        }
        else if (m_tPlayerStat.bMagnetPulling || m_tPlayerStat.bMagnetPulled)
        {
            Magnet_UnPulled();
            Play_PlayerSound(L"Magnet_UnPulled");
        }
    }
    break;
    case (_uint)COLLISION_GROUP::FORK :
    {
        if (m_tPlayerStat.bInteract)
        {
            m_tPlayerStat.bInteract = false;
            m_pPlayerWeapon->Toggle_Active(true);
            
            m_pPlayerCineCamera->Play_Cinematic(L"GrabFork");
            m_pStateMachine->ChangeState(L"CutScene_GrabFork");
        }
    }
    break;
    case (_uint)COLLISION_GROUP::HEARTKELP:
    {
        if (m_tPlayerStat.bInteract)
        {
            m_tPlayerStat.bInteract = false;
            m_tPlayerStat.bPreserveItem = true;

            m_pUpperStateMachine->ChangeState(L"Item_HeartKelp");
        }
    }
    break;
    default:
        break;
    }
}

void CPlayer::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::MAGNET:
    {
        CUI_Compass* pUICompass = static_cast<CUI_Compass*>(Find_PartObject({ L"Part_UI", L"Part_UI_Compass" }));
        pUICompass->Set_Broken(false);

        if (m_tPlayerStat.bMagnetPulling || m_tPlayerStat.bMagnetPulled)
            Magnet_UnPulled();
    }
    break;
    case (_uint)COLLISION_GROUP::MONSTER:
    {
        if (m_tPlayerStat.bRolling)
        {
            if (m_pRollingTarget == pOther)
                m_pRollingTarget = nullptr;
        }
    }
    default:
        break;
    }
}

HRESULT CPlayer::Ready_Component()
{
    // Model
    {
        CModel::DESC Desc{};

        Desc.pOwner = this;
        Desc.pRootBone = "Spine";

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Player",
            L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
            return E_FAIL;

        m_pModelCom->Add_AnimSlot("LowerChest", "Antennae_End_R_end");
        m_pModelCom->Add_AnimSlot("clavical_l", "Hand_Grip_L_end");
    }

    // Com_Shader
    {
        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
            L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    // Com_Movement
    {
        CMovement::DESC Desc{};
        Desc.fMoveAccel = m_tPlayerStat.fAccel;
        Desc.fMoveDecel = m_tPlayerStat.fDecel;
        Desc.fMaxSpeed = m_tPlayerStat.fMaxSpeed;
        Desc.fForceFriction = m_tPlayerStat.fForceFriction;
        Desc.fRotationScale = m_tPlayerStat.fRotationScale;

        Desc.fInAirControlRatio = m_tPlayerStat.fInAirControlRatio;
        Desc.fMaxFallingSpeed = m_tPlayerStat.fMaxFallingSpeed;
        Desc.fGravityScale = m_tPlayerStat.fGravityScale;

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
            L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
            return E_FAIL;

        m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
        m_pMovementCom->Set_RotateToDirection(true);
    }

    // Com_Collider
    {
        CCollider_Sphere::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PLAYER;

        Desc.fRadius = 1.f;
        Desc.vCenter = { 0.f, 1.f, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    // Com_PhysicsController
    {
        CPhysics_Controller::DESC Desc{};
        Desc.vPosition = _mat(m_CombinedWorldMatrix).position._vector(); // 플레이어 위치 따라가기
        //Desc.eClimbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
        //Desc.eNonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
        Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
        //Desc.pFinalMatrix = &m_CombinedWorldMatrix;
        Desc.fRadius = 0.5f;
        Desc.fHeight = 1.f;
        Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;
        Desc.fScaleCoeff = 1.35f;

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
            L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Ready_PartObject()
{
    // SpringArm_Player
    {
        CSpringArm_Player::DESC SpringArmDesc{};

        SpringArmDesc.pOwner = this;

        SpringArmDesc.SpringArmDesc.GameObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
        SpringArmDesc.SpringArmDesc.GameObjectDesc.fSpeedPerSec = 1.f;
        SpringArmDesc.SpringArmDesc.GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

        SpringArmDesc.SpringArmDesc.vOffset = _float3(-0.8f, 0.8f, -4.5f);
        SpringArmDesc.SpringArmDesc.vTargetOffset = _float3(0.8f, 0.7f, 0.f);
        SpringArmDesc.SpringArmDesc.fArmLength = 5.f;
        SpringArmDesc.SpringArmDesc.fLagSpeed = 0.1f;
        SpringArmDesc.SpringArmDesc.fLagRotation = 0.9f;
        SpringArmDesc.SpringArmDesc.fCollisionOffset = 0.1f;

        SpringArmDesc.fLerpSpeed = 5.f;
        SpringArmDesc.fMaxPitchDegree = 60.f;

        if (FAILED(Add_PartObject(L"Part_SpringArm", LEVEL_STATIC, L"Prototype_GameObject_SpringArm_Player", &SpringArmDesc)))
            return E_FAIL;

        m_pPlayerSpringArm = static_cast<CSpringArm_Player*>(Find_PartObject({ L"Part_SpringArm" }));
        m_pPlayerSpringArm->Set_PhysicsCollision(true);
    }

    // CineCamera_Player
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"CineCamera_Player";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera_Player", &CineCameraDesc)))
            return E_FAIL;

        m_pPlayerCineCamera = static_cast<CCineCamera_Player*>(Find_PartObject({ L"Part_CineCamera" }));
        m_pPlayerCineCamera->Add_Cinematic(L"Grapple", "../Bin/DataFiles/CameraData/Player_Grapple.data");
        m_pPlayerCineCamera->Add_Cinematic(L"Shelleport", "../Bin/DataFiles/CameraData/Player_Shelleport.data");
        m_pPlayerCineCamera->Add_Cinematic(L"GrabFork", "../Bin/DataFiles/CameraData/Player_GrabFork.data");
        m_pPlayerCineCamera->Add_Cinematic(L"UpgradeFork", "../Bin/DataFiles/CameraData/Player_UpgradeFork.data");
        m_pPlayerCineCamera->Add_Cinematic(L"FinishAttack", "../Bin/DataFiles/CameraData/Player_FinishAttack.data");
        m_pPlayerCineCamera->Add_Cinematic(L"MantisPunch", "../Bin/DataFiles/CameraData/Player_MantisPunch.data");
        m_pPlayerCineCamera->Add_Cinematic(L"MantisPunch2", "../Bin/DataFiles/CameraData/Player_MantisPunch2.data");
        m_pPlayerCineCamera->Add_Cinematic(L"Urchin", "../Bin/DataFiles/CameraData/Player_Urchin.data");
        m_pPlayerCineCamera->Add_Cinematic(L"ParryAttack", "../Bin/DataFiles/CameraData/Player_ParryAttack.data");
        m_pPlayerCineCamera->Add_Cinematic(L"BackStepAttack", "../Bin/DataFiles/CameraData/Player_BackStepAttack.data");
        m_pPlayerCineCamera->Add_Cinematic(L"GrappleAttack", "../Bin/DataFiles/CameraData/Player_GrappleAttack.data");
    }

    CUI_Player::DESC UIDesc{};
    {
        UIDesc.pPlayerStat = &m_tPlayerStat;
        UIDesc.ppShellStat = &m_pPlayerShellStat;
        UIDesc.ppGrappleTarget = &m_pGrappleTarget;
        UIDesc.ppFinishTarget = &m_pFinishTarget;
    }
    if (FAILED(Add_PartObject(L"Part_UI", LEVEL_STATIC, L"Prototype_GameObject_UI_Player", &UIDesc)))
        return E_FAIL;

    // Skill
    {
        vector<FActiveSkill> vecActiveSkill = CSkill_DataBase::Get().Get_ActiveSkills();
        for (auto& Skill : vecActiveSkill)
        {
            //if (!Skill.bUnLocked)
            //    continue;

            CActiveSkill::DESC tSkillDesc{};
            tSkillDesc.tGameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
            tSkillDesc.pOwner = this;
            tSkillDesc.eActiveSkill = Skill.eActiveSkill;

            _wstring strPrototypeTag = L"Prototype_GameObject_" + Skill.strActiveSkill;
            _wstring strPartTag = L"Part_" + Skill.strActiveSkill;

            if (FAILED(Add_PartObject
            (
                strPartTag,
                LEVEL_STATIC,
                strPrototypeTag,
                &tSkillDesc
            )))
                return E_FAIL;

            m_ActiveSkills[static_cast<_uint>(Skill.eActiveSkill)] = static_cast<CActiveSkill*>(Find_PartObject({ strPartTag }));
        }
    }


    // Rope For Rope Action
    {
        CEffect_Textured_Rope::DESC RopeDesc{};
        RopeDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name({ "Hand_Grip_L" });
        RopeDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();

        if (FAILED(Add_PartObject(L"Part_Rope", LEVEL_STATIC, L"Prototype_GameObject_Effect_TexturedRope", &RopeDesc)))
            return E_FAIL;
    }

    return S_OK;
}


HRESULT CPlayer::Ready_StateMachine()
{
    // StateMachine 생성
    CStateMachine_Player::DESC StateMachineDesc{};

    StateMachineDesc.StateMachineDesc.pOwner = this;
    StateMachineDesc.StateMachineDesc.strState = L"Idle";
    StateMachineDesc.pPlayerState = &m_PlayerState;

    if(m_pGameInstance->Get_CurrentLevelIndex() == LEVELID::LEVEL_BEACH)
        StateMachineDesc.StateMachineDesc.strState = L"CutScene_Hide";

    m_pStateMachine = static_cast<CStateMachine_Player*>(m_pGameInstance->Clone_Prototype
    (
        LEVEL_STATIC,
        L"Prototype_StateMachine_Player",
        &StateMachineDesc
    ));

    // UpperStateMachine 생성
    CStateMachine_PlayerUpper::DESC UpperStateMachineDesc{};

    UpperStateMachineDesc.StateMachineDesc.pOwner = this;
    UpperStateMachineDesc.StateMachineDesc.strState = L"None";
    UpperStateMachineDesc.pPlayerState = &m_PlayerState;

    m_pUpperStateMachine = static_cast<CStateMachine_PlayerUpper*>(m_pGameInstance->Clone_Prototype
    (
        LEVEL_STATIC,
        L"Prototype_StateMachine_PlayerUpper",
        &UpperStateMachineDesc
    ));

    return S_OK;
}

HRESULT CPlayer::Ready_Event()
{
    // Jump 이벤트 등록
    if (FAILED(Add_Event(L"Event_JumpLaunchStart", CEvent_JumpLaunchStart::Create(this))))
        return E_FAIL;

    // Transition 이벤트 등록
    if (FAILED(Add_Event(L"Event_Transition", CEvent_Transition::Create(this))))
        return E_FAIL;

    // NormalAttack 이벤트 등록
    if (FAILED(Add_Event(L"Event_NormalAttackInit", CEvent_NormalAttackInit::Create(this))) ||
        FAILED(Add_Event(L"Event_NormalAttackStart", CEvent_NormalAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_NormalAttackEnd", CEvent_NormalAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_HammerAttackStart", CEvent_HammerAttackStart::Create(this))))
        return E_FAIL;

    // ChargeAttack 이벤트 등록
    if (FAILED(Add_Event(L"Event_ChargeAttackStart", CEvent_ChargeAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_ChargeAttackEnd", CEvent_ChargeAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_HammerChargeAttackStart", CEvent_HammerChargeAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_HammerChargeAttackEnd", CEvent_HammerChargeAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_ChargeOn", CEvent_ChargeOn::Create(this))) ||
        FAILED(Add_Event(L"Event_ChargeOff", CEvent_ChargeOff::Create(this))))
        return E_FAIL;

    // DashAttack 이벤트 등록
    if (FAILED(Add_Event(L"Event_DashAttackStart", CEvent_DashAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_DashAttackEnd", CEvent_DashAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_HammerDashAttackStart", CEvent_HammerDashAttackStart::Create(this))))
        return E_FAIL;

    // SwipeAttack 이벤트 등록
    if (FAILED(Add_Event(L"Event_SwipeAttackInit", CEvent_SwipeAttackInit::Create(this))) ||
        FAILED(Add_Event(L"Event_SwipeAttackStart", CEvent_SwipeAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_SwipeAttackEnd", CEvent_SwipeAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_HammerSwipeAttackStart", CEvent_HammerSwipeAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_HammerSwipeAttackEnd", CEvent_HammerSwipeAttackEnd::Create(this))))
        return E_FAIL;

    // PlungeAttack 이벤트 등록
    if (FAILED(Add_Event(L"Event_PlungeAttackStart", CEvent_PlungeAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_PlungeAttackEnd", CEvent_PlungeAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_PlungeAttackImpact", CEvent_PlungeAttackImpact::Create(this))))
        return E_FAIL;

    // ParryAttack 이벤트 등록
    if(FAILED(Add_Event(L"Event_ParryAttackInit", CEvent_ParryAttackInit::Create(this))) ||
        FAILED(Add_Event(L"Event_ParryAttackStart", CEvent_ParryAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_ParryAttackEnd", CEvent_ParryAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_ParryAttackImpact", CEvent_ParryAttackImpact::Create(this))) ||
        FAILED(Add_Event(L"Event_ParryAttackFinish", CEvent_ParryAttackFinish::Create(this))))
        return E_FAIL;

    // BackStepAttack 이벤트 등록
    if (FAILED(Add_Event(L"Event_BackStepAttackInit", CEvent_BackStepAttackInit::Create(this))) ||
        FAILED(Add_Event(L"Event_BackStepAttackStart", CEvent_BackStepAttackStart::Create(this))) ||
        FAILED(Add_Event(L"Event_BackStepAttackEnd", CEvent_BackStepAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_BackStepAttack_Sound", CEvent_BackStepAttack_Sound::Create(this))) ||
        FAILED(Add_Event(L"Event_BackStepAttackFinish", CEvent_BackStepAttackFinish::Create(this))) ||
        FAILED(Add_Event(L"Event_BackStepOn", CEvent_BackStepOn::Create(this))) ||
        FAILED(Add_Event(L"Event_BackStepOff", CEvent_BackStepOff::Create(this))))
        return E_FAIL;

    // Evade 이벤트 등록
    if (FAILED(Add_Event(L"Event_EvadeNakedStart", CEvent_EvadeNakedStart::Create(this))) ||
        FAILED(Add_Event(L"Event_EvadeNakedEnd", CEvent_EvadeNakedEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_EvadeMediumStart", CEvent_EvadeMediumStart::Create(this))) ||
        FAILED(Add_Event(L"Event_EvadeHeavyStart", CEvent_EvadeHeavyStart::Create(this))) ||
        FAILED(Add_Event(L"Event_EvadeOn", CEvent_EvadeOn::Create(this))) ||
        FAILED(Add_Event(L"Event_EvadeOff", CEvent_EvadeOff::Create(this))) ||
        FAILED(Add_Event(L"Event_BackStepStart", CEvent_BackStepStart::Create(this))))
        return E_FAIL;

    // Collider 이벤트 등록
    if (FAILED(Add_Event(L"Event_WeaponColliderOn", CEvent_WeaponColliderOn::Create(this))) ||
        FAILED(Add_Event(L"Event_WeaponColliderOff", CEvent_WeaponColliderOff::Create(this))) ||
        FAILED(Add_Event(L"Event_HandColliderOn", CEvent_HandColliderOn::Create(this))) ||
        FAILED(Add_Event(L"Event_HandColliderOff", CEvent_HandColliderOff::Create(this))))
        return E_FAIL;

    // Parry 이벤트 등록
    if (FAILED(Add_Event(L"Event_ParryOn", CEvent_ParryOn::Create(this))) ||
        FAILED(Add_Event(L"Event_ParryOff", CEvent_ParryOff::Create(this))))
        return E_FAIL;

    // ShellSkill 이벤트 등록
    if (FAILED(Add_Event(L"Event_Skill_Fizzle", CEvent_Skill_Fizzle::Create(this))) ||
        FAILED(Add_Event(L"Event_Skill_RollOut", CEvent_Skill_RollOut::Create(this))) ||
        FAILED(Add_Event(L"Event_Skill_RollOut_BackFlip", CEvent_Skill_RollOut_BackFlip::Create(this))))
        return E_FAIL;

    // ActionSkill 이벤트 등록
    if (FAILED(Add_Event(L"Event_Hammer_Change", CEvent_Hammer_Change::Create(this))) ||
        FAILED(Add_Event(L"Event_GrappleAttack_Launch", CEvent_GrappleAttack_Launch::Create(this))) ||
        FAILED(Add_Event(L"Event_GrappleAttack_Impact", CEvent_GrappleAttack_Impact::Create(this))) ||
        FAILED(Add_Event(L"Event_GrappleAttack_BackFlip", CEvent_GrappleAttack_BackFlip::Create(this))) ||
        FAILED(Add_Event(L"Event_GrappleAttack_BackFlip2", CEvent_GrappleAttack_BackFlip2::Create(this))) ||
        FAILED(Add_Event(L"Event_GrappleAttackFinish", CEvent_GrappleAttackFinish::Create(this))) ||
        FAILED(Add_Event(L"Event_FinishAttackEnd", CEvent_FinishAttackEnd::Create(this))) ||
        FAILED(Add_Event(L"Event_FinishAttackExit", CEvent_FinishAttackExit::Create(this))) ||
        FAILED(Add_Event(L"Event_FinishAttackFinish", CEvent_FinishAttackFinish::Create(this))))
        return E_FAIL;

    // ActiveSkill 이벤트 등록
    if (FAILED(Add_Event(L"Event_ActiveSkill", CEvent_ActiveSkill::Create(this))) ||
        FAILED(Add_Event(L"Event_MantisPunch_AutoLock", CEvent_MantisPunch_AutoLock::Create(this))) ||
        FAILED(Add_Event(L"Event_MantisPunch_Teleport", CEvent_MantisPunch_Teleport::Create(this))) ||
        FAILED(Add_Event(L"Event_MantisPunch_Impact", CEvent_MantisPunch_Impact::Create(this))) ||
        FAILED(Add_Event(L"Event_MantisPunch_PostImpact", CEvent_MantisPunch_PostImpact::Create(this))) ||
        FAILED(Add_Event(L"Event_MantisPunch_TimeSlow", CEvent_MantisPunch_TimeSlow::Create(this))) ||
        FAILED(Add_Event(L"Event_MantisPunch_TimeSlow2", CEvent_MantisPunch_TimeSlow2::Create(this))) ||
        FAILED(Add_Event(L"Event_MantisPunchFinish", CEvent_MantisPunchFinish::Create(this))) ||
        FAILED(Add_Event(L"Event_Urchin_Play", CEvent_Urchin_Play::Create(this))) ||
        FAILED(Add_Event(L"Event_Urchin_Stop", CEvent_Urchin_Stop::Create(this))) ||
        FAILED(Add_Event(L"Event_Urchin_Dodge", CEvent_Urchin_Dodge::Create(this))) ||
        FAILED(Add_Event(L"Event_Urchin_Launch", CEvent_Urchin_Launch::Create(this))))
        return E_FAIL;

    // Item 이벤트 등록
    if (FAILED(Add_Event(L"Event_Item_HeartKelp", CEvent_Item_HeartKelp::Create(this))))
        return E_FAIL;

    // Attacked 이벤트 등록
    if (FAILED(Add_Event(L"Event_GetUp_Front", CEvent_GetUp_Front::Create(this))) ||
        FAILED(Add_Event(L"Event_GetUp_Back", CEvent_GetUp_Back::Create(this))))
        return E_FAIL;

    // Attacked 이벤트 등록
    if (FAILED(Add_Event(L"Event_HeavyDamage", CEvent_HeavyDamage::Create(this))) ||
        FAILED(Add_Event(L"Event_NormalDamage", CEvent_NormalDamage::Create(this))))
        return E_FAIL;

    // 이펙트 이벤트에 필요한 매트릭스
    const _float4x4* pParentMatrix = &m_CombinedWorldMatrix;
    const _float4x4* pLeftHandSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name({ "Hand_Grip_L" });
    const _float4x4* pRightHandSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name({ "Hand_Grip_R" });
    const _float4x4* pLeftFootSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name({ "foot_L" });
    const _float4x4* pRightFootSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name({ "foot_R" });
    const _float4x4* pWeaponCombinedMatrix = &m_PartObjects.find(TEXT("Part_Weapon"))->second->Get_CombinedMatrix();

    // 모래 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Event_Effect_SandBurst_L", CEvent_SandBurst::Create(this, pParentMatrix, pLeftFootSocketMatrix))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_SandBurst_R", CEvent_SandBurst::Create(this, pParentMatrix, pRightFootSocketMatrix))))
        return E_FAIL;

    // 버블 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Event_LArm_Bubble", CEvent_SwimBubble::Create(this, pParentMatrix, pLeftHandSocketMatrix))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_RArm_Bubble", CEvent_SwimBubble::Create(this, pParentMatrix, pRightHandSocketMatrix))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_AttNormal", CEvent_AttackOne::Create(this, pParentMatrix, pLeftHandSocketMatrix))))
        return E_FAIL;

    // 차징 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Event_Effect_Att", CEvent_Effect_ChargeAtt::Create(this, pParentMatrix, pWeaponCombinedMatrix))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_Seq00", CEvent_Effect_ChargingSeq00::Create(this, pWeaponCombinedMatrix))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_Seq01", CEvent_Effect_ChargingSeq01::Create(this, pParentMatrix, pLeftHandSocketMatrix))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_Seq02", CEvent_Effect_ChargingSeq02::Create(this, pParentMatrix, pLeftHandSocketMatrix))))
        return E_FAIL;

    // 대쉬 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Event_Effect_BeforeDash", CEvent_Effect_BeforeDash::Create(this, pParentMatrix, pWeaponCombinedMatrix))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_AfterDash", CEvent_Effect_AfterDash::Create(this, pParentMatrix, pWeaponCombinedMatrix))))
        return E_FAIL;

    // 해머 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Event_Effect_HammerSwap", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::SWAP))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Event_Effect_HammerCharge", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::CHARGE))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_HammerJump", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::JUMP))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_HammerThrust", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::THRUST))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_HammerThrustWallop", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::THRUST_WALLOP))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_HammerWallop", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::CHARGE_WALLOP))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Event_Effect_HammerOnce", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::ONCE))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_HammerTwice", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::TWICE))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Event_Effect_HammerDash", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::DASH))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_HammerAir", CEvent_Effect_Hammer::Create(this, pWeaponCombinedMatrix, CEvent_Effect_Hammer::AIR))))
        return E_FAIL;

    // 맨티스 펀치 이펙트 이벤트 등록(오른손)
    if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Player_Charging", 
        CEvent_Effect_MantisPunch_Player::Create(this, pRightHandSocketMatrix, 
            CEvent_Effect_MantisPunch_Player::CHARGE))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Player_Jump",
        CEvent_Effect_MantisPunch_Player::Create(this, pRightHandSocketMatrix,
            CEvent_Effect_MantisPunch_Player::JUMP))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Player_TeleportStart",
        CEvent_Effect_MantisPunch_Player::Create(this, pRightHandSocketMatrix,
            CEvent_Effect_MantisPunch_Player::TELE_START))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Player_TeleportEnd",
        CEvent_Effect_MantisPunch_Player::Create(this, pRightHandSocketMatrix,
            CEvent_Effect_MantisPunch_Player::TELE_END))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Player_Impact",
        CEvent_Effect_MantisPunch_Player::Create(this, pRightHandSocketMatrix,
            CEvent_Effect_MantisPunch_Player::IMPACT))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Player_JumpOut",
        CEvent_Effect_MantisPunch_Player::Create(this, pRightHandSocketMatrix,
            CEvent_Effect_MantisPunch_Player::JUMP_OUT))))
        return E_FAIL;

    // 내려찍기(Plunge) 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Event_Effect_Plunge_Ready", CEvent_Effect_Plunge::Create(this, pWeaponCombinedMatrix, false))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Event_Effect_Plunge_Impact", CEvent_Effect_Plunge::Create(this, pWeaponCombinedMatrix, true))))
        return E_FAIL;

    // 넘어짐 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Effect_FallDown", CEvent_Effect_FallDown::Create(this))))
        return E_FAIL;

    // 포크 관련(획득, 업그레이드) 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Effect_Upgrade_Fork", CEvent_Effect_Fork::Create(this, pWeaponCombinedMatrix))))
        return E_FAIL;

    // 피니시 공격 이펙트 이벤트 등록
    if (FAILED(Add_Event(L"Effect_Finish", CEvent_Effect_Finish::Create(this, false))))
        return E_FAIL;
    if (FAILED(Add_Event(L"Effect_Finish_Blood", CEvent_Effect_Finish::Create(this, true))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Sound()
{
    // FootStep
    if (FAILED(Add_Sound(L"FootStep", L"Player_FootStep_Sand.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"FootStepLand", L"Player_FootStep_Sand_Land.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)))
        return E_FAIL;

    // Impact
    if (FAILED(Add_Sound(L"Impact", L"Player_Impact.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)))
        return E_FAIL;

    // Dodge
    if (FAILED(Add_Sound(L"DodgeNaked", L"Player_DodgeNaked.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"DodgeNaked_Bubble", L"Player_DodgeNaked_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"DodgeMedium", L"Player_DodgeMedium.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"DodgeMedium_Bubble", L"Player_DodgeMedium_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"DodgeHeavy", L"Player_DodgeHeavy.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"DodgeHeavy_Bubble", L"Player_DodgeHeavy_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"Dodge_Voice", L"Player_Dodge_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;
    
    // BackStep
    if (FAILED(Add_Sound(L"BackStep", L"Player_BackStep.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"BackStep_Bubble", L"Player_BackStep_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"BackStep_Success", L"Player_BackStep_Success.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"BackStepAttack", L"Player_BackStepAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"BackStepAttack_Voice", L"Player_BackStepAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)) ||
        FAILED(Add_Sound(L"BackStepAttack_Voice2", L"Player_BackStepAttack_Voice2.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Jump
    if (FAILED(Add_Sound(L"Jump_Bubble", L"Player_Jump_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"Jump_Voice", L"Player_Jump_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Swim
    if (FAILED(Add_Sound(L"Swim_Bubble", L"Player_Swim_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)))
        return E_FAIL;

    // NormalAttack
    if (FAILED(Add_Sound(L"NormalAttack1", L"Player_NormalAttack1.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"NormalAttack2", L"Player_NormalAttack2.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"NormalAttack3", L"Player_NormalAttack3.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"NormalAttack4", L"Player_NormalAttack4.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"NormalAttack_Bubble", L"Player_NormalAttack_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"NormalAttack_Voice", L"Player_NormalAttak_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // ChargeAttack
    if (FAILED(Add_Sound(L"ChargeAttack", L"Player_ChargeAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"ChargeAttack_Bubble", L"Player_ChargeAttack_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"ChargeAttack_Effect", L"Player_ChargeAttack_Effect.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"ChargeAttack_Voice", L"Player_ChargeAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // DashAttack
    if (FAILED(Add_Sound(L"DashAttack", L"Player_DashAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"DashAttack_Voice", L"Player_DashAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // SwipeAttack
    if (FAILED(Add_Sound(L"SwipeAttack", L"Player_SwipeAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"SwipeAttack_Bubble", L"Player_SwipeAttack_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"SwipeAttack_Voice", L"Player_SwipeAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // PlungeAttack
    if (FAILED(Add_Sound(L"PlungeAttack", L"Player_PlungeAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"PlungeAttack_Bubble", L"Player_PlungeAttack_Bubble.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"PlungeAttack_Bubble2", L"Player_PlungeAttack_Bubble2.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)) ||
        FAILED(Add_Sound(L"PlungeAttack_Effect", L"Player_PlungeAttack_Effect.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"PlungeAttack_Impact", L"Player_PlungeAttack_Impact.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"PlungeAttack_Voice", L"Player_PlungeAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Hammer_Pick
    if (FAILED(Add_Sound(L"Hammer_Get", L"Player_Hammer_Get.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)))
        return E_FAIL;

    // Hammer_NormalAttack
    if (FAILED(Add_Sound(L"Hammer_NormalAttack1", L"Player_Hammer_NormalAttack1.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Hammer_NormalAttack2", L"Player_Hammer_NormalAttack2.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Hammer_NormalAttack_Voice", L"Player_Hammer_NormalAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Hammer_ChargeAttack
    if (FAILED(Add_Sound(L"Hammer_ChargeAttack_Impact", L"Player_Hammer_ChargeAttack_Impact.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Hammer_ChargeAttack_Voice", L"Player_Hammer_ChargeAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)) ||
        FAILED(Add_Sound(L"Hammer_ChargeAttack_Voice2", L"Player_Hammer_ChargeAttack_Voice2.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Hammer_DashAttack
    if (FAILED(Add_Sound(L"Hammer_DashAttack", L"Player_Hammer_DashAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Hammer_DashAttack_Voice", L"Player_Hammer_DashAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Hammer_SwipeAttack
    if (FAILED(Add_Sound(L"Hammer_SwipeAttack", L"Player_Hammer_SwipeAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Hammer_SwipeAttack_Voice", L"Player_Hammer_SwipeAttack_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Parry
    if (FAILED(Add_Sound(L"Parry", L"Player_Parry.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Parry_Success", L"Player_Parry_Success.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Parry_Attack", L"Player_Parry_Attack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)))
        return E_FAIL;

    // Shell
    if (FAILED(Add_Sound(L"Shell_Equip", L"Player_Shell_Equip.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_UnEquip", L"Player_Shell_UnEquip.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Enter", L"Player_Shell_Enter.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)))
        return E_FAIL;

    // Shell_Attacked
    if (FAILED(Add_Sound(L"Shell_Attacked_Home", L"Player_Shell_Attacked_Home.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Attacked_SodaCan", L"Player_Shell_Attacked_SodaCan.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Attacked_MirrorBall", L"Player_Shell_Attacked_MirrorBall.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Attacked_Duck", L"Player_Shell_Attacked_Duck.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Attacked_TennisBall", L"Player_Shell_Attacked_TennisBall.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)))
        return E_FAIL;

    // Shell_Break
    if (FAILED(Add_Sound(L"Shell_Break_Base", L"Player_Shell_Break_Base.wav", SOUND_CHANNEL::PLAYER_SUBEFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Break_SodaCan", L"Player_Shell_Break_SodaCan.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Break_MirrorBall", L"Player_Shell_Break_MirrorBall.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Break_Duck", L"Player_Shell_Break_Duck.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Break_TennisBall", L"Player_Shell_Break_TennisBall.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)))
        return E_FAIL;

    // Shell_Skill
    if (FAILED(Add_Sound(L"Shell_RollOut_Charge", L"Player_Shell_RollOut_Charge.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_RollOut_Loop", L"Player_Shell_RollOut_Loop.wav", SOUND_CHANNEL::PLAYER_LOOP, 1.f)) ||
        FAILED(Add_Sound(L"Shell_RollOut_Impact", L"Player_Shell_RollOut_Impact.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Fizzle", L"Player_Shell_Fizzle.wav", SOUND_CHANNEL::PROJECTILE_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Shell_Fizzle_Bubble", L"Player_Shell_Fizzle_Bubble.wav", SOUND_CHANNEL::PROJECTILE_EFFECT, 1.f)))
        return E_FAIL;

    // Grapple
    if (FAILED(Add_Sound(L"Grapple_Throw", L"Player_Grapple_Throw.wav", SOUND_CHANNEL::PROJECTILE_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Grapple_Hit", L"Player_Grapple_Hit.wav", SOUND_CHANNEL::PROJECTILE_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Grapple_Loop", L"Player_Grapple_Loop.wav", SOUND_CHANNEL::PLAYER_AMBIENCE, 1.f)))
        return E_FAIL;

    // Grapple_Skill
    if (FAILED(Add_Sound(L"Grapple_AttackStart", L"Player_Grapple_AttackStart.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Grapple_Attack", L"Player_Grapple_Attack.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Grapple_Shell", L"Player_Grapple_Shell.wav", SOUND_CHANNEL::PLAYER_LOOP, 1.f)))
        return E_FAIL;

    // Grabbed
    if (FAILED(Add_Sound(L"Grabbed1", L"Player_Grabbed1.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"Grabbed2", L"Player_Grabbed2.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"Grabbed3", L"Player_Grabbed3.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)))
        return E_FAIL;

    // NormalAttacked
    if (FAILED(Add_Sound(L"NormalAttacked1", L"Player_NormalAttacked1.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"NormalAttacked2", L"Player_NormalAttacked2.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)))
        return E_FAIL;

    // HeavyAttacked
    if (FAILED(Add_Sound(L"HeavyAttacked1", L"Player_HeavyAttacked1.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"HeavyAttacked2", L"Player_HeavyAttacked2.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"HeavyAttacked3", L"Player_HeavyAttacked3.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"HeavyAttacked4", L"Player_HeavyAttacked4.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)))
        return E_FAIL;

    // SuperAttacked
    if (FAILED(Add_Sound(L"SuperAttacked1", L"Player_SuperAttacked1.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"SuperAttacked2", L"Player_SuperAttacked2.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"SuperAttacked3", L"Player_SuperAttacked3.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"SuperAttacked4", L"Player_SuperAttacked4.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"SuperAttacked5", L"Player_SuperAttacked5.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"SuperAttacked6", L"Player_SuperAttacked6.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"SuperAttacked_Fly", L"Player_SuperAttacked_Fly.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)))
        return E_FAIL;

    // NormalDeath
    if (FAILED(Add_Sound(L"NormalDeath1", L"Player_NormalDeath1.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"NormalDeath2", L"Player_NormalDeath2.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"NormalDeath3", L"Player_NormalDeath3.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)))
        return E_FAIL;

    // HeavyDeath
    if (FAILED(Add_Sound(L"HeavyDeath1", L"Player_HeavyDeath1.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)) ||
        FAILED(Add_Sound(L"HeavyDeath2", L"Player_HeavyDeath2.wav", SOUND_CHANNEL::PLAYER_VOICE, 1.f)))
        return E_FAIL;

    // HeartKelp
    if (FAILED(Add_Sound(L"HeartKelp", L"Player_HeartKelp.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)))
        return E_FAIL;

    // Feared
    if (FAILED(Add_Sound(L"Feared_Loop", L"Player_Feared_Loop.wav", SOUND_CHANNEL::PLAYER_LOOP, 1.f)) ||
        FAILED(Add_Sound(L"Feared_Voice", L"Player_Feared_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // FinishAttack
    if (FAILED(Add_Sound(L"FinishAttack", L"Player_FinishAttack.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"FinishAttack_Siren", L"Player_FinishAttack_Siren.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)))
        return E_FAIL;

    // MantisPunch
    if (FAILED(Add_Sound(L"MantisPunch_Charge", L"Player_MantisPunch_Charge.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"MantisPunch_Teleport", L"Player_MantisPunch_Teleport.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"MantisPunch_Reappear", L"Player_MantisPunch_Reappear.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"MantisPunch_Impact", L"Player_MantisPunch_Impact.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"MantisPunch_Impact2", L"Player_MantisPunch_Impact2.wav", SOUND_CHANNEL::PLAYER_SUBEFFECT, 1.f)) ||
        FAILED(Add_Sound(L"MantisPunch_Voice", L"Player_MantisPunch_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)) ||
        FAILED(Add_Sound(L"MantisPunch_Voice2", L"Player_MantisPunch_Voice2.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)))
        return E_FAIL;

    // Urchin
    if (FAILED(Add_Sound(L"Urchin_Base", L"Player_Urchin_Base.wav", SOUND_CHANNEL::PLAYER_LOOP, 1.f)) ||
        FAILED(Add_Sound(L"Urchin_Charge", L"Player_Urchin_Charge.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Urchin_Charge_Voice", L"Player_Urchin_Charge_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)) ||
        FAILED(Add_Sound(L"Urchin_Charge_Explosion_Small", L"Player_Urchin_Explosion_Small.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Urchin_Charge_Explosion_Medium", L"Player_Urchin_Explosion_Medium.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Urchin_Charge_Explosion", L"Player_Urchin_Explosion.wav", SOUND_CHANNEL::PLAYER_EFFECT, 1.f)) ||
        FAILED(Add_Sound(L"Urchin_Throw", L"Player_Urchin_Throw.wav", SOUND_CHANNEL::PLAYER_ACTION, 1.f)) ||
        FAILED(Add_Sound(L"Urchin_Throw_Voice", L"Player_Urchin_Throw_Voice.wav", SOUND_CHANNEL::PLAYER_VOICE, 0.7f)) ||
        FAILED(Add_Sound(L"Urchin_Whirl", L"Player_Urchin_Whirl.wav", SOUND_CHANNEL::PLAYER_SUBEFFECT, 1.f)))
        return E_FAIL;

    // Magnet
    if (FAILED(Add_Sound(L"Magnet_Pulled", L"Player_Magnet_Pulled.wav", SOUND_CHANNEL::PLAYER_EFFECT, 0.7f)) ||
        FAILED(Add_Sound(L"Magnet_UnPulled", L"Player_Magnet_UnPulled.wav", SOUND_CHANNEL::PLAYER_EFFECT, 0.7f)))
        return E_FAIL;

    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    if (PLAYERINST != nullptr)
        CPlayerManager::Save_PlayerData(*this);

    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pMovementCom);
    Safe_Release(m_pPhysicsControllerCom);

    Safe_Release(m_pStateMachine);
    Safe_Release(m_pUpperStateMachine);

    Safe_Release(m_pInputGate);
    Safe_Release(m_pInputBuffer);
}