#pragma once

#include "Base.h"
#include "Physics_Controller.h"

#include "Physics_Controller.h"

BEGIN(Engine)
class CPhysics;
class CPhysics_Chunk;
class CPhsics_SimulationCallback;
class CPhysics_ControllerHitCallback;
class CPhysics_ControllerFilterCallback;

class CPhysics_Manager final : public CBase
{
	using super = CBase;

private:
	CPhysics_Manager() = default;
	virtual ~CPhysics_Manager() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta); // simulate
	void Clear();


	void Simulate_Physics(CPhysics* pPhysics);
	void Simulate_Physics(CPhysics_Controller* pController);
	PxController* Create_Controller(
		const CPhysics_Controller::DESC& Desc, 
		CPhysics_Controller::USER_DATA* pUserData, 
		_Out_ PxControllerFilters& outFilters, 
		_Out_ _uint& iChunkIndexX, 
		_Out_ _uint& iChunkIndexZ);
	void Add_Movable_To_Chunk(const _vec3& vPosition, _Out_ _uint& iChunkIndexX, _Out_ _uint& iChunkIndexZ);

	void Resist_Physics_To_Chunk(_vec3 vMin, _vec3 vMax, CPhysics* pPhysics);
	void Update_ChunkIndex_On_Move(_vec3 vMovedPosition, _Inout_ _uint& iChunkIndexX, _Inout_ _uint& iChunkIndexZ);
	HRESULT SetUp_Scene(const _vec2& vMin, const _vec2& vMax, _float fChunkSize); // ���� ���� �ν��Ͻ��� �ȿű�
	void Remove_Controller_From_Chunk(_uint iX, _uint iZ);

	_bool Ray_Cast(_vec3 vOrigin, _vec3 vDir, _float fLength, _Out_ _float& fDist, _uint iFilter) const;

	void Erase_DynamicPhysicsHash(PxActor* pActor);
	void Resist_DynamicPhysicsHash(PxActor* pActor, CPhysics* pPhysics);

private:
	PxDefaultErrorCallback		m_ErrorCallback;
	PxDefaultAllocator			m_Allocator;
	PxFoundation*				m_pFoundation { nullptr };

	/* 2. �ֻ��� PxPhysics �� ����� ���� �ʿ��� ���� */
	PxPhysics*			m_pPhysics { nullptr };
	PxPvd*				m_pPvd { nullptr };

	/* 4. CPU���ҽ��� ȿ�������� ������ �� �ֵ��� �ϱ� ���� �����ϴ� ���� ��õ */
	PxDefaultCpuDispatcher*			m_pCpuDispatcher { nullptr };
	PxScene*						m_pScene { nullptr };
	CPhsics_SimulationCallback*		m_pSimulationCallback { nullptr };

	PxControllerManager*					m_pControllerManager { nullptr };
	CPhysics_ControllerHitCallback*			m_pControllerHitCallback { nullptr };
	CPhysics_ControllerFilterCallback*		m_pControllerFilterCallback { nullptr };

	/* �ùķ��̼� �ϴ� ������Ʈ��, GameObject�� ������ ��ġ ����ȭ */
	vector<CPhysics*>				m_PhysicsComs;
	vector<CPhysics_Controller*>	m_ControllerComs;

	vector<vector<CPhysics_Chunk*>> m_Chunks;

	_float			m_fChunkSize { 16.f };
	_vec2			m_vWorldMin { 0.f, -32.f };
	_vec2			m_vWorldMax { 32.f * 32.f, 32.f * 32.f };

	/* ���۷��� ī��Ʈ ���� ���ϴ� ��� ��۸� �����Ͱ� ������ �ؾ� �Ѵ�. */
	map<PxActor*, CPhysics*> m_ActorToDynamicPhysicsCom;

private:
	HRESULT Ready_Scene();
	HRESULT Ready_ControllerManager();

	void Add_ChunkCount(_uint iX, _uint iZ);
	void Sub_ChunkCount(_uint iX, _uint iZ);
	_bool Out_Of_Boundary(_int iX, _int iZ);

public:
	static CPhysics_Manager* Create();
	virtual void Free() override;
};
END
