#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CTransform;
class CCollider;
class CGameObject;
END

BEGIN(Client)
class CIK_Segment;
class CArrow;
class CIK_Manager : public CCloneable
{
public:
	struct IK_DESC {
		CGameObject*	pOwner{ nullptr };
		_int			iSegmentGroupSize{};
		CTransform*		pRootBoneTransform{ nullptr };
		_float			fLerpSpeed{ 1.f };
		_float			fMaxHeight{ 1.f };
		_float			fConstraintMaxDistance{};
		_float			fConstraintMinDistance{};
		_float			fConstraintAngle{};		
	};

public:
	struct IK_SegmentGroup {
		_bool					bIsMoving{ false };
		_float					fLerpFactor{ 0.f };
		vector<CIK_Segment*>	Segments{};
	};

private:
	CIK_Manager();
	CIK_Manager(const CIK_Manager& Prototype);
	virtual ~CIK_Manager() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pDesc);

public:
	void Update(_float fTimeDelta);

public:
	void Set_Operation(_bool _bOperate) { m_bOperate = _bOperate; }
	void Reset_Position();

public:
	void Adjust_Segment(_float fTimeDelta);
	void Reposition_SegmentGroup(vector<CIK_Segment*> _pSegmentGroup, _vector vTargetPosition);
	_vector Compute_TargetPosition(vector<CIK_Segment*> _pSegmentGroup);
	_vector Compute_InterporatedTargetPosition(_fvector _StartPos, _fvector _TargetPos, _float _fFactor);
	_bool Detect_Ground(_fvector vRayOrigin, _float& _fDistance);
	_bool Compare_Angle(vector<CIK_Segment*> _pSegmentGroup);
	_bool Compare_Distance(CIK_Segment* _pRootSegment);

public:
	void Add_SegmentGroup(_uint _iSegementGroup, CIK_Segment* _pSegment) { m_SegmentGroups[_iSegementGroup].Segments.push_back(_pSegment); }

private:
	_bool							m_bOperate{ true };

private:
	CGameObject*					m_pOwner{ nullptr };

private:
	_float4x4						m_CombinedWorldMatrix{};
	CTransform*						m_pRootBoneTransform{ nullptr };

private:
	vector<IK_SegmentGroup>			m_SegmentGroups{};

private:
	_bool							bSegmenPlaced{ false };
	_float							m_fLerpSpeed{ 1.f };
	_float							m_fMaxHeight{ 1.f };	
	_float							m_fConstraintMaxDistance{};
	_float							m_fConstraintMinDistance{};
	_float							m_fConstraintAngle{};	

public:
	CCloneable* Clone(void* pArg) override;
	static CIK_Manager* Create(void* pArg);
	void Free();		
};

END