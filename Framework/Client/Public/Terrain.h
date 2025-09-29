#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CVIBuffer_Terrain;
class CTexture;
class CSahder;
class CNavigation;
class CPhysics_Terrain;

END

BEGIN(Client)

class CTerrain final : public CGameObject
{
	//using super = CGameObject;
	typedef CGameObject super;

public:
	struct MYTERRAIN_DESC
	{
		CGameObject::GAMEOBJECT_DESC	GameObjectDesc;
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
		MSEG,
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

	HRESULT Load_HeightMap(const _tchar* pHeightMapFile);

private:
	std::array<CTexture*, (_uint)TEXTURE_TYPE::MAX> m_TextureComs {}; // stl 컨테이너라고 꼭 초기화가 필요없는건 아니더라... 배열이니까 있어야하네
	//CTexture*				m_TextureComs[(_uint)TEXTURE_TYPE::MAX] {};
	CShader*				m_pShaderCom { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom { nullptr };
	CNavigation*			m_pNavigationCom { nullptr };
	CPhysics_Terrain*		m_pPhysics_Terrain { nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;


};

END
