#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CIK_Segment : public CCloneable
{
public:
	struct IK_Segment_DESC {
		CGameObject*	pOwner{ nullptr };				
		_float4x4*		pCombinedMatrix{};		
		_float			fPitchOffset{ 0.f };
		_float			fDefaultDistance{ 0.f };
		_float			fBaseAngle{ 0.f };	
	};

private:
	CIK_Segment();
	virtual ~CIK_Segment() = default;

public:
	HRESULT Initialize(void* pDesc);

public:	
	void Update(_float fTimeDelta);	
	void Reposition();
	
public:		
	void Set_CurPosition(_fvector m_pPosition) { XMStoreFloat4(&m_vCurPoistion, m_pPosition); }
	void Set_LerpStartPosition(_fvector m_pPosition) { XMStoreFloat4(&m_vLerpStartPoistion, m_pPosition); }	
	void Set_TargetPosition(_fvector m_pPosition) { XMStoreFloat4(&m_vTargetPoistion, m_pPosition); }	
	void Set_ToePosition(_fvector m_pPosition) { XMStoreFloat4(&m_vToePoistion, m_pPosition); }

public:	
	_float4x4* Get_WorldSpace();
	_float4x4* Get_CombineMatrix() { return m_pCombinedWorldMatrix; }	
	_float4* Get_LerpStartPosition() { return &m_vLerpStartPoistion; }	
	_float4* Get_CurPosition() { return &m_vCurPoistion; }
	_float4* Get_TargetPosition() { return &m_vTargetPoistion; }
	_float4* Get_ToePosition() { return &m_vToePoistion; }	
	_float Get_BaseAngle() { return m_fBaseAngle; }
	const _float Get_DefaultDistance() { return m_fDefaultDistance; }


private:
	CGameObject*	m_pOwner{ nullptr };	

private:	
	_float4			m_vCurPoistion{};
	_float4			m_vToePoistion{};
	_float4			m_vTargetPoistion{};
	_float4			m_vLerpStartPoistion{};			
	
private:		
	_float			m_fBaseAngle{ 0.f };
	_float			m_fPitchOffset{ 0.f };
	_float			m_fDefaultDistance{ 0.f };

private:
	_float4x4*		m_pCombinedWorldMatrix{};
	_float4x4*		m_pParentCombinedMatrix{};
	_float4x4		m_pWorldMatrix{};

public:
	CCloneable* Clone(void* pArg) override;
	static CIK_Segment* Create(void* pDesc);
	void Free();	
};

END
