#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class CVIBuffer_Terrain;
class CTexture;
class CSahder;
class CNavigation;

END

BEGIN(MapTool)

class CTerrain final : public CGameObject
{
	using super = CGameObject;

public:
	struct MYTERRAIN_DESC
	{
		CGameObject::GAMEOBJECT_DESC	GameObjectDesc{};
	};

private:
	enum class TEXTURE_TYPE
	{
		DIFFUSE,
		NORMAL,
		SECOND_NORMAL,
		THIRD_NORMAL,
		MASK,
		BRUSH,

		MAX
	};

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

public:
	_int	Add_Point(_float3& vOutPointPos);
	HRESULT Delete_Point(_uint iIndex);

	HRESULT Add_Cell(const vector<_int>& pPoint);
	HRESULT Delete_PrevCell();

	HRESULT Save_Navigation(const _wstring& strFilePath);
	HRESULT Load_Navigation(const _wstring& strFilePath);

	HRESULT Generate_HeightMap(_float4 vLocalPickPos, _float fRadius, _float fHeightScale, _float fSigma, _bool IsUp);
	HRESULT Save_HeightMap(const _tchar* pHeightMapFile);
	HRESULT Load_HeightMap(const _tchar* pHeightMapFile);
private:
	std::array<CTexture*, (_uint)TEXTURE_TYPE::MAX> m_TextureComs{}; // stl �����̳ʶ�� �� �ʱ�ȭ�� �ʿ���°� �ƴϴ���... �迭�̴ϱ� �־���ϳ�
	//CTexture*				m_TextureComs[(_uint)TEXTURE_TYPE::MAX] {};
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;


};

END
