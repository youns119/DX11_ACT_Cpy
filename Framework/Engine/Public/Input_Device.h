#pragma once
#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
	using super = CBase;

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte Get_DIKeyState(_ubyte byKeyID) const { return m_byKeyState[byKeyID]; }
	_byte Get_DIMouseState(MOUSEKEYSTATE eMouse) const { return m_MouseState.rgbButtons[eMouse]; }
	//_int Get_DIMouseMove(MOUSEMOVESTATE eMouseState) const { return *(((_int*)&m_MouseState) + eMouseState); }
	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState) const { return *(((_long*)&m_MouseState) + eMouseState); }

	_bool Key_Down(KEY eKey) const;
	_bool Key_Press(KEY eKey) const;
	_bool Key_Up(KEY eKey) const;

	_bool Mouse_Down(MOUSEKEYSTATE eMouse) const;
	_bool Mouse_Press(MOUSEKEYSTATE eMouse) const;
	_bool Mouse_Up(MOUSEKEYSTATE eMouse) const;

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK { nullptr };

	LPDIRECTINPUTDEVICE8	m_pKeyBoard { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse { nullptr };

private:
	_byte					m_byPrevKeyState[256] {};		// 키보드에 있는 이전 프레임의 모든 키값을 저장하기 위한 변수
	_byte					m_byKeyState[256] {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_PrevMouseState {};
	DIMOUSESTATE			m_MouseState {};

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void Free(void) override;

};
END

