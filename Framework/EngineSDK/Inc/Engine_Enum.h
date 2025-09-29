#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum class KEY
	{
		F1 = DIK_F1, F2 = DIK_F2, F3 = DIK_F3, F4 = DIK_F4, F5 = DIK_F5, F6 = DIK_F6, F7 = DIK_F7, F8 = DIK_F8, F9 = DIK_F9,
		F10 = DIK_F10, F11 = DIK_F11, F12 = DIK_F12,

		NUM_1 = DIK_1, NUM_2 = DIK_2, NUM_3 = DIK_3, NUM_4 = DIK_4, NUM_5 = DIK_5,
		NUM_6 = DIK_6, NUM_7 = DIK_7, NUM_8 = DIK_8, NUM_9 = DIK_9, NUM_0 = DIK_0,

		Q = DIK_Q, W = DIK_W, E = DIK_E, R = DIK_R, T = DIK_T, Y = DIK_Y, U = DIK_Y, I = DIK_I, O = DIK_O, P = DIK_P,
		A = DIK_A, S = DIK_S, D = DIK_D, F = DIK_F, G = DIK_G, H = DIK_H, J = DIK_J, K = DIK_K, L = DIK_L,
		Z = DIK_Z, X = DIK_X, C = DIK_C, V = DIK_V, B = DIK_B, N = DIK_N, M = DIK_M,

		LSHIFT = DIK_LSHIFT, RSHIFT = DIK_RSHIFT, ENTER = DIK_RETURN, SPACE = DIK_SPACE, TAB = DIK_TAB,
		UP = DIK_UP, DOWN = DIK_DOWN, RIGHT = DIK_RIGHT, LEFT = DIK_LEFT,

		LCONTROL = DIK_LCONTROL, RCONTORL = DIK_RCONTROL,
		EQUALS = DIK_EQUALS,

		BACKSPACE = DIK_BACKSPACE,
		DELETEKEY = DIK_DELETE, // 예약된게 있어서 얘만 이렇게 했어요
		ESCAPE = DIK_ESCAPE,
	};

	//enum class RENDERGROUP
	//{
	//	PRIORITY,
	//	NONBLEND,
	//	BLEND,
	//	UI,

	//	MAX
	//};

	enum class LIGHT_TYPE
	{
		DIRECTIONAL,
		POINT,
		SPOT,

		MAX
	};

	enum class MATERIAL_TEXTURE
	{
		NONE = aiTextureType_NONE,
		DIFFUSE = aiTextureType_DIFFUSE,
		SCECULAR = aiTextureType_SPECULAR,
		AMBIENT = aiTextureType_AMBIENT,
		EMISSIVE = aiTextureType_EMISSIVE,
		HEIGHT = aiTextureType_HEIGHT,
		NORMAL = aiTextureType_NORMALS,
		SHININESS = aiTextureType_SHININESS,
		OPACITY = aiTextureType_OPACITY,
		DISPLACEMENT = aiTextureType_DISPLACEMENT,
		LIGHTMAP = aiTextureType_LIGHTMAP,
		REFLECTION = aiTextureType_REFLECTION,
		BASE_COLOR = aiTextureType_BASE_COLOR,
		CAMERA = aiTextureType_NORMAL_CAMERA,
		EMISSION_COLOR = aiTextureType_EMISSION_COLOR,
		METALNESS = aiTextureType_METALNESS,
		ROUGHNESS = aiTextureType_DIFFUSE_ROUGHNESS,
		OCCLUSION = aiTextureType_AMBIENT_OCCLUSION,
		UNKNOWN = aiTextureType_UNKNOWN,
		// ㅅㅂ 적당히 많아야지
		MAX
	};

	static vector<string> MaterialTypesToString {
		"none",		"diffuse",		"scecular",		"ambient",	"emissive",
		"height",	"normal",		"shininess",	"opacity",	"dispalcement",
		"lightmap",	"reflection",	"base color",	"camera",	"emission_color",
		"mealness",	"roughness",	"occlusion",	"unknown"
	};

	enum class SORTING
	{
		LEFT,
		MIDDLE,
		RIGHT,
		NONE
	};

	enum
	{
		MATERIAL_TEXTURE_END = MATERIAL_TEXTURE::MAX,
		SOUND_END = 32,
	};
}
#endif // Engine_Enum_h__
