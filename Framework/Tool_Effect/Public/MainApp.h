#pragma once


#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

/* ��������(Window API) ������ ������ �����ϱ� ����� ������ */
/* �� �ڵ��� �帧�� CMainAppŬ������ �ݺ����� ������Ʈ�� �������� ��ܿ´�. */
BEGIN(Client)

class CMainApp final : public CBase
{
	using super = CBase;
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private: /*������ ����� �̿��ϱ� ���� �����ν��ͽ� Ŭ������ ��üȭ */
	Engine::CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

#ifdef _DEBUG
private:
	_float m_fTimerAcc = {};
	_uint m_iDrawCount = {};
	_tchar m_szFPS[MAX_PATH] = {};
#endif

private:
	HRESULT Start_Level(LEVELID eStartLevel);
	HRESULT Ready_Prototype_For_Static();
	HRESULT Ready_Font(); // todo : Make Texture and Save To png, dds file.

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END