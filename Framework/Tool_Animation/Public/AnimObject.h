#pragma once

#include "ToolAnimation_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;
class CAnimation;
class CBone;

END

BEGIN(ToolAnimation)

class CCineCamera_Tool;

class CAnimObject final
	: public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
		_wstring strModelTag;
		LEVELID eLevelID{ LEVEL_STATIC };
	};

private:
	CAnimObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CAnimObject(const CAnimObject& _Prototype);
	virtual ~CAnimObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint Get_NumAnimation();
	vector<CAnimation*>* Get_Animations();
	CAnimation* Get_CurrAnimation();

	vector<CBone*>* Get_Bones();
	const _float4x4* Get_CombinedMatrix(const _char* _pBoneName);

	CCineCamera_Tool* Get_CineCamera() { return m_pCineCamera; }

public:
	void SetUp_Animation(_uint _iAnimIndex, _bool _bLoop, _float _fLerpTime);
	void SetUp_Animation(_uint _iAnimIndex, _bool _bLoop);
	void Set_Animation_TrackPosition(_double _fTrackPosition);

public:
	void Add_AnimNotify(_uint iAnimIndex, _double _fTrackPosition, string& _strNotifyName);
	void Change_AnimNotify(_uint _iAnimIndex, string& _strNotifyName, string& _strChangeName, _double _fTrackPosition, _double _fChangePosition);
	void Delete_AnimNotify(_uint _iAnimIndex, string& _strNotifyName, _double _fTrackPosition);

public:
	HRESULT Change_Model(const _wstring& _strModelTag, LEVELID _eLevelID);

public:
	void Reset_Animation();
	void Reset_RootBone(_uint _iAnimIndex);

public:
	HRESULT Save_Animation(_uint _iAnimIndex);

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};

private:
	CCineCamera_Tool* m_pCineCamera{};

private:
	const _char* Get_ModelRootName(const _wstring& _strModelName);

private:
	HRESULT Ready_Components(const _wstring& _strModelTag, LEVELID _eLevelID);
	HRESULT Ready_PartObject();
	HRESULT Ready_Events();

public:
	static CAnimObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};

END