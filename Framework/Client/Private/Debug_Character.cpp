#include "pch.h"
#include "Debug_Character.h"

#include "GameInstance.h"
#include "Animation.h"
#include "Movement.h"

#include "Player.h"

CDebug_Character::CDebug_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CDebug_Character::CDebug_Character(const CDebug_Character& Prototype)
	: super(Prototype)
{
}

HRESULT CDebug_Character::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebug_Character::Initialize(void* pArg)
{
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());

	return S_OK;
}

void CDebug_Character::Priority_Update(_float fTimeDelta)
{
}

void CDebug_Character::Update(_float fTimeDelta)
{
	_bool isImguiHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered();

	if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::DIM_LB) && !isImguiHovered)
	{
		_uint iOut = 0;

		if (m_pGameInstance->Compute_ObjectID(iOut) == true)
			m_iPickedID = iOut;
	}
}

void CDebug_Character::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
}

HRESULT CDebug_Character::Render()
{
	Debug_Player();
	Debug_Character();

	return S_OK;
}

void CDebug_Character::Debug_Player()
{
	ImGui::Begin("Player Debugger");

	Debug_PlayerStat();

	ImGui::End();
}

void CDebug_Character::Debug_Character()
{
	CGameObject* pPickedGameObject = m_pGameInstance->Find_GameObject_With_ID(m_pGameInstance->Get_CurrentLevelIndex(), m_iPickedID);

	if (m_iPickedID == 0 ||
		pPickedGameObject == nullptr ||
		pPickedGameObject == m_pPlayer)
		return;

	ImGui::Begin("Character Debugger");
	ImGui::Text("ID: %d", pPickedGameObject->Get_ID());

	Debug_MovementComponent(pPickedGameObject);

	ImGui::End();
}

void CDebug_Character::Debug_PlayerStat()
{
	ImGui::NewLine();

	CMovement* pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));
	if (pMovement == nullptr) 
		return;

	_float3 vHorizontal = pMovement->Get_HorizontalVelocity();
	_float3 vVertical = pMovement->Get_VerticalVelocity();

	ImGui::Text("Horizontal Speed : %.2f", pMovement->Get_HorizontalSpeed());
	ImGui::Text("Vertical Speed : %.2f", pMovement->Get_VerticalSpeed());
	ImGui::Text("Horizontal : %.2f, %.2f, %.2f", vHorizontal.x, vHorizontal.y, vHorizontal.z);
	ImGui::Text("Vertical : %.2f, %.2f, %.2f", vVertical.x, vVertical.y, vVertical.z);
	ImGui::NewLine();

	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
	_vec3 vRot = pTransform->Get_Rotation();
	_vec3 vLook = pTransform->Get_State(CTransform::STATE::LOOK);

	ImGui::Text("Pos : %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);
	ImGui::Text("Rot : %.2f, %.2f, %.2f", vRot.x, vRot.y, vRot.z);
	ImGui::Text("Look : %.2f, %.2f, %.2f", vLook.x, vLook.y, vLook.z);
	ImGui::NewLine();

	_bool bHasTarget{};
	bHasTarget = m_pPlayer->Get_GrappleTarget();
	ImGui::Text("GrappleTarget : %d", bHasTarget);

	//static _bool bCheat{};
	//if (ImGui::Checkbox("CheatMode", &bCheat))
	//	m_pPlayer->CheatMode(bCheat);

	_float fLagTime = m_pGameInstance->Get_LagTime();
	ImGui::Text("LagTime : %.2f", fLagTime);

	ImGui::NewLine();

	if (!m_pPlayer->Get_PlayerStat()->bCheat)
	{
		_float fAccel = pMovement->Get_Accel();
		if (ImGui::DragFloat("Accel", &fAccel, 0.f))
			pMovement->Set_Accel(fAccel);

		_float fDecel = pMovement->Get_Decel();
		if (ImGui::DragFloat("Decel", &fDecel, 0.f))
			pMovement->Set_Decel(fDecel);

		_float fMaxSpeedRatio = m_pPlayer->Get_PlayerStat()->fRunMaxSpeedRatio;
		if (ImGui::DragFloat("MaxSpeedRatio", &fMaxSpeedRatio, 0.f))
			pMovement->Set_MaxSpeed(m_pPlayer->Get_PlayerStat()->fMaxSpeed * fMaxSpeedRatio);

		_float fMaxFallingSpeed = pMovement->Get_MaxFallingSpeed();
		if (ImGui::DragFloat("MaxFallingSpeed", &fMaxFallingSpeed, 0.f))
			pMovement->Set_MaxFallingSpeed(fMaxFallingSpeed);

		 _float fRotationScale = pMovement->Get_RotationScale();
		if (ImGui::SliderFloat("RotationScale", &fRotationScale, 0.f, 1.f))
			pMovement->Set_RotationScale(fRotationScale);

		_float fGravityScale = pMovement->Get_GravityScale();
		if (ImGui::DragFloat("GravityScale", &fGravityScale, 0.f))
			pMovement->Set_GravityScale(fGravityScale);

		ImGui::NewLine();

		PlayerStateFlags PlayerState = m_pPlayer->Get_PlayerState();
		if (PlayerState.test(PLAYERSTATE_IDLE))
			ImGui::Text("CurrState : Idle");
		else if (PlayerState.test(PLAYERSTATE_WALK))
			ImGui::Text("CurrState :Walk");
		else if (PlayerState.test(PLAYERSTATE_RUN))
			ImGui::Text("CurrState :Run");
		else if (PlayerState.test(PLAYERSTATE_AIRBONE))
			ImGui::Text("CurrState :Airbone");
		else if (PlayerState.test(PLAYERSTATE_SWIMMING))
			ImGui::Text("CurrState :Swimming");
		else if (PlayerState.test(PLAYERSTATE_LOCKON))
			ImGui::Text("CurrState :LockOn");
		else if (PlayerState.test(PLAYERSTATE_JUMPLAUNCH))
			ImGui::Text("CurrState :JumpLaunch");
		else if (PlayerState.test(PLAYERSTATE_JUMPLAND))
			ImGui::Text("CurrState :JumpLand");
		else if (PlayerState.test(PLAYERSTATE_EVADE))
			ImGui::Text("CurrState :Evade");
		else if (PlayerState.test(PLAYERSTATE_NORMALATTACKED))
			ImGui::Text("CurrState :Attacked");
		else if (PlayerState.test(PLAYERSTATE_NORMALATTACK))
			ImGui::Text("CurrState :NormalAttack");
		else if (PlayerState.test(PLAYERSTATE_CHARGEATTACK))
			ImGui::Text("CurrState :ChargeAttack");
		else if (PlayerState.test(PLAYERSTATE_DASHATTACK))
			ImGui::Text("CurrState :DashAttack");
		else if (PlayerState.test(PLAYERSTATE_SWIPEATTACK))
			ImGui::Text("CurrState :SwipeAttack");
		else if (PlayerState.test(PLAYERSTATE_PARRY))
			ImGui::Text("CurrState :Parry");

		if (PlayerState.test(PLAYERSTATE_GUARD))
			ImGui::Text("Guard");

		_bool bEvade = m_pPlayer->Get_PlayerStat()->bEvade;
		_bool bParry = m_pPlayer->Get_PlayerStat()->bParry;
		if (bEvade) ImGui::Text("SecondState : Evading");
		else if (bParry) ImGui::Text("SecondState : Parrying");
	}
	else
	{
		_float fCheatSpeedRatio = m_pPlayer->Get_PlayerStat()->fCheatSpeedRatio;
		if (ImGui::DragFloat("CheatSpeedRatio", &fCheatSpeedRatio, 0.f))
			m_pPlayer->Get_PlayerStat()->fCheatSpeedRatio = fCheatSpeedRatio;

		_float fCheatMaxSpeedRatio = m_pPlayer->Get_PlayerStat()->fCheatMaxSpeedRatio;
		if (ImGui::DragFloat("CheatMaxSpeedRatio", &fCheatMaxSpeedRatio, 0.f))
			m_pPlayer->Get_PlayerStat()->fCheatMaxSpeedRatio = fCheatMaxSpeedRatio;
	}
}

void CDebug_Character::Debug_MovementComponent(CGameObject* pGameObject)
{
	ImGui::NewLine();

	CMovement* pMovement = static_cast<CMovement*>(pGameObject->Find_Component(L"Com_Movement"));
	if (pMovement == nullptr) return;

	_float3 vHorizontal = pMovement->Get_HorizontalVelocity();
	_float3 vVertical = pMovement->Get_VerticalVelocity();

	ImGui::Text("Horizontal Speed : %.2f", pMovement->Get_HorizontalSpeed());
	ImGui::Text("Vertical Speed : %.2f", pMovement->Get_VerticalSpeed());
	ImGui::Text("Horizontal : %.2f, %.2f, %.2f", vHorizontal.x, vHorizontal.y, vHorizontal.z);
	ImGui::Text("Vertical : %.2f, %.2f, %.2f", vVertical.x, vVertical.y, vVertical.z);
	ImGui::NewLine();
}

CDebug_Character* CDebug_Character::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Character* pInstance = new CDebug_Character(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CDebug_Character");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CDebug_Character::Clone(void* pArg)
{
	CDebug_Character* pInstance = new CDebug_Character(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CDebug_Character");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebug_Character::Free()
{
	super::Free();
}