#pragma once

#include "Component.h"

BEGIN(Engine)

class CGameObject;

struct MESH_DATA
{
	_string strName;
	_uint iMaterialIndex;
	_uint iBoneINdex;
	_uint iNumVertices;
	_uint iNumIndices;
	_bool isRender;
};

class CShader;

class ENGINE_DLL CModel final : public CComponent
{
	using super = CComponent;

public:
	enum class TYPE
	{
		PRE_TRANSFORM,
		NON_ANIM,
		ANIM,

		MAX
	};

	enum class STATE
	{
		ANIMATION,
		TRANSITION,

		MAX
	};

	struct DESC
	{
		CGameObject* pOwner{};

		vector<_uint> InvisibleMeshIndices;
		vector<_uint> ShaderPasses;

		const _char* pRootBone = "";
	};

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _string& strFilePath, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

	void SetUp_Animation(_uint iAnimIndex, _bool isLoop, _float fLerpTime);
	void SetUp_Animation(_uint iAnimIndex, _bool isLoop);
	_bool Play_Animation(_float fTimeDelta, _float fAnimSpeed = 1.f);
	HRESULT Bind_Material(CShader* pShader, _uint iMeshIndex, MATERIAL_TEXTURE eType, const _char* pConstantName, _uint iTextureIndex = 0);
	HRESULT Bind_BoneMatrices(CShader* pShader, _uint iMeshIndex, const _char* pConstantName);
	HRESULT Bind_BaseColor(CShader* pShader, _uint iMeshIndex, const _char* pConstantName) const;
	HRESULT Bind_EmissionColor(CShader* pShader, _uint iMeshIndex, const _char* pConstantName) const;

	size_t Get_NumMeshes() const { return m_Meshes.size(); }
	_uint Get_BoneIndex_By_Name(const _char* pBoneName) const;
	/*const */_float4x4& Get_BoneCombinedMatrix_By_Name(const _char* pBoneName) const;
	TYPE Get_Type() const { return m_eType; }

	/* Mesh Get Set */
	_bool Is_Render(_uint iMeshIndex);
	_string Get_MeshName(_uint iMeshIndex) const;
	void Is_Render(_uint iMeshIndex, _bool isRender); 
	_uint Get_MaterialIndex_In_Mesh(_uint iMeshIndex);
	const vector<_uint>& Get_BoneIndices_In_Mesh(_uint iMeshIndex);
	const vector<VTXANIMMESH>& Get_VertexData(_uint iMeshIndex);
	const vector<_uint>& Get_IndexData(_uint iMeshIndex);


	/* Material Get Set */
	const _float4& Get_BaseColor(_uint iMtrlIdx) const;
	void Set_BaseColor(_uint iMtrlIdx, const _float4& vColor);

	size_t Get_NumMaterials() const { return m_Materials.size(); }
	_uint Get_ShaderPass(_uint iMaterialIndex);
	void Set_ShaderPass(_uint iMaterialIndex, _uint iShaderPass);
	const vector<pair<_uint, _tchar[MAX_PATH]>>& Get_TextureFiles_By_Type(_uint iMaterialIndex);
	const vector<ID3D11ShaderResourceView*>& Get_Mtrltextures(_uint iMaterialIndex, _uint iType);
	_bool Has_Texture(_uint iMaterialIndex, MATERIAL_TEXTURE eTexture) const;

	/* Bone Get Set */
	size_t Get_NumBones() const { return m_Bones.size(); }
	_string Get_BoneName(_uint iBoneIndex) const;
	const _char* Get_RootBoneName() const { return m_szRootBone; }
	class CTransform* Get_BoneSperateTransformCom(_uint iIndex);
	
	_bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist);
	_bool Is_Progress(_float fRate) const;

	_bool Has_Texture_In_MeshMaterial(_uint iMeshIndex, MATERIAL_TEXTURE eTexture) const;

	// Animation Function
public :
	_bool Get_IsPlaying(_uint iAnimIndex);
	_uint Get_AnimationIndex(const _char* pAnimationName);
	_uint Get_NumAnimation() { return static_cast<_uint>(m_Animations.size()); }
	vector<class CAnimation*>* Get_Animations() { return &m_Animations; }
	CAnimation* Get_CurrAnimation() { return m_Animations[m_iCurrentAnimIndex]; }
	CGameObject* Get_Owner() { return m_pOwner; }
	vector<class CBone*>* Get_Bones() { return &m_Bones; }

	void Set_AnimationFrame_Specific(_double fTrackPosition);

	void Reset_Animation();
	HRESULT Save_Animation(_uint iAnimIndex);

	void Add_AnimNotify(_uint iAnimIndex, _double fTrackPosition, string& strNotifyName);
	void Change_AnimNotify(_uint iAnimIndex, string& strNotifyName, string& strChangeName, _double fTrackPosition, _double fChangeTrack);
	void Delete_AnimNotify(_uint iAnimIndex, string& strNotifyName, _double fTrackPosition);

	void Reset_RootBone(_uint iAnimIndex);

	// 상하체 전용
	void Add_AnimSlot(const _char* szStartBone, const _char* szEndBone);
	void Activate_SlotAnim(_uint iSlotIndex, _bool bActive);
	void SetUp_SlotAnimation(_uint iSlotIndex, _uint iAnimIndex, _bool isLoop, _float fLerpTime);
	void SetUp_SlotAnimation(_uint iSlotIndex, _uint iAnimIndex, _bool isLoop);
	_bool Play_SlotAnimation(_uint iSlotIndex, _float fTimeDelta, _float fAnimSpeed = 1.f);

	_bool Is_SlotAnimActive(_uint iSlotIndex) const { return m_vecSlotAnimActive[iSlotIndex]; }
	_bool Is_SlotBone_Total(_uint iBoneIndex);
	//_bool Is_SlotBone(_uint iBoneIndex) const { return m_vecSlotBones[m_iCurrSlotIndex][iBoneIndex]; }
	_bool Is_SlotBone(_uint iBoneIndex) const;

	CAnimation* Get_SlotAnimation(_uint iSlotIndex) { return m_vecSlotAnimation[iSlotIndex]; }
	_uint Get_CurrSlotIndex() { return m_iCurrSlotIndex; }
	_uint Is_SlotState(_uint iSlotIndex) { return (_uint)m_vecSlotState[iSlotIndex]; }
	_bool Is_CurrSlotAnim(_uint iAnimIndex);
	_bool Is_SlotPlaying() { return m_bSlotPlaying; }
	_bool Has_Slot() { return m_vecSlotAnimation.size(); }

	_uint Get_CurrAnimationIndex() const { return m_iCurrentAnimIndex; }
	_uint Get_SlotAnimationIndex(_uint iSlotIndex) const { return m_vecSlotAnimIndex[iSlotIndex]; }
	_uint Get_CurrPlayingAnimIndex() const { return m_iCurrPlayingAnimIndex; }

private:
	const aiScene*			m_pAIScene { nullptr };
	Assimp::Importer		m_Importer {};

	CGameObject* m_pOwner{};
	const _char* m_szRootBone = "";

	//_uint						m_iNumMeshes { 0 };
	std::vector<class CMesh*>			m_Meshes {};
	std::vector<class CMaterial*>		m_Materials {};
	std::vector<class CBone*>			m_Bones {};
	std::vector<class CAnimation*>		m_Animations {};

	TYPE		m_eType { TYPE::MAX };
	STATE		m_eState { STATE::ANIMATION };

	_uint		m_iCurrentAnimIndex { 0 };
	_bool		m_isAnimLoop { FALSE };

	_float		m_fAnimLerpTime{ 0.f };

	_float4x4 m_PreTransformMatrix {};

	// 이름이 맘에 안듣다. : 현재 전이 할 애니메이션의 Channel들의 Bone의 Transformation matrix 에서 추출한 scale, rotation, translation 이다.
	vector<KEYFRAME> m_CurrentChannelKeyFrames; 

	_float m_fProgressRate { 0.f };
	_float m_fPreProgressRate { 0.f };

	// 상하체 전용
	vector<vector<_bool>> m_vecSlotBones{};
	vector<class CAnimation*> m_vecSlotAnimation{};
	vector<STATE> m_vecSlotState;
	vector<_uint> m_vecSlotAnimIndex{};
	vector<_bool> m_vecSlotAnimLoop{};
	vector<_bool> m_vecSlotAnimActive{};
	vector<_float> m_vecSlotAnimLerpTime{};
	_bool m_bSlotPlaying{};

	_uint m_iCurrSlotIndex{};
	_uint m_iCurrPlayingAnimIndex{};

private:
	HRESULT Ready_Meshes(_fmatrix PreTransformMatrix);
	HRESULT Ready_Materials(const _string& strModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParendtIndex = -1);
	HRESULT Ready_Animations();

	HRESULT Save_Model(const _wstring& strFilePath);
	HRESULT Load_Model(const _wstring& strFilePath, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation);

private :
	HRESULT Save_Animation(const _wstring& strFilePath);
	HRESULT Load_Animation(const _wstring& strFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _string& strFilePath, TYPE eType, _fmatrix PreTransformMatrix = XMMatrixIdentity(), _bool isNeedOfRecalculation = false);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
