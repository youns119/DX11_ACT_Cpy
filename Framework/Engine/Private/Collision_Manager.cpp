#include "Collision_Manager.h"

#include "GameObject.h"
#include "Collider.h"

void CCollision_Manager::Update()
{
	for (_uint i = 0; i < LAYER_END; ++i)
	{
		for (_uint j = 0; j < LAYER_END; ++j)
		{
			if (m_CollisionGroupMatrix[i] & 1 << j)
			{
				Check_Collision_Between_Group(i, j);
			}
		}
	}

	// delete dead obejct with collider
	for (auto& Pairs : m_ColliderGroup)
	{
		vector<pair<CGameObject*, CCollider*>> LiveGroup;
		LiveGroup.reserve(Pairs.size());
		for (_uint i=0; i<static_cast<_uint>(Pairs.size()); ++i)
		{
			auto& Pair = Pairs[i];
			if (true == Pair.first->Get_IsDead())
			{
				Safe_Release(Pair.first);
				Safe_Release(Pair.second);
			}
			else
			{
				LiveGroup.push_back(Pair);
			}
		}

		Pairs.swap(LiveGroup);
	}
}

void CCollision_Manager::Set_CollisionEnableGroup(_uint iSrcLayer, _uint iDstLayer, _bool isEnable)
{
	if (iSrcLayer > iDstLayer)
		swap(iSrcLayer, iDstLayer);

	m_CollisionGroupMatrix[iSrcLayer] |= 1 << iDstLayer;
}

void CCollision_Manager::Add_ColliderGroup(CGameObject* pGameObject, CCollider* pCollider, _uint iGruop)
{
	if (iGruop >= LAYER_END)
	{
		MSG_BOX("Layer Index Out Of Range");
		return;
	}

	Safe_AddRef(pGameObject);
	Safe_AddRef(pCollider);

	m_ColliderGroup[iGruop].push_back({ pGameObject, pCollider });
}

void CCollision_Manager::Clear()
{
	m_CollisionMap.clear();

	for (auto& Group : m_ColliderGroup)
	{
		for (auto& Pair : Group)
		{
			Safe_Release(Pair.first);
			Safe_Release(Pair.second);
		}

		Group.clear();
	}

	m_CollisionGroupMatrix.fill(0);
}

_bool CCollision_Manager::Ray_Cast_To_Collider_In_ColliderGroup(_fvector vRayOrigin, _fvector vRayDirection, _float fRayLength, _Out_ CGameObject*& pHit, _Out_ _float& fDist, _uint iColliderGroup)
{
	const vector<pair<CGameObject*, CCollider*>>& ColliderPair = m_ColliderGroup[iColliderGroup];
	vector<pair<_float, CGameObject*>> Hits;

	for (auto& Pair : ColliderPair)
	{
		auto TempHits = Pair.first->Ray_Cast_To_Collider(vRayOrigin, vRayDirection, fRayLength);
		Hits.insert(Hits.end(), TempHits.begin(), TempHits.end());
	}

	if (Hits.empty())
	{
		pHit = nullptr;
		fDist = 0.f;
		return false;
	}

	pair<_float, CGameObject*> HitObject = *std::min_element(Hits.begin(), Hits.end(),
		[](const pair<_float, CGameObject*>& Src, const pair<_float, CGameObject*> Dst)->_bool
		{
			return Src.first < Dst.first;
		});

	pHit = HitObject.second;
	fDist = HitObject.first;
	return true;

}

void CCollision_Manager::Check_Collision_Between_Group(_uint iSrcGroup, _uint iDstGroup)
{
	const auto& SrcGroup = m_ColliderGroup[iSrcGroup];
	const auto& DstGroup = m_ColliderGroup[iDstGroup];

	struct CollisionInfo
	{
		pair<CGameObject*, CCollider*> Pair;
		_uint iGroup;
	};
	vector<CollisionInfo> DeadCollisions;

	//for (auto& SrcPair : SrcGroup)
	for (size_t i =0; i<SrcGroup.size(); ++i)
	{
		auto& SrcPair = SrcGroup[i];
		//for (auto& DstPair : DstGroup)
		for (size_t j=0; j<DstGroup.size(); ++j)
		{
			auto& DstPair = DstGroup[j];

			_bool isDeadObject = SrcPair.first->Get_IsDead() || DstPair.first->Get_IsDead();
			_bool isActiveObject = SrcPair.first->Get_IsActive() && DstPair.first->Get_IsActive();

			if (true == isDeadObject)
				Process_DeadCollision(SrcPair, DstPair, iSrcGroup, iDstGroup);
			/*else if (false == isActiveObject)
				Process_DeactiveCollision(SrcPair, DstPair, iSrcGroup, iDstGroup);*/
			else
				Process_Collision(SrcPair, DstPair, iSrcGroup, iDstGroup);
		}
	}
}

_bool CCollision_Manager::Is_Collision(CCollider* pSrcCollider, CCollider* pDstCollider)
{
	if (!pSrcCollider->Is_Active() || !pDstCollider->Is_Active())
		return false;

	return pSrcCollider->Intersect(pDstCollider);
}

void CCollision_Manager::Process_Collision(pair<CGameObject*, CCollider*> SrcPair, pair<CGameObject*, CCollider*> DstPair, _uint iSrcGroup, _uint iDstGroup)
{
	COLLISION_ID  CollisionID {};
	CollisionID.iLeft = SrcPair.second->Get_ID();
	CollisionID.iRight = DstPair.second->Get_ID();

	auto iter = m_CollisionMap.find(CollisionID.iCollisionID);
	if (m_CollisionMap.end() == iter)
	{
		m_CollisionMap.emplace(CollisionID.iCollisionID, false);
		iter = m_CollisionMap.find(CollisionID.iCollisionID);
	}

	if (Is_Collision(SrcPair.second, DstPair.second))
	{
		//최초 충돌
		if (false == iter->second)
		{
			SrcPair.first->On_CollisionEnter(DstPair.first, iDstGroup);
			DstPair.first->On_CollisionEnter(SrcPair.first, iSrcGroup);

			iter->second = true;
		}
		else // 이미 충돌 중
		{
			SrcPair.first->On_Collision(DstPair.first, iDstGroup);
			DstPair.first->On_Collision(SrcPair.first, iSrcGroup);
		}
	}
	else
	{
		//충돌을 하지 않은 상태

		// 이전 프레임에 충돌이 있었음
		if (true == iter->second)
		{
			SrcPair.first->On_CollisionExit(DstPair.first, iDstGroup);
			DstPair.first->On_CollisionExit(SrcPair.first, iSrcGroup);

			iter->second = false;
		}
	}
}

void CCollision_Manager::Process_DeactiveCollision(pair<CGameObject*, CCollider*> SrcPair, pair<CGameObject*, CCollider*> DstPair, _uint iSrcGroup, _uint iDstGroup)
{
	COLLISION_ID  CollisionID {};
	CollisionID.iLeft = SrcPair.second->Get_ID();
	CollisionID.iRight = DstPair.second->Get_ID();

	auto iter = m_CollisionMap.find(CollisionID.iCollisionID);
	if (m_CollisionMap.end() == iter)
	{
		m_CollisionMap.emplace(CollisionID.iCollisionID, false);
		iter = m_CollisionMap.find(CollisionID.iCollisionID);
	}

	// 이전 프레임에 충돌이 있었음
	if (true == iter->second)
	{
		SrcPair.first->On_CollisionExit(DstPair.first, iDstGroup);
		DstPair.first->On_CollisionExit(SrcPair.first, iSrcGroup);

		iter->second = false;
	}
}

void CCollision_Manager::Process_DeadCollision(pair<CGameObject*, CCollider*> SrcPair, pair<CGameObject*, CCollider*> DstPair, _uint iSrcGroup, _uint iDstGroup)
{
	COLLISION_ID  CollisionID {};
	CollisionID.iLeft = SrcPair.second->Get_ID();
	CollisionID.iRight = DstPair.second->Get_ID();

	auto iter = m_CollisionMap.find(CollisionID.iCollisionID);
	if (m_CollisionMap.end() != iter)
	{
		// 지금 충돌했냐 안했냐 중요한가? 지금 이번 프레임에 삭제한다고 요청된 게임 오브젝트임, 어짜피 2프레임 지연 안한다면 충돌하나 안하나 확인할 필요 없을 듯
		// 이전 프레임에 충돌하고 있었으면 exit 호출
		if (true == iter->second)
		{
			SrcPair.first->On_CollisionExit(DstPair.first, iDstGroup);
			DstPair.first->On_CollisionExit(SrcPair.first, iSrcGroup);
			iter->second = false;
		}

		m_CollisionMap.erase(iter);
	}
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	//if (FAILED(pInstance))
	//{
	//	MSG_BOX("Failed To Create : CCollision_Manager");
	//	Safe_Release(pInstance);
	//}

	return pInstance;
}

void CCollision_Manager::Free()
{
	super::Free();


	Clear();
}
