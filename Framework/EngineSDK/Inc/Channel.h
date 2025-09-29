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

	/* �� ���� Ư�� �ִϸ��̼��� �����ϱ����� Ư�� �ð������� ���¸� ������. */
	/* Keyframe : Ư�� �ð��� ǥ���ؾ��� ���� ����. */
	std::vector<KEYFRAME> m_KeyFrames; // �̳��� �ٸ� �޸𸮿� ����� �� �ּҸ� ��� �־��ٸ� channel ���� ���� �����ؼ� 

	//_uint m_iCurrentKeyFrameIndex { 0 }; // animation Ŭ�������� �ڽ��� ���� channel �� ���� key frame indext �� �����Ѵ�.

	/* �� ä��(��)�� �̸��� ���� �̸��� ���� ��(��ü��)���� ���� ã�Ƽ� �� ���� �ε����� �����Ѵ�. */
	_uint m_iBoneIndex{ 0 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const CModel* pModel);
	static CChannel* Create_From_File(const HANDLE& hFile, const CModel* pModel);
	virtual void Free() override;
};

END
