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
1. ���ӳ��� �������� ������ �� �ִ� ��ü���� ������ü�� �����Ѵ�.
2. �������� �������� �и��Ͽ� �����Ѵ�.
2-1. ��� �������� �����ϴ� ������ ���� ��Ƽ� �����Ѵ�.
3. �������� ���ÿ� ���� ������ü�� �����Ͽ� �纻�� ����� �������ִ� ���.
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
