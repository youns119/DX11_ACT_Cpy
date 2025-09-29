#include "pch.h"
#include "BobbitWorm_State_Death.h"
#include "GameInstance.h"
#include "Enemy_BobbitWorm.h"
#include "Player.h"
#include "BobbitWorm_Mouth.h"

CBobbitWorm_State_Death::CBobbitWorm_State_Death()
{
}

HRESULT CBobbitWorm_State_Death::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Death";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_BobbitWorm*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CBobbitWorm_State_Death::Enter(void* pObj)
{
	CEnemy_BobbitWorm* pBobbitWorm = static_cast<CEnemy_BobbitWorm*>(pObj);
	CBobbitWorm_Mouth* pMouth = static_cast<CBobbitWorm_Mouth*>(pBobbitWorm->Find_PartObject({ TEXT("Part_Mouth") }));
	static_cast<CCollider*>(pMouth->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pBobbitWorm->Find_PartObject({ TEXT("Part_Detector") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pBobbitWorm->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	
	CPlayer* pPlayer = static_cast<CPlayer*>(pMouth->Get_GrabObject());
	//if (pPlayer)
	//	pPlayer->UnGrab(static_cast<CTransform*>(pBobbitWorm->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK));

	pBobbitWorm->Drop_Loot();
	pBobbitWorm->Death();

	CGameInstance::GetInstance()->Play_Sound(L"Bobbit_DieScream.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.3f);
	m_pModel->SetUp_Animation((_uint)BOBBITWORM_ANIM_INDEX::DIE, false, 0.3f);

	__super::Enter(pObj);
}

void CBobbitWorm_State_Death::Update(_float fTimeDelta, void* pObj)
{
	__super::Update(fTimeDelta, pObj);
}

void CBobbitWorm_State_Death::Exit(void* pObj)
{	
	__super::Exit(pObj);
}

CBobbitWorm_State_Death* CBobbitWorm_State_Death::Create(void* pArg)
{
	CBobbitWorm_State_Death* pInstance = new CBobbitWorm_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBobbitWorm_State_Death::Free()
{
	__super::Free();
}
