#pragma once

namespace MapTool
{
	const unsigned int g_iWindowSizeX = 1280;
	const unsigned int g_iWindowSizeY = 720;

	enum LEVELID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_LOGO,
		LEVEL_TOOL,
		LEVEL_END
	};
	enum class SHADER_PASS_INSTANCINGMESH
	{
		DEFAULT,
		BASE_COLOR_NON_NORMAL,
		UV_WRAPPING,
		UV_WRAPPING_EXCEPT_NORMAL,
		BASE_COLOR_NONCULLING,
		NON_CULLING,
		CROSSPLANE,
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
	};

	enum class SHADER_PASS_ANIMMESH
	{
		DEFAULT,
		NON_NORMAL_MAPPING,
		FOWARD,
		SHADOW,
		BASE_COLOR,
	};

	enum class COLLISION_GROUP
	{
		DEFAULT,
		PLAYER,
		PLAYER_LOCKON,
		PLAYER_WEAPON,
		MONSTER,
		MONSTER_HIT,
		SWING_HIT,
		ITEM,
		NPC,
		TRIGGER,
		MAP_INTERACTION,
		PINBALL,
	};

}
using namespace MapTool;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#include <process.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"