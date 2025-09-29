#include "pch.h"
#include "Debug_ObjectInfo.h"

#include "GameInstance.h"

CDebug_ObjectInfo::CDebug_ObjectInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CDebug_ObjectInfo::CDebug_ObjectInfo(const CDebug_ObjectInfo& Prototype)
	: super(Prototype)
{
}

HRESULT CDebug_ObjectInfo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebug_ObjectInfo::Initialize(void* pArg)
{
	return S_OK;
}

void CDebug_ObjectInfo::Priority_Update(_float fTimeDelta)
{
}

void CDebug_ObjectInfo::Update(_float fTimeDelta)
{
	ImGui::Begin("Object info");

	if (ImGui::RadioButton("Pick pixel", m_eMode == MODE::PICK_PIXEL))
		m_eMode = MODE::PICK_PIXEL;

	if (ImGui::RadioButton("Find_Name", m_eMode == MODE::FIND_NAME))
		m_eMode = MODE::FIND_NAME;

	switch (m_eMode)
	{
	case MODE::PICK_PIXEL:
		Find_GameObject_With_ID();
		break;
	case MODE::FIND_NAME:
		Find_GameObject_With_Name();
		break;
	}

	if (nullptr != m_pPickedGameObject)
	{
		if (true == m_pPickedGameObject->Get_IsDead())
			m_pPickedGameObject = nullptr;
	}

	if (nullptr != m_pPickedGameObject)
	{
		ImGui::Separator();
		ImGui::Text("ID: %d", m_pPickedGameObject->Get_ID());
		ImGui::Text(_string("Name: " + CUtils::ConvertToMultibyteChar(m_pPickedGameObject->Get_Name())).c_str());
		ImGui::Separator();

		Debug_Transform(m_pPickedGameObject);
		ImGui::Spacing();
		Debug_Activation(m_pPickedGameObject);
	}
	else
	{
		ImGui::Text("nullptr");
	}

	ImGui::End();
}

void CDebug_ObjectInfo::Late_Update(_float fTimeDelta)
{
}

void CDebug_ObjectInfo::Find_GameObject_With_ID()
{
	_bool isImguiHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered();

	if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::DIM_LB) && !isImguiHovered)
	{
		_uint iOut = 0;
		if (true == m_pGameInstance->Compute_ObjectID(iOut))
		{
			m_pPickedGameObject = m_pGameInstance->Find_GameObject_With_ID(m_pGameInstance->Get_CurrentLevelIndex(), iOut);
		}
	}
}

void CDebug_ObjectInfo::Find_GameObject_With_Name()
{
	CGameObject* pGameObject = nullptr;

	_char szName[MAX_PATH] {};
	if (ImGui::InputText("Name", szName, MAX_PATH))
	//if (ImGui::Button("Find"))
	{
		_wstring strName = CUtils::ConvertToWideChar(_string(szName));

		_uint iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
		const vector<_wstring> LayerTags = m_pGameInstance->Get_LayerTags(iLevelID);
		for (auto& strLayerTag : LayerTags)
		{
			m_pPickedGameObject = m_pGameInstance->Find_GameObject_With_Name(iLevelID, strLayerTag, strName);
			if (nullptr != m_pPickedGameObject)
				break;
		}
	}
}

void CDebug_ObjectInfo::Debug_Transform(CGameObject* pGameObject)
{

	CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(g_strTransformTag));


	_float3 vScale = pTransform->Get_Scale();

	const _float4x4& WorldMatrix = pTransform->Get_WorldMatrix_Ref();
	// 스케일? 
	_float fPitch, fYaw, fRoll;

	_float fZ_Up = WorldMatrix.m[2][1];

	// Compute pitch (rotation around X-axis)
	fPitch = std::asin(-WorldMatrix.m[2][1]);

	// Check for gimbal lock
	if (std::abs(WorldMatrix.m[2][1]) < 0.9999f)
	{
		// Compute yaw (rotation around Y-axis) and roll (rotation around Z-axis)
		fYaw = std::atan2(WorldMatrix.m[2][0], WorldMatrix.m[2][2]);
		fRoll = std::atan2(WorldMatrix.m[0][1], WorldMatrix.m[1][1]);
	}
	else
	{
		// Gimbal lock case
		fYaw = 0.0f; // Can set to 0 or some default value
		fRoll = std::atan2(-WorldMatrix.m[1][0], WorldMatrix.m[0][0]);
	}

	// Return the Euler angles (pitch, yaw, roll)
	_float3 vRotation = _float3(fPitch, fYaw, fRoll);
	_float3 vRotationDegree = vRotation;
	vRotationDegree.x = XMConvertToDegrees(vRotation.x);
	vRotationDegree.y = XMConvertToDegrees(vRotation.y);
	vRotationDegree.z = XMConvertToDegrees(vRotation.z);

	_float3 vPosition {};
	XMStoreFloat3(&vPosition, pTransform->Get_State(CTransform::STATE::POSITION));

	_float3 vPreRotation = vRotationDegree;

	ImGui::DragFloat3(" Scale", (float*)&vScale, 0.1f);
	ImGui::DragFloat3(" Rotation", (float*)&vRotationDegree, 0.1f);
	ImGui::DragFloat3(" Position", (float*)&vPosition, 0.2f);

	pTransform->Set_Scale(vScale);

	if (abs(vPreRotation.x - vRotationDegree.x) > 0.005f ||
		abs(vPreRotation.y - vRotationDegree.y) > 0.005f ||
		abs(vPreRotation.z - vRotationDegree.z) > 0.005f)
	{
		// 값에 유효한 변화가 있을 경우만 저장하기 
		pTransform->Rotate(vRotationDegree.x, vRotationDegree.y, vRotationDegree.z);
	}

	pTransform->Locate(vPosition);
}

void CDebug_ObjectInfo::Debug_Activation(CGameObject* pGameObject)
{
	_bool isActive = pGameObject->Get_IsActive();
	if (ImGui::Checkbox("isActive", &isActive))
		pGameObject->Set_IsActive(isActive);
}

CDebug_ObjectInfo* CDebug_ObjectInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_ObjectInfo* pInstance = new CDebug_ObjectInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CDebug_ObjectInfo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CDebug_ObjectInfo::Clone(void* pArg)
{
	CDebug_ObjectInfo* pInstance = new CDebug_ObjectInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CDebug_ObjectInfo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebug_ObjectInfo::Free()
{
	super::Free();
}
