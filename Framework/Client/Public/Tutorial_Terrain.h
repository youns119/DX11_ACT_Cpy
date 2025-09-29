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

class CTutorial_Terrain final : public CGameObject
{
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
	CTutorial_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_Terrain(const CTutorial_Terrain& Prototype);
	virtual ~CTutorial_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Load_HeightMap(const _tchar* pHeightMapFile);

private:
	std::array<CTexture*, (_uint)TEXTURE_TYPE::MAX> m_TextureComs{}; // stl �����̳ʶ�� �� �ʱ�ȭ�� �ʿ���°� �ƴϴ���... �迭�̴ϱ� �־���ϳ�
	//CTexture*				m_TextureComs[(_uint)TEXTURE_TYPE::MAX] {};
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };
	CPhysics_Terrain* m_pPhysics_Terrain{ nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTutorial_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;


};
END

