#include "pch.h"
#include "Enemy_Manager.h"
#include "GameInstance.h"

#include "Enemy.h"

CEnemy_Manager::CEnemy_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEnemy_Manager::CEnemy_Manager(const CEnemy_Manager& Prototype)
	: super(Prototype)
{
}

HRESULT CEnemy_Manager::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;	

	return S_OK;
}

HRESULT CEnemy_Manager::Initialize(void* pArg)
{
	CEnemy_Manager::DESC* pDesc = static_cast<DESC*>(pArg);
	
	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	for (auto&  pObj : m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Monster"))
	{
		ENEMY_OBJECT temp;
		temp.EnemyObject = pObj;
		temp.EnemyTransform = static_cast<CTransform*>(pObj->Find_Component(L"Com_Transform"));
		m_EnemyObjects.push_back(temp);
	}
	
	return S_OK;
}

void CEnemy_Manager::Priority_Update(_float fTimeDelta)
{		
}

void CEnemy_Manager::Update(_float fTimeDelta)
{			
	if (m_pGameInstance->Key_Up(KEY::F5)) {
		Respawn_All_Enemies();
	}
}

void CEnemy_Manager::Late_Update(_float fTimeDelta)
{
	for (auto&  obj : m_EnemyObjects)
	{
		CEnemy* pEnemy = static_cast<CEnemy*>(obj.EnemyObject);
		_float fFrustumRange = pEnemy->Get_FrustumRange();
		 
		_vector vPlayerPos = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());
		_vector vDis = vPlayerPos - obj.EnemyTransform->Get_State(CTransform::STATE::POSITION);
		_float fDis = XMVectorGetX(XMVector3Length(vDis));		

		if (pEnemy->Get_IsDead())
			continue;

		if ((true == m_pGameInstance->Is_InFrustum_WorldSpace(obj.EnemyTransform->Get_State(CTransform::STATE::POSITION), 5.f) 
			&& fDis < 250.f)|| (pEnemy->IsAggroed())) {
			if (true == pEnemy->Get_IsActive())
				continue;

			pEnemy->Active();
		} 
		else {
			if (true != pEnemy->Get_IsActive())
				continue;

			pEnemy->Deactive();
		}
	}	
}

HRESULT CEnemy_Manager::Render()
{
	return S_OK;
}

void CEnemy_Manager::Respawn_All_Enemies()
{
	for (auto& obj : m_EnemyObjects)
	{
		CEnemy* pEnemy = static_cast<CEnemy*>(obj.EnemyObject);
		static_cast<CEnemy*>(obj.EnemyObject)->Respawn();
	}
}

CEnemy_Manager* CEnemy_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Manager* pInstance = new CEnemy_Manager(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEnemy_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCloneable* CEnemy_Manager::Clone(void* pArg)
{
	CEnemy_Manager* pInstance = new CEnemy_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEnemy_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Manager::Free()
{
	super::Free();
}
