#include "pch.h"
#include "Inkerton_Event_MoveSpline.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"


CInkerton_Event_MoveSpline::CInkerton_Event_MoveSpline(_bool _bIsMove, _float _fDuraton)
	:m_bIsMove(_bIsMove)
	,m_fDuraton(_fDuraton)
{
}


HRESULT CInkerton_Event_MoveSpline::Execute()
{	
	if(!m_bIsMove)
		static_cast<CBoss_Inkerton*>(m_pGameObject)->Set_IsSplineMove(nullptr, false, 1.f);

	m_Routes.clear();

	CTransform* m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(m_pGameObject)->Find_Component(TEXT("Com_Transform")));
	CModel* pModel = static_cast<CModel*>(static_cast<CBoss_Inkerton*>(m_pGameObject)->Find_Component(TEXT("Com_Model")));
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	CTransform* m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	_vector vSour = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vec2 StartPos = _vec2(XMVectorGetX(vSour), XMVectorGetZ(vSour));
	vSour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vec2 TargetPos = _vec2(XMVectorGetX(vSour), XMVectorGetZ(vSour));
	m_fDisatance = 0;	
	_float fCurveAmount = Compute_Distance(StartPos, TargetPos) * 0.06f;

	_int i = -1;
	for (_float t = 0.1f; t < 1.f; t += 0.1f){		
		_vec2 temp = static_cast<CBoss_Inkerton*>(m_pGameObject)->Compute_CardinalSpline(StartPos, TargetPos, t, fCurveAmount);
		if (m_Routes.size() == 0) 
			m_fDisatance += Compute_Distance(StartPos, temp);
		else 
			m_fDisatance += Compute_Distance(m_Routes[i], temp);
		i++;
		m_Routes.push_back(temp);
	}

	_float fCurFrame = (_float)pModel->Get_CurrAnimation()->Get_TrackPosition();
	_float fCurAnimSpeed = pModel->Get_CurrAnimation()->Get_BaseAnimSpeed();
	_float fTickPerSec = (_float)pModel->Get_CurrAnimation()->Get_TickPerSecond();
	_float fReminingTime = (((m_fDuraton - fCurFrame) / (fTickPerSec * fCurAnimSpeed)));
	_float fSpeed = m_fDisatance / fReminingTime;
	
	static_cast<CBoss_Inkerton*>(m_pGameObject)->Set_IsSplineMove(&m_Routes, true, fSpeed);

	return S_OK;
}

_float CInkerton_Event_MoveSpline::Compute_Distance(_vec2 vA, _vec2 vB)
{
	_float fX = vA.x - vB.x;
	_float fZ = vA.y - vB.y;	
	return sqrt(fX * fX + fZ * fZ);
}

CInkerton_Event_MoveSpline* CInkerton_Event_MoveSpline::Create(CGameObject* _pGameObject, _bool _bIsMove, _float _fDuraton)
{
	CInkerton_Event_MoveSpline* pInstance = new CInkerton_Event_MoveSpline(_bIsMove, _fDuraton);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_MoveSpline");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_MoveSpline::Free()
{
	__super::Free();
}
