#include "Light_Manager.h"

#include "Light.h"

#if defined(_DEBUG) && defined(PROFILING)
#include "GameInstance.h"
#endif

CLight_Manager::CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

//const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
//{
//    //list<CLight*>::const_iterator iter = m_Lights.begin();
//    auto iter = m_Lights.begin();
//
//    for (_uint i = 0; i < iIndex; ++i)
//        ++iter;
//
//    return (*iter)->Get_LightDesc();
//}

const LIGHT_DESC& CLight_Manager::Get_LightDesc(const _wstring& strLightTag) const
{
    auto iter = m_Lights.find(strLightTag);
    if (iter == m_Lights.end())
    {
        MSG_BOX("Can't find : Light");
        static const LIGHT_DESC EmptyLightDesc {};
        return EmptyLightDesc;
    }
    return iter->second->Get_LightDesc();
}

void CLight_Manager::Set_LightDesc(const _wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	auto iter = m_Lights.find(strLightTag);
    if (iter == m_Lights.end())
    {
        if (FAILED(Add_Light(strLightTag, LightDesc)))
        {
            MSG_BOX("Can't find : Light");
            return;
        }
        iter = m_Lights.find(strLightTag);
    }
    iter->second->Set_LightDesc(LightDesc);
}

_float4 CLight_Manager::Set_LightRotateY(_float4 vDirection, _float fAngle)
{
    _float4 vDir = {};

    _vector vBase = XMLoadFloat4(&vDirection);
    _matrix matRot = XMMatrixRotationY(fAngle);
    _vector vRot = XMVector3TransformNormal(vBase, matRot);
    vRot = XMVector3Normalize(vRot);
    XMStoreFloat4(&vDir, vRot);

    return vDir;
}

HRESULT CLight_Manager::Initialize()
{
    return S_OK;
}

HRESULT CLight_Manager::Add_Light(const _wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
    if( m_Lights.end() != m_Lights.find(strLightTag) )
        return E_FAIL;

    CLight* pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.emplace(strLightTag, pLight);

    return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    PROFILE_SCOPE_TAG;
    for (auto& Pair : m_Lights)
    {
        if (FAILED(Pair.second->Render(pShader, pVIBuffer)))
            return E_FAIL;
    }

    return S_OK;
}

void CLight_Manager::Remove_Light(const _wstring& strLightTag)
{
    auto iter = m_Lights.find(strLightTag);

    if (iter == m_Lights.end())
        return;

    Safe_Release(iter->second);
    m_Lights.erase(iter);
}

void CLight_Manager::Toggle_Light(const _wstring& strLightTag)
{
    auto iter = m_Lights.find(strLightTag);
    iter->second->Toggle();
}

void CLight_Manager::Turn_On(const _wstring& strLightTag)
{
    auto iter = m_Lights.find(strLightTag);
    iter->second->Turn_On();
}

void CLight_Manager::Turn_Off(const _wstring& strLightTag)
{
    auto iter = m_Lights.find(strLightTag);
    iter->second->Turn_Off();
}

void CLight_Manager::Clear_Lights()
{
    for (auto& Pair : m_Lights)
        Safe_Release(Pair.second);
    m_Lights.clear();
}

void CLight_Manager::Save_Light(HANDLE hFile, LIGHT_TYPE eType) 
{
    DWORD dwByte{};

    for (auto& Light : m_Lights)
    {
        if (Light.second->Get_LightDesc().eType == eType)
        {
            _tchar szLightName[MAX_PATH] = L"";
            lstrcpy(szLightName, Light.first.c_str());
            WriteFile(hFile, szLightName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
            WriteFile(hFile, &Light.second->Get_LightDesc(), sizeof(LIGHT_DESC), &dwByte, nullptr);
        }   
    }
}

CLight_Manager* CLight_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLight_Manager* pInstance = new CLight_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CLight_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight_Manager::Free()
{
    super::Free();

    Clear_Lights();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
