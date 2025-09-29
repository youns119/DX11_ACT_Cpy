#include "pch.h"
#include "Dialog.h"

const COMDLG_FILTERSPEC CDialog::s_FileTypes[(unsigned int)FILE_TYPE::MAX] =
{
    {L"Model Files (*.model)", L"*.model"},
    {L"Map Files (*.mapData)", L"*.mapData"},
    {L"Navigation (*.navigation)", L"*.navigation"},
    {L"Interaction Object (*.interaction)", L"*.interaction"},
    {L"Map Object (*.environment)", L"*.environment"},
    {L"MeshInstancing (*.Instancing)", L"*.instancing"},
    {L"NPC (*.npc)", L"*.npc"},
    {L"MapTrigger (*.mapTrigger)", L"*.mapTrigger"},
    {L"Item (*.item)", L"*.item"},
    {L"Monster (*.monster)", L"*.monster"},
    {L"Light (*.light)", L"*.light"},
    {L"Atmosphere (*.atmosphere)", L"*.atmosphere"},
    {L"Bmp (*.bmp)", L"*.bmp"},
};

CDialog::CDialog()
{
}

HRESULT CDialog::Set_FileType(FILE_TYPE eType)
{
    return m_pFileDialog->SetFileTypeIndex((_uint)eType);
}

HRESULT CDialog::Set_DefaultExt(const _tchar* szDefaultExt)
{
    return m_pFileDialog->SetDefaultExtension(szDefaultExt);
}

HRESULT CDialog::Set_DefaultOption(FILEOPENDIALOGOPTIONS eOption)
{
    FILEOPENDIALOGOPTIONS CurrentOption{};

    m_pFileDialog->GetOptions(&CurrentOption);
    CurrentOption |= eOption;

    return m_pFileDialog->SetOptions(CurrentOption);
}

HRESULT CDialog::Show_Dialog()
{
    HRESULT hr{};

    // 파일 열기 대화 상자 표시 // 여기서부터 다이얼로그 열ㄹ림
    hr = m_pFileDialog->Show(nullptr);
    if (FAILED(hr))
    {
        return E_FAIL;
    }

    // 선택한 파일 경로 가져오기
    hr = m_pFileDialog->GetResult(&m_pItem);
    if (FAILED(hr))
    {
        MSG_BOX("Failed to Get result");
        return E_FAIL;
    }

    hr = m_pItem->GetDisplayName(SIGDN_FILESYSPATH, &m_pFilePath);
    if (FAILED(hr))
    {
        MSG_BOX("Failed to Get DisplayName");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CDialog::Initialize(TYPE eType)
{
    HRESULT hr{};
    //HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    //if (FAILED(hr))
    //{
    //    MSG_BOX("File to CoInitialize Ex");
    //    return E_FAIL;
    //}

    switch (eType)
    {
    case TYPE::LOAD:
        // IFileOpenDialog 생성
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pFileDialog));
        if (FAILED(hr))
        {
            MSG_BOX("fail to CoCreateInstance : File Open Dialog");
            return E_FAIL;
        }

        m_pFileDialog->SetFileTypes((_uint)FILE_TYPE::MAX - 1, s_FileTypes);

        // default setting
        m_pFileDialog->GetOptions(&m_DefaultOptions);
        m_DefaultOptions |= FOS_FORCEFILESYSTEM;
        m_DefaultOptions |= FOS_FILEMUSTEXIST;
        m_DefaultOptions |= FOS_PATHMUSTEXIST;
        m_pFileDialog->SetOptions(m_DefaultOptions);
        break;

    case TYPE::SAVE:
        // todo 
        hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pFileDialog));
        if (FAILED(hr))
        {
            MSG_BOX("fail to CoCreateInstance : File Save Dialog");
            return E_FAIL;
        }

        m_pFileDialog->SetFileTypes((_uint)FILE_TYPE::MAX - 1, s_FileTypes);

        // default setting
        m_pFileDialog->GetOptions(&m_DefaultOptions);
        m_DefaultOptions |= FOS_FORCEFILESYSTEM;
        //m_DefaultOptions |= FOS_FILEMUSTEXIST;
        m_DefaultOptions |= FOS_PATHMUSTEXIST;
        m_pFileDialog->SetOptions(m_DefaultOptions);
        break;
    }


    WCHAR szDefaultPath[MAX_PATH];
    if (GetModuleFileName(nullptr, szDefaultPath, MAX_PATH) > 0)
    {
        _tchar szDrive[MAX_PATH]{};
        _tchar szDirectory[MAX_PATH]{};

        _wsplitpath_s(szDefaultPath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

        m_strDefaultPath = _wstring(szDrive) + _wstring(szDirectory);
    }
    else
        return E_FAIL;

    IShellItem* pDefaultFolder = nullptr;
    hr = SHCreateItemFromParsingName(m_strDefaultPath.c_str(), nullptr, IID_PPV_ARGS(&pDefaultFolder));
    if (SUCCEEDED(hr))
    {
        // 대화 상자의 기본 폴더 설정
        m_pFileDialog->SetFolder(pDefaultFolder);
        m_pFileDialog->SetDefaultFolder(pDefaultFolder);
        pDefaultFolder->Release();
    }

    return S_OK;
}

CDialog* CDialog::Create(TYPE eType)
{
    CDialog* pInstance = new CDialog();

    if (FAILED(pInstance->Initialize(eType)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed To Create : CDialog");
    }

    return pInstance;
}

void CDialog::Free()
{
    super::Free();

    CoTaskMemFree(m_pFilePath);

    Safe_Release(m_pItem);
    Safe_Release(m_pFileDialog);

    //CoUninitialize();
}
