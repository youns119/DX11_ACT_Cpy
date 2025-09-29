#pragma once

#include "Base.h"

BEGIN(Engine)

class CPixelPicking final : public CBase
{
	using super = CBase;
private:
	CPixelPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPixelPicking() = default;

public:
	HRESULT Initialize();
	void Update();

	_bool Compute_PickPos(_float3& Out);
	_bool Compute_ObjectID(_uint& Out);
	_vector Compute_WorldPos(const _float2& vViewportPos);
	_bool Compute_ObjectID_WorldPos(const _float2& vViewportPos, _uint& Out);


private:
	HWND		m_hWnd {};
	_float		m_fViewportWidth {};
	_float		m_fViewportHeight {};

	ID3D11Device* m_pDevice { nullptr };
	ID3D11DeviceContext* m_pContext { nullptr };

	/* ��ŷ�� ���� �ȼ��� ���̸� �����س��Ҵ� ����Ÿ���ؽ���(map, unmap�� �ȵǴϱ�)�� �����Ͽ� �����ϴ� �ؽ��� ����. */
	ID3D11Texture2D* m_pTexture2D { nullptr };
	ID3D11Texture2D* m_pTextureObjectID { nullptr };
	vector<_float2> m_vDepthDesc;

	class CGameInstance* m_pGameInstance { nullptr };

private:
	_bool Is_OutOfBoundary(_float2 vMouse) const;

public:
	static CPixelPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END
