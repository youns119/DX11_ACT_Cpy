#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel;
class CBone;

class CChannel final : public CBase
{
	using super = CBase;
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const CModel* pModel);
	HRESULT Initialize_From_File(const HANDLE& hFile, const CModel* pModel);
	void Update_TransformationMatrix(KEYFRAME::_scaleUnit fCurrentTrackPosition, _uint& iCurrentKeyIndex, const vector<CBone*>& Bones, CModel* pModel);
	void Update_TransformationMatrix_While_Transition(_float fRatio, _float fBlendInTime, _uint& iCurrentKeyIndex, const vector<CBone*>& Bones, const KEYFRAME& SourceKeyFrames, CModel* pModel);

	HRESULT Save(HANDLE hFile);

	KEYFRAME Get_CurrentKeyFrame(const vector<CBone*>& Bones) const;

private:
	_char m_szName[MAX_PATH]{};

	/* 이 뼈가 특정 애니메이션을 구동하기위해 특정 시간마다의 상태를 가진다. */
	/* Keyframe : 특정 시간에 표현해야할 뼈의 상태. */
	std::vector<KEYFRAME> m_KeyFrames; // 이놈을 다른 메모리에 만들고 그 주소를 들고 있었다면 channel 까지 깊은 복사해서 

	//_uint m_iCurrentKeyFrameIndex { 0 }; // animation 클래스에서 자신이 가진 channel 에 대한 key frame indext 를 관리한다.

	/* 이 채널(뼈)의 이름과 같은 이름을 가진 모델(전체뼈)중의 뼈를 찾아서 그 뼈의 인덱스를 저장한다. */
	_uint m_iBoneIndex{ 0 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const CModel* pModel);
	static CChannel* Create_From_File(const HANDLE& hFile, const CModel* pModel);
	virtual void Free() override;
};

END
