#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel;
class CChannel;
class CBone;

class CAnimation final : public CBase
{
	using super = CBase;

public:
	enum class ROOTMOTION
	{
		ROOT_NONE,
		ROOT_DELETE,
		ROOT_TRANSFORM,
		ROOT_END,
	};

private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const CModel* pModel);
	HRESULT Initialize_From_File(const HANDLE& hFile, const CModel* pModel, _wstring strAnimName);
	_bool Update_TransformationMatrix(_float fTimeDelta, const vector<CBone*> Bones, _bool isLoop, _float fAnimSpeed, CModel* pModel);
	_bool Update_TransformationMatrix_While_Transition(_float fTimeDelta, _float fLerpTime, const vector<CBone*> Bones, const vector<KEYFRAME>& SourKeyFrames, CModel* pModel);

public:
	_double Get_Duration() const { return m_Duration; }
	_double Get_TickPerSecond() const { return m_TicksPerSecond; }
	_double Get_TrackPosition() const { return m_CurrentTrackPosition; }

	const _char* Get_Name() { return m_szName; }

	_bool Get_Active() const { return m_bActive; }
	_bool Get_Playing() const { return m_bPlaying; }
	_bool Get_Blendable() const { return m_bBlendable; }

	_float Get_BaseLerpTime() const { return m_fBaseLerpTime; }
	_float Get_BaseAnimSpeed() const { return m_fBaseAnimSpeed; }
	_float Get_BlendInTime() const { return m_fBlendInTime; }
	_float Get_BlendOutTime() const { return m_fBlendOutTime; }

	ROOTMOTION Get_RootAnimation() const { return m_eRootMotion; }
	queue<pair<_double, string>> Get_AnimNotify() { return m_queAnimNotify; }

public:
	void Set_Active(_bool bActive) { m_bActive = bActive; }
	void Set_Playing(_bool bPlaying) { m_bPlaying = bPlaying; }

	void Set_BaseLerpTime(_float fBaseLerpTime) { m_fBaseLerpTime = fBaseLerpTime; }
	void Set_BaseAnimSpeed(_float fBaseAnimSpeed) { m_fBaseAnimSpeed = fBaseAnimSpeed; }
	void Set_BlendInTime(_float fBlendInTime) { m_fBlendInTime = fBlendInTime; }
	void Set_BlendOutTime(_float fBlendOutTime) { m_fBlendOutTime = fBlendOutTime; }

	void Set_RootAnimation(ROOTMOTION eRootMotion) { m_eRootMotion = eRootMotion; }

public:
	void Update_TransformationMatrix_Specific(_double fTrackPosition, const vector<CBone*> Bones, CModel* pModel);

public:
	HRESULT Save(const _char* szFilePath);
	HRESULT Save();
	HRESULT Save_AnimData(const _char* szFilePath);
	HRESULT Save_AnimData();
	HRESULT Load_AnimData(const HANDLE& hFile);

	vector<KEYFRAME> Get_CurrentChannelKeyFrames(const vector<CBone*>& Bones) const;
	void Reset();

	_float Compute_Progress() const { return static_cast<_float>(m_CurrentTrackPosition / m_Duration); }

public:
	void Add_AnimNotify(_double fTrackPosition, string& strNotifyName);
	void Change_AnimNotify(string& strNotifyName, string& strChangeName, _double fTrackPosition, _double fChangePosition);
	void Delete_AnimNotify(string& strNotifyName, _double fTrackPosition);

	void Arrange_AnimNotify();

private:
	KEYFRAME::_scaleUnit		m_Duration{};
	KEYFRAME::_scaleUnit		m_TicksPerSecond{};
	KEYFRAME::_scaleUnit		m_CurrentTrackPosition{};
	KEYFRAME::_scaleUnit		m_PrevTrackPosition{};

	std::vector<CChannel*>	m_Channels;
	std::vector<_uint>		m_CurrentKeyFrameIndices;

private:
	_char m_szName[MAX_PATH]{};
	_char m_szAnimPath[MAX_PATH]{};

	_bool m_bActive{ true };
	_bool m_bPlaying{ false };
	_bool m_bBlendable{ false };

	_float m_fBaseLerpTime{ 0.2f };
	_float m_fBaseAnimSpeed{ 1.f };
	_float m_fBlendInTime{};
	_float m_fBlendOutTime{};

	_float m_fLerpTime{};

	ROOTMOTION m_eRootMotion{ ROOTMOTION::ROOT_NONE };

private:
	queue<pair<_double, string>> m_queAnimNotify;

private:
	void Call_AnimNotify(CModel* pModel);

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const CModel* pModel);
	static CAnimation* Create_From_File(const HANDLE& hFile, const CModel* pModel, _wstring strAnimName);
	CAnimation* Clone() const;
	virtual void Free() override;
};

END
