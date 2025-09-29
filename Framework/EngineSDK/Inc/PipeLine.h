#pragma once

#include "Base.h"

/* ���� ������ ���� ��, ��������� �����Ѵ�. */
/* �ݺ����� ������ ���� �� ����� ����ĵ� ���Ͽ� �����Ѵ�. */
/* Set, Get�Լ��� ���� ����� ���� �Ǵ� ���t����. */

BEGIN(Engine)

class CPipeLine final : public CBase
{
	using super = CBase;

public:
	enum class TRANSFORM
	{
		VIEW,
		PROJ,

		MAX
	};

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_float4x4 Get_Transform_Float4x4(TRANSFORM eState) const { 
		return m_TransformationMatrices[(_uint)eState]; 
	}
	_matrix Get_Transform_Matrix(TRANSFORM eState) const { return XMLoadFloat4x4(&m_TransformationMatrices[(_uint)eState]); }
	_float4x4 Get_Transform_Float4x4_Inverse(TRANSFORM eState) const { return m_TransformationInverseMatrices[(_uint)eState]; }
	_matrix Get_Transform_Matrix_Inverse(TRANSFORM eState)const { return XMLoadFloat4x4(&m_TransformationInverseMatrices[(_uint)eState]); }

	void Set_Transform(TRANSFORM eState, _fmatrix TransformMatrix) { XMStoreFloat4x4(&m_TransformationMatrices[(_uint)eState], TransformMatrix); }

	const _float4& Get_CamPosition() const { return m_vCamPosition; }

	HRESULT Initialize();
	void Update();

	
private:
	_float4x4		m_TransformationMatrices[(_uint)TRANSFORM::MAX]{};
	_float4x4		m_TransformationInverseMatrices[(_uint)TRANSFORM::MAX]{};
	_float4			m_vCamPosition {};

public:
	static CPipeLine* Create();
	virtual void Free() override;

};

END
