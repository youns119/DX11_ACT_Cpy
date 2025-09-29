#pragma once

#include "Base.h"

#include "Transform.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_MyTerrain.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "FSM.h"
#include "Model.h"
#include "Navigation.h"
#include "Movement.h"
#include "Collider_AABB.h"
#include "Collider_OBB.h"
#include "Collider_Sphere.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_Instancing_Mesh.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Rope.h"
#include "VIBuffer_Instancing_Mesh_SRT.h"

#include "Physics_Sphere.h"
#include "Physics_Capsule.h"
#include "Physics_Convex.h"
#include "Physics_Controller.h"
#include "Physics_Mesh.h"
#include "Physics_Terrain.h"

/*
1. 게임내에 동적으로 생성될 수 있는 객체들의 원형객체를 보관한다.
2. 레벨별로 원형들을 분리하여 보관한다.
2-1. 모든 레벨에서 공유하는 원형도 따로 모아서 관리한다.
3. 개발자의 선택에 의한 원형객체를 복제하여 사본을 만들고 리턴해주는 기능.
*/

BEGIN(Engine)

class CCloneable;

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	//HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CCloneable* pPrototype);
	//CBase* Clone_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	CCloneable* Clone_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

	vector<_wstring> Get_PrototypeTags(_uint iLevelIndex);

private:
	_uint	m_iNumLevels{};

	//map<const _wstring, CBase*>*	m_pPrototypes{ nullptr };
	//typedef map<const wstring, CBase*> PROTOTYPES;
	//map<const _wstring, CCloneable*>*	m_pPrototypes{ nullptr };
	vector<map<const _wstring, CCloneable*>> m_Prototypes;
	typedef map<const wstring, CCloneable*> PROTOTYPES;

private:
	//CBase* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	CCloneable* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CPrototype_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END
