#include "DebugRender.h"

#include "DebugDraw.h"
#include "GameInstance.h"

CDebugRender::CDebugRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_Batch(pContext)
	, m_Effect(pDevice)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CDebugRender::Initialize()
{
	m_Effect.SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {};

	m_Effect.GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilState()))
		return E_FAIL;

	return S_OK;
}

void CDebugRender::Render()
{
	auto pGameInstance = CGameInstance::GetInstance();

	m_Effect.SetWorld(XMMatrixIdentity());
	m_Effect.SetView(pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::VIEW));
	m_Effect.SetProjection(pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::PROJ));

	m_Effect.Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	Render_Sphere();
	Render_Triangle();

	for (auto& RenderFunction : m_RenderFunctionQueue)
		RenderFunction();

	m_RenderFunctionQueue.clear();
}

void CDebugRender::Render_NonBlend()
{
	m_pContext->OMSetDepthStencilState(m_pDSS, 0);
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	auto pGameInstance = CGameInstance::GetInstance();

	m_Effect.SetWorld(XMMatrixIdentity());
	m_Effect.SetView(pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::VIEW));
	m_Effect.SetProjection(pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::PROJ));

	m_Effect.Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	Render_Sphere();
	Render_Capsule();
	Render_Qaud();
	Render_Ray();
	Render_Grid();
}

void CDebugRender::Render_Blend()
{
	m_pContext->OMSetDepthStencilState(m_pDSS, 0);
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	auto pGameInstance = CGameInstance::GetInstance();

	m_Effect.SetWorld(XMMatrixIdentity());
	m_Effect.SetView(pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::VIEW));
	m_Effect.SetProjection(pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::PROJ));

	m_Effect.Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);
	Render_Triangle();

	for (auto& RenderFunction : m_RenderFunctionQueue)
		RenderFunction();
}

void CDebugRender::Add_DebugRender_Sphere(DirectX::BoundingSphere Sphere, _float4 vColor)
{
	m_SphereRenderQueue.push_back({ Sphere, vColor });
}

void CDebugRender::Add_DebugRender_Triangle(_float3 Points[3], _float4 vColor)
{
	std::array<_float3, 3> PointArray {};
	memcpy(PointArray.data(), Points, sizeof(_float3) * 3);

	m_TriangleRenderQueue.push_back({ PointArray, vColor });
}

void CDebugRender::Add_DebugRender_Quad(_vec3 Points[4], _vec4 vColor)
{
	std::array<_vec3, 4> PointArray {};
	memcpy(PointArray.data(), Points, sizeof(_vec3) * 4);

	m_QuadRenderQueue.push_back({ PointArray, vColor });
}

void CDebugRender::Add_DebugRender_Box(_mat WorldMatrix, _vec4 vColor)
{
	_vec3 vCuboid[8] {};

	_vec4 vHalfRight = WorldMatrix.right * 0.5f;
	_vec4 vHalfUp = WorldMatrix.up * 0.5f;
	_vec4 vHalfLook = WorldMatrix.look * 0.5f;

	vCuboid[0] = WorldMatrix.position - vHalfRight + vHalfUp - vHalfLook;
	vCuboid[1] = WorldMatrix.position + vHalfRight + vHalfUp - vHalfLook;
	vCuboid[2] = WorldMatrix.position + vHalfRight - vHalfUp - vHalfLook;
	vCuboid[3] = WorldMatrix.position - vHalfRight - vHalfUp - vHalfLook;
	vCuboid[4] = WorldMatrix.position - vHalfRight + vHalfUp + vHalfLook;
	vCuboid[5] = WorldMatrix.position + vHalfRight + vHalfUp + vHalfLook;
	vCuboid[6] = WorldMatrix.position + vHalfRight - vHalfUp + vHalfLook;
	vCuboid[7] = WorldMatrix.position - vHalfRight - vHalfUp + vHalfLook;

	for (_uint i = 0; i < 4; ++i)
	{
		array<_vec3, 4> Quad =
		{
			_vec3(vCuboid[(i + 0) % 4]),
			_vec3(vCuboid[(i + 1) % 4]),
			_vec3(vCuboid[(i + 1) % 4 + 4]),
			_vec3(vCuboid[(i + 0) % 4 + 4]),
		};
		m_QuadRenderQueue.push_back({ Quad, vColor });
	}
	for (_uint i = 0; i < 2; ++i)
	{
		array<_vec3, 4> Quad =
		{
			_vec3(vCuboid[4 * i + 0]),
			_vec3(vCuboid[4 * i + 1]),
			_vec3(vCuboid[4 * i + 2]),
			_vec3(vCuboid[4 * i + 3]),
		};
		m_QuadRenderQueue.push_back({ Quad, vColor });
	}
}

void CDebugRender::Add_DebugRender_Ray(_vec3 vOrigin, _vec3 vDir, _bool isNormalize, _vec4 vColor)
{
	m_RayQueue.push_back({ {vOrigin, vDir}, isNormalize, vColor });
}

void CDebugRender::Add_DebugRender_Grid(_vec3 vOrigin, _vec3 vAxisX, _vec3 vAxisY, _float fUnitX, _float fUnitY, _vec4 vColor)
{
	_float fInt = 0.f;
	assert(modf(vAxisX.Length() / fUnitX, &fInt) == 0.f);
	assert(modf(vAxisY.Length() / fUnitY, &fInt) == 0.f);

	m_GridQueue.push_back({ {vOrigin, vAxisX, vAxisY, fUnitX, fUnitY}, vColor });
}

void CDebugRender::Add_DebugRender_Capsule(_vec3 vPosition, _vec3 vDirection, _float fRadius, _float fHeight, _float4 vColor)
{
	m_CapsuleQueue.push_back({ CAPSULE_DESC{ vPosition, vDirection, fRadius, fHeight }, vColor });
}

void CDebugRender::Clear()
{
	m_SphereRenderQueue.clear();
	m_TriangleRenderQueue.clear();
	m_QuadRenderQueue.clear();
	m_GridQueue.clear();
	m_RayQueue.clear();
	m_RenderFunctionQueue.clear();
}

HRESULT CDebugRender::Ready_DepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc {};
	DepthStencilDesc.DepthEnable = TRUE;                        // 깊이 테스트 활성화
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // 깊이 버퍼에 쓰기 허용
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;       // 가까운 픽셀만 통과

	DepthStencilDesc.StencilEnable = FALSE;                       // 스텐실 비활성화
	DepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// 스텐실 프론트 페이스 동작 정의 (사용하지 않으므로 기본값)
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 스텐실 백 페이스 동작 정의
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;

	HRESULT hr = m_pDevice->CreateDepthStencilState(&DepthStencilDesc, &m_pDSS);
	if (FAILED(hr))
	{
		MSG_BOX("Fail To Create Depth stencil stae");
		return E_FAIL;
	}

	return S_OK;
}

void CDebugRender::Render_Sphere()
{
	for (auto& Info : m_SphereRenderQueue)
	{
		_vector vColor = XMLoadFloat4(&Info.second);

		m_Batch.Begin();
		DX::Draw(&m_Batch, Info.first, vColor);
		m_Batch.End();
	}
	m_SphereRenderQueue.clear();
}

void CDebugRender::Render_Triangle()
{
	for (auto& Info : m_TriangleRenderQueue)
	{
		_vector vColor = XMLoadFloat4(&Info.second);

		_vector vPointA = XMLoadFloat3(&Info.first[0]);
		_vector vPointB = XMLoadFloat3(&Info.first[1]);
		_vector vPointC = XMLoadFloat3(&Info.first[2]);

		m_Batch.Begin();
		DX::DrawTriangle(&m_Batch, vPointA, vPointB, vPointC, vColor);
		m_Batch.End();
	}
	m_TriangleRenderQueue.clear();
}

void CDebugRender::Render_Qaud()
{
	for (auto& Info : m_QuadRenderQueue)
	{
		_vector vColor = Info.second._vector();

		_vector vPointA = Info.first[0]._vector();
		_vector vPointB = Info.first[1]._vector();
		_vector vPointC = Info.first[2]._vector();
		_vector vPointD = Info.first[3]._vector();

		m_Batch.Begin();
		DX::DrawQuad(&m_Batch, vPointA, vPointB, vPointC, vPointD, vColor);
		m_Batch.End();
	}

	m_QuadRenderQueue.clear();
}

void CDebugRender::Render_Ray()
{
	for (auto& Info : m_RayQueue)
	{
		_vector vOrigin = std::get<0>(Info).first._vector();
		_vector vDir = std::get<0>(Info).second._vector();
		_bool isNormalize = std::get<1>(Info);
		_vector vColor = std::get<2>(Info)._vector();

		m_Batch.Begin();
		DX::DrawRay(&m_Batch, vOrigin, vDir, isNormalize, vColor);
		m_Batch.End();
	}

	m_RayQueue.clear();
}

void CDebugRender::Render_Grid()
{
	for (auto& Info : m_GridQueue)
	{
		_vector vAxisX = Info.first.vAxisX._vector();
		_vector vAxisY = Info.first.vAxisY._vector();
		_vector vOrigin = (vAxisX + vAxisY) * 0.5f + Info.first.vOrgin._vector();
		size_t iXDivs = static_cast<size_t>(Info.first.vAxisX.Length() / Info.first.fUnitX);
		size_t iYDivs = static_cast<size_t>(Info.first.vAxisY.Length() / Info.first.fUnitY);

		_vector vColor = Info.second._vector();
		
		m_Batch.Begin();
		DX::DrawGrid(&m_Batch, vAxisX, vAxisY, vOrigin, iXDivs * 2, iYDivs * 2, vColor);
		m_Batch.End();
	}

	m_GridQueue.clear();
}

void CDebugRender::Render_Capsule()
{
	for (auto& Info : m_CapsuleQueue)
	{
		_vector vOrigin = Info.first.vOrigin._vector();
		_vector vDirection = Info.first.vDirection._vector();
		_float fRadius = Info.first.fRadius;
		_float fHeight = Info.first.fHeight;
		_vector vColor = Info.second._vector();

		m_Batch.Begin();
		DX::DrawCapsule3D(&m_Batch, vOrigin, vDirection, fRadius, fHeight, vColor);
		m_Batch.End();
	}

	m_CapsuleQueue.clear();
}

CDebugRender* CDebugRender::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto pInstance = new CDebugRender(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Create : CDebugRender");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebugRender::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pDSS);

	Safe_Release(m_pInputLayout);

	Clear();

}
