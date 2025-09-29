#include "pch.h"
#include "Splitter_Event_Fear.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"
#include "CallbackUpdate.h"

CSplitter_Event_Fear::CSplitter_Event_Fear()
{
}

HRESULT CSplitter_Event_Fear::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));

	return S_OK;
}

HRESULT CSplitter_Event_Fear::Execute()
{
	CEnemy_Splitter* pSplitter = static_cast<CEnemy_Splitter*>(m_pGameObject);
	pSplitter->Set_IsFear(true);
	if (false == pSplitter->Is_GrayScaling())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		_matrix BoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("GlowMask.18"));
		_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
		_vector vPos = (BoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
		_vector vPlayerPos = static_cast<CTransform*>(CGameInstance::GetInstance()->Find_Component(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player"), g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
		_vector vLook = XMVectorSetW(XMVector3Normalize(vPlayerPos - vPos), 0.f);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_FEAR, vPos, vLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_FEAR_SCREW, vPos, vLook, false, nullptr);
		pSplitter->Set_IsGrayScaling(true);

		COLORCORRECTION_PARAMS InitialColorCorrection_Params = pGameInstance->Get_ColorCorrectionParams();
		_float fInitialSaturation = InitialColorCorrection_Params.fSaturation;

		CCallbackUpdate* pCallbackUpdate = static_cast<CCallbackUpdate*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate"));
		pCallbackUpdate->Add_Update_Callback([fInitialSaturation, pGameInstance](_float fElapsed, _float  fTimeDelta)->_bool
			{
				_float fFactor = min(1.f, fElapsed / 1.5f);
				_float fSaturation = CUtils::Lerp_Float(fInitialSaturation, 0.f, min(1.f, 3.f * sinf(fFactor * XM_PI)));

				COLORCORRECTION_PARAMS ColorCorrectionParams = pGameInstance->Get_ColorCorrectionParams();
				ColorCorrectionParams.fSaturation = fSaturation;

				pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
				
				return fElapsed > 1.5f;
			});

	}
	else {
		pSplitter->Set_IsGrayScaling(false);
		pSplitter->Set_IsFear(false);
	}		
	
	return S_OK;
}

CSplitter_Event_Fear* CSplitter_Event_Fear::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_Fear* pInstance = new CSplitter_Event_Fear();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Fear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Fear::Free()
{
	__super::Free();
}