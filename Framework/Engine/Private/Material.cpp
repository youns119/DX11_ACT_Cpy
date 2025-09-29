#include "Material.h"

#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const aiMaterial* pAIMaterial, const _string& strModelFilePath)
{
    aiColor4D vBaseColor;
    pAIMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, vBaseColor);
    memcpy(&m_vBaseColor, &vBaseColor, sizeof(_float4));
    AI_MATKEY_METALLIC_FACTOR;
    AI_MATKEY_ROUGHNESS_FACTOR;
    _float fRoughness;
    pAIMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, fRoughness);

    aiColor4D vEmissionColor;
    pAIMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, vEmissionColor);
    memcpy(&m_vEmissionColor, &vEmissionColor, sizeof(_float4));


    for (unsigned int i = 0; i < pAIMaterial->mNumProperties; ++i) {
        const aiMaterialProperty* prop = pAIMaterial->mProperties[i];
        _string log = prop->mKey.C_Str() + _string("\n");
        OutputDebugStringA(log.c_str());
    }

    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
    {
        /* 특정 속성을 표현하는 텍스쳐가 몇장인가? */
        aiTextureType eTextureType = static_cast<aiTextureType>(i);
        _uint iNumTextures = pAIMaterial->GetTextureCount(eTextureType);

        m_MtrlTextures[i].reserve(iNumTextures);

        /* 각각의 텍스처들을 로드한다. */
        for (_uint j = 0; j < iNumTextures; ++j)
        {
            aiString strTextureFilePath;
            ID3D11ShaderResourceView* pSRV = nullptr;

            if (FAILED(pAIMaterial->GetTexture(eTextureType, j, &strTextureFilePath)))
                return E_FAIL;

            _char		szDrive[MAX_PATH] = "";
            _char		szDirectory[MAX_PATH] = "";
            _char		szFileName[MAX_PATH] = "";
            _char		szEXT[MAX_PATH] = "";

            _splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(strTextureFilePath.C_Str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

            _char       szFullPath[MAX_PATH] = "";
            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDirectory);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szEXT);

            _tchar      szWideCharPath[MAX_PATH] = TEXT("");
            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_uint)strlen(szFullPath), szWideCharPath, MAX_PATH);

            HRESULT hr {};

            // By DirectXTex
            if (false == strcmp(szEXT, ".dds"))
            {
                hr = CreateDDSTextureFromFile(m_pDevice, szWideCharPath, nullptr, &pSRV);
            }
            else if (false == strcmp(szEXT, ".tga"))
            {
                hr = E_FAIL;
            }
            else
            {
                ScratchImage Image {};
                ScratchImage MipChain {};
                TexMetadata MetaData {};
                hr = LoadFromWICFile(szWideCharPath, WIC_FLAGS_NONE, &MetaData, Image);
                //hr = LoadFromWICFile(szTextureFilePath, WIC_FLAGS_FORCE_RGB, &MetaData, Image);
                if (FAILED(hr))
                    //continue;
                    return E_FAIL;

                //hr = GenerateMipMaps(Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), TEX_FILTER_DEFAULT, 0, MipChain);
                //if (FAILED(hr))
                //    return E_FAIL;

                //hr = CreateShaderResourceView(m_pDevice, MipChain.GetImages(), MipChain.GetImageCount(), MipChain.GetMetadata(), &pSRV);
                hr = CreateShaderResourceView(m_pDevice, Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), &pSRV);

            }

            if (FAILED(hr))
                return E_FAIL;

            m_MtrlTextures[i].push_back(pSRV);

            PATH_BY_INDEX PathByIndex {};
            {
                PathByIndex.first = (_uint)i;
                lstrcpyW(PathByIndex.second, szWideCharPath);
            }
            m_TextureFilesByType.push_back(PathByIndex);
        }
    }

    Ready_DefaultTexture();

    return S_OK;
}

HRESULT CMaterial::Initialize_From_File(const HANDLE& hFile, const _wstring& strModelFilePath)
{
    // Load(hFile);

    assert(!(INVALID_HANDLE_VALUE == hFile));

    _ulong dwByte = 0;
    _bool bResult = FALSE;

    _uint iSize = 0;

    bResult = ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    m_TextureFilesByType.resize(iSize);
    bResult = ReadFile(hFile, m_TextureFilesByType.data(), sizeof(PATH_BY_INDEX) * iSize, &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_vBaseColor, sizeof(_float4), &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_vEmissionColor, sizeof(_float4), &dwByte, nullptr);



    for (auto& Pair : m_TextureFilesByType)
    {
        _uint iIndex = Pair.first;
        _tchar* szFileName = Pair.second;

        HRESULT hr {};

        ID3D11ShaderResourceView* pSRV = nullptr;

        //_char		szDrive[MAX_PATH] = "";
        //_char		szDirectory[MAX_PATH] = "";
        //_char		szFileName[MAX_PATH] = "";
        _tchar		szEXT[MAX_PATH] = {};

        _wsplitpath_s(szFileName, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);


        // By DirectXTex
        if (false == lstrcmp(szEXT, L".dds"))
        {
            hr = CreateDDSTextureFromFile(m_pDevice, szFileName, nullptr, &pSRV);
        }
        else if (false == lstrcmp(szEXT, L".tga"))
        {
            hr = E_FAIL;
        }
        else
        {
            ScratchImage Image {};
            ScratchImage MipChain {};
            TexMetadata MetaData {};
            hr = LoadFromWICFile(szFileName, WIC_FLAGS_NONE, &MetaData, Image);
            if (FAILED(hr))
                return E_FAIL;

            // 밉맵 만드는데 생각보다 오래 걸리네 ㅋ 개발중에는 끄고 있다가 개발 끝나면 켜자 어짜피 그거니까
            // DDS 로 변환도 할까?

            //hr = GenerateMipMaps(Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), TEX_FILTER_DEFAULT, 0, MipChain);
            //if (FAILED(hr))
            //    return E_FAIL;

            //hr = CreateShaderResourceView(m_pDevice, MipChain.GetImages(), MipChain.GetImageCount(), MipChain.GetMetadata(), &pSRV);
            hr = CreateShaderResourceView(m_pDevice, Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), &pSRV);

        }

        if (FAILED(hr))
            return E_FAIL;


        m_MtrlTextures[iIndex].push_back(pSRV);
    }

    Ready_DefaultTexture();

    return S_OK;
}

HRESULT CMaterial::Bind_Material(CShader* pShader, aiTextureType eType, const _char* pConstantName, _uint iTextureIndex)
{
    if (iTextureIndex >= m_MtrlTextures[eType].size())
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_MtrlTextures[eType][iTextureIndex]);
}

HRESULT CMaterial::Save(const HANDLE& hFile)
{
    // 저장해야할 데이터가 파일의 이름들이네? 

    assert(!(INVALID_HANDLE_VALUE == hFile));

    _ulong dwByte = 0;

    _uint iSize = (_uint)m_TextureFilesByType.size();

    //if (0 == iSize)
    //    return S_OK;

    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    WriteFile(hFile, m_TextureFilesByType.data(), sizeof(PATH_BY_INDEX) * iSize, &dwByte, nullptr);
    WriteFile(hFile, &m_vBaseColor, sizeof(_float4), &dwByte, nullptr);
    WriteFile(hFile, &m_vEmissionColor, sizeof(_float4), &dwByte, nullptr);


    return S_OK;
}

_bool CMaterial::Has_Texture(MATERIAL_TEXTURE eTexture) const
{
    return !m_MtrlTextures[(_uint)eTexture].empty();
}

HRESULT CMaterial::Bind_BaseColor(CShader* pShader, const char* pConstantName) const
{
    return pShader->Bind_RawValue(pConstantName, &m_vBaseColor, sizeof(_float4));
}

HRESULT CMaterial::Bind_EmissionColor(CShader* pShader, const char* pConstantName) const
{
    return pShader->Bind_RawValue(pConstantName, &m_vEmissionColor, sizeof(_float4));
}

void CMaterial::Ready_DefaultTexture()
{
    //if (true == m_MtrlTextures[(_uint)MATERIAL_TEXTURE::DIFFUSE].empty())
    //{
    //    m_MtrlTextures[(_uint)MATERIAL_TEXTURE::DIFFUSE].push_back(Generate_DefaultTexture_With_Color(m_vBaseColor));
    //}

    //Has_Texture(MATERIAL_TEXTURE::NORMAL)
    if (false == Has_Texture(MATERIAL_TEXTURE::NORMAL))
    {
        m_MtrlTextures[(_uint)MATERIAL_TEXTURE::NORMAL].push_back(Generate_DefaultTexture_With_Color(_vec4(1.f, 0.5f, 0.5f, 0.f)));
    }
    if (false == Has_Texture(MATERIAL_TEXTURE::METALNESS))
    {
        m_MtrlTextures[(_uint)MATERIAL_TEXTURE::METALNESS].push_back(Generate_DefaultTexture_With_Color(_vec4(0.0f, 0.0f, 1.0f, 0.f)));
    }
    if (false == Has_Texture(MATERIAL_TEXTURE::SHININESS))
    {
        m_MtrlTextures[(_uint)MATERIAL_TEXTURE::SHININESS].push_back(Generate_DefaultTexture_With_Color(_vec4(0.0f, 0.0f, 1.0f, 0.f)));
    }
    if (false == Has_Texture(MATERIAL_TEXTURE::EMISSIVE))
    {
        m_MtrlTextures[(_uint)MATERIAL_TEXTURE::EMISSIVE].push_back(Generate_DefaultTexture_With_Color(_vec4(0.0f, 0.0f, 1.0f, 0.f)));
    }
}

ID3D11ShaderResourceView* CMaterial::Generate_DefaultTexture_With_Color(const _vec4& vColor)
{
    /* 1x1 texture */

	// Convert color to 8-bit format (RGBA8)
    _uint iColor = 
        (static_cast<_uint>(vColor.w * 255) << 24) |
        (static_cast<_uint>(vColor.z * 255) << 16) |
        (static_cast<_uint>(vColor.y * 255) << 8) |
        (static_cast<_uint>(vColor.x * 255));

    D3D11_TEXTURE2D_DESC TextureDesc = {};
    TextureDesc.Width = 1;
    TextureDesc.Height = 1;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA InitalData = {};
    InitalData.pSysMem = &iColor;
    InitalData.SysMemPitch = sizeof(_uint);

    ID3D11Texture2D* pTexture2D = nullptr;
    HRESULT hr = m_pDevice->CreateTexture2D(&TextureDesc, &InitalData, &pTexture2D);
    if (FAILED(hr))
        return nullptr;

    // Create Shader Resource View
    ID3D11ShaderResourceView* pSRV = nullptr;
    hr = m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &pSRV);
    pTexture2D->Release(); // SRV holds reference, so we release our local pointer

    return pSRV;

}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial, const _string& strModelFilePath)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pAIMaterial, strModelFilePath)))
    {
        MSG_BOX("Failed to Create : CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CMaterial* CMaterial::Create_From_File(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const HANDLE& hFile, const _wstring& strModelFilePath)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize_From_File(hFile, strModelFilePath)))
    {
        MSG_BOX("Failed to Create : CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMaterial::Free()
{
    super::Free();

    for (vectorSRV& SRVs : m_MtrlTextures)
        for (ID3D11ShaderResourceView*& pSRV : SRVs)
            Safe_Release(pSRV);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
