#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

#define	DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#include <Effects11/d3dx11effect.h>

#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK\ScreenGrab.h>

#include <DirectXTex/DirectXTex.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

#include <DirectXTK/spriteBatch.h>
#include <DirectXTK/spriteFont.h>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#pragma comment(lib, "fmod_vc.lib")

#include <io.h>

//#ifdef _DEBUG
//#pragma comment(lib, "DirectXTKd.lib")
//#else
//#pragma comment(lib, "DirectXTK.lib")
//#endif
using namespace DirectX;

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <utility>
#include <functional>
#include <random>
#include <bitset>
using namespace std;

/* for PhysX */
#include <PhysX/PxPhysics.h>
#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/PxConfig.h>
#define PVD_HOST "127.0.0.1"
using namespace physx;

//#define PROFILING

#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

namespace Engine
{
	static wstring		g_strTransformTag = TEXT("Com_Transform");

	/*const unsigned int g_iMaxWidth = 16384;
const unsigned int g_iMaxHeight = 9216;	*/

	//const unsigned int g_iMaxWidth = 8192;
	//const unsigned int g_iMaxHeight = 4608;

	//const unsigned int g_iMaxWidth = 4086;
	//const unsigned int g_iMaxHeight = 2304;


}
using namespace Engine;


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG
