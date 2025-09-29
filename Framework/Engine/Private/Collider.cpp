#include "Collider.h"

#include "GameInstance.h"

#include "Collider_AABB.h"
#include "GameObject.h" // 콜리전 매니저 만들걸

_uint CCollider::s_iIDCounter { 0 };

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
	: super(Prototype)
#ifdef _DEBUG
	, m_pBatch { Prototype.m_pBatch }
	, m_pEffect { Prototype.m_pEffect }
	, m_pInputLayout { Prototype.m_pInputLayout }
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

	m_iID = s_iIDCounter;
	++s_iIDCounter;
}

HRESULT CCollider::Initialize_Prototype()
{
#ifdef _DEBUG
	using DirectX::VertexPositionColor;

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif


    return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	m_iCollisionGroup = pDesc->iCollisionGroup;

	m_pGameInstance->Add_ColliderGroup(pDesc->pOwner, this, pDesc->iCollisionGroup);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::PROJ));

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);


	return S_OK;
}
#endif // _DEBUG

void CCollider::Free()
{
    super::Free();

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif
}
