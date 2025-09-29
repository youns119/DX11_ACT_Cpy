#include "Input_Device.h"

CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
					DIRECTINPUT_VERSION,
					IID_IDirectInput8,
					(void**)&m_pInputSDK, nullptr)))
		return E_FAIL;


	// Ű���� ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();


	return S_OK;
}

void CInput_Device::Update(void)
{
	memcpy(m_byPrevKeyState, m_byKeyState, sizeof(_byte) * 256);
	memcpy(&m_PrevMouseState, &m_MouseState, sizeof(DIMOUSESTATE));

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

_bool CInput_Device::Key_Down(KEY eKey) const
{
	return (!(m_byPrevKeyState[(_uint)eKey] & 0x80) && m_byKeyState[(_uint)eKey] & 0x80);
}

_bool CInput_Device::Key_Press(KEY eKey) const
{
	return (m_byPrevKeyState[(_uint)eKey] & 0x80 && m_byKeyState[(_uint)eKey] & 0x80);
	// ? Ȥ�� �׳� ���� ������ ó���ҰŶ�� ���� �����ӿ� ������ �ʾƵ� �ɵ�?
}

_bool CInput_Device::Key_Up(KEY eKey) const
{
	return ((m_byPrevKeyState[(_uint)eKey] & 0x80) && !(m_byKeyState[(_uint)eKey] & 0x80));
}

_bool CInput_Device::Mouse_Down(MOUSEKEYSTATE eMouse) const
{
	return (!(m_PrevMouseState.rgbButtons[eMouse] & 0x80) && (m_MouseState.rgbButtons[eMouse] & 0x80));
}

_bool CInput_Device::Mouse_Press(MOUSEKEYSTATE eMouse) const
{
	return (m_PrevMouseState.rgbButtons[eMouse] & 0x80 && m_MouseState.rgbButtons[eMouse] & 0x80);
}

_bool CInput_Device::Mouse_Up(MOUSEKEYSTATE eMouse) const
{
	return ((m_PrevMouseState.rgbButtons[eMouse] & 0x80) && !(m_MouseState.rgbButtons[eMouse] & 0x80));
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInst, hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free(void)
{
	super::Free();

	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}