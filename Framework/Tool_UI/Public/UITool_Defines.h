#pragma once

//#include <process.h>
//#include <windows.h>
#include <commdlg.h>

namespace UITool
{
	const unsigned int g_iWindowSizeX = 1280;
	const unsigned int g_iWindowSizeY = 720;

	enum LEVELID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_UITOOL,
		LEVEL_END
	};

	enum class SHADER_PASS_UI
	{
		DEFAULT,
		BLEND,
		COLOR,
		COLOR_BLEND,
		RING,
		FILTER_BLEND,
		MENU_BUTTON,
		OPTION_BUTTON,
		CHATBUBBLE,
		SPRITE,
		BUTTON,
		INVENTORY,
		LOCK,
		STRETCH,
		HURT,
		STRETCH_BAR,
		STRETCH_BAR_EFFECT,
		NOISE,
		NOISE_EDGE,
		COLOR_NONCULL,
		BORDER,
		STRETCH_DEFAULT,
		SHOP_BORDER,
		OUTLINE,
		SHELL_BORDER,
		SKILL,
		GAUGE,
	};
}
using namespace UITool;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#include <process.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
