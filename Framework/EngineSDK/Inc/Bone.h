#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel;
class CAnimation;

class CBone final : public CBase
{
	using super = CBase;

private:
	CBone();
	CBone(const CBone& Prototype);
	virtual ~CBone() = default;

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex, const vector<CBone*>& Bones);
	HRESULT Initialize_From_File(const HANDLE& hFile, const vector<CBone*>& Bones);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix, CModel* pModel, CAnimation* pAnimation); // �̰� ���� �̻����

	const _char* const Get_Name() const { return m_szName; }

	_matrix Get_CombindedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	/*const */_float4x4& Get_CombindedTransformationMatrix_Ref() /*const*/ { return m_CombinedTransformationMatrix; }

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }
	_matrix Get_TransformationMatrix() const { return XMLoadFloat4x4(&m_TransformationMatrix); }

	_int Get_ParentBoneIndex() { return m_iParentBoneIndex; }

	HRESULT Save(HANDLE& hFile, const _wstring& strFilePath);

	class CTransform* Get_SeperateTransformCom() { return m_pTransformCom; }

public:
	void Reset_Transform(const vector<CBone*>& Bones, CAnimation* pAnimation);

private:
	_char		m_szName[MAX_PATH]{};
	// std::string	m_strName;

	_float4x4		m_TransformationMatrix{};
	_float4x4		m_PreCombinedTransformationMatrix{};
	_float4x4		m_CombinedTransformationMatrix{};

	_int		m_iParentBoneIndex{ -1 };

	// for sepereate transform from animation;
	//_float3	m_fSeperateScale { 1.f, 1.f, 1.f };
	//_float3	m_fSeperateRotation { 0.f, 0.f, 0.f };
	//_float3	m_fSeperateTranslation { 0.f, 0.f, 0.f };
	class CTransform* m_pTransformCom{ nullptr }; // transform component �� ����Ϸ��� pGameInstance�� ��� �־���ϴµ�.. Ȥ�� 
	// device, context � ��� �־���ϴµ� �켱 ������.. nullptr �Ѱܵ� �Ǵ���� ��
	// ������������ transform matrix �� �����ϴµ� transform ���� ���� �༮�� ����. �׷��� �׳� ����.

private:
	void RootAnimation(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix, CModel* pModel, CAnimation* pAnimation);

public:
	static CBone* Create(const aiNode* pAINode, _int iParentBoneIndex, const vector<CBone*>& Bones);
	static CBone* Create_From_File(const HANDLE& hFile, const vector<CBone*>& Bones);
	CBone* Clone() const;
	virtual void Free() override;
};

END
