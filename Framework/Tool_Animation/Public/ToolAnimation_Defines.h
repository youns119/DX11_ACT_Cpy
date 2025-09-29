#pragma once

namespace ToolAnimation
{
	const unsigned int g_iWindowSizeX = 1280;
	const unsigned int g_iWindowSizeY = 720;

	enum LEVELID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_TOOL,
		LEVEL_END
	};

	enum class SHADER_PASS_MESH
	{
		DEFAULT,
		BASE_COLOR_NON_NORMAL,
		NON_NORMAL_MAPPING,
		FOWARD,
		UV_WRAPPING,
		UV_WRAPPING_EXCEPT_NORMAL,
		SHADOW,
		BASE_COLOR,
		BASE_COLOR_UV_TRANSFORM,
		NON_CULLING,
		DISSOLVE,
	};

	enum class SHADER_PASS_ANIMMESH
	{
		DEFAULT,
		NON_NORMAL_MAPPING,
		FOWARD,
		SHADOW,
		BASE_COLOR,
		DISSOLVE,
		ENEMYDAMAGED,
		RIMLIGHT
	};

	enum class COLLISION_GROUP
	{
		DEFAULT,
		CINECAMERA,
		CAMERADEBUG,
	};
}

using namespace ToolAnimation;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#include <process.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"

#include <shellapi.h>