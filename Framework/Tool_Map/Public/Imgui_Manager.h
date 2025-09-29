#pragma once

#include "Tool_Define.h"
#include "GameInstance.h"
#include "Base.h"

BEGIN(MapTool)

class CImgui_Manager final : public CBase
{
public:
	static HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static void Update();
	static void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static void Release();

};

END