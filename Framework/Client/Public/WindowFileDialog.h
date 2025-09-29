#pragma once
#ifdef _DEBUG

#include <shobjidl_core.h>

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CWindowFileDialog final : public CBase
{
    using super = CBase;

public:
    enum class FILE_TYPE
    {
        ALL,
        ATMOSPHERE,

        MAX
    };

    static const COMDLG_FILTERSPEC s_FileTypes[(unsigned int)FILE_TYPE::MAX];

    enum class TYPE
    {
        SAVE,
        LOAD,
    };

private:
    CWindowFileDialog();
    CWindowFileDialog(const CWindowFileDialog& rhs) = delete;
    virtual ~CWindowFileDialog() = default;

public:
    HRESULT Set_FileType(FILE_TYPE eType);
    HRESULT Set_DefaultExt(const _tchar* szDefaultExt);
    HRESULT Set_DefaultOption(FILEOPENDIALOGOPTIONS eOption);

    HRESULT Show_Dialog();
    _tchar* Get_FilePath() { return m_pFilePath; }

private:
    //IFileOpenDialog*   m_pFileOpen { nullptr };
    IFileDialog* m_pFileDialog { nullptr };
    IShellItem* m_pItem { nullptr };
    _tchar* m_pFilePath { nullptr };

    FILEOPENDIALOGOPTIONS m_DefaultOptions {};

    _wstring m_strDefaultPath;


private:
    HRESULT Initialize(TYPE eType);

public:
    static CWindowFileDialog* Create(TYPE eType);
    virtual void Free() override;
};
END
#endif
