#pragma once
#include <shobjidl_core.h>

#include "Tool_Define.h"
#include "Base.h"


BEGIN(MapTool)

class CDialog final : public CBase
{
    using super = CBase;

public:
    enum class FILE_TYPE
    {
        ALL,
        MDOEL,
        MAP,
        NAVIGATION,
        INTERACTION_OBJ,
        NON_INTERACTION_OBJ,
        INSTANCING,
        NPC,
        MAP_TRIGGER,
        ITEM,
        MONSTER,
        LIGHT,
        ATMOSPHERE,
        BMP,
        MAX
    };

    static const COMDLG_FILTERSPEC s_FileTypes[(unsigned int)FILE_TYPE::MAX];

    enum class TYPE
    {
        SAVE,
        LOAD,
    };

private:
    CDialog();
    CDialog(const CDialog& rhs) = delete;
    virtual ~CDialog() = default;

public:
    HRESULT Set_FileType(FILE_TYPE eType);
    HRESULT Set_DefaultExt(const _tchar* szDefaultExt);
    HRESULT Set_DefaultOption(FILEOPENDIALOGOPTIONS eOption);

    HRESULT Show_Dialog();
    _tchar* Get_FilePath() { return m_pFilePath; }

private:
    //IFileOpenDialog*   m_pFileOpen { nullptr };
    IFileDialog* m_pFileDialog{ nullptr };
    IShellItem* m_pItem{ nullptr };
    _tchar* m_pFilePath{ nullptr };

    FILEOPENDIALOGOPTIONS m_DefaultOptions{};

    _wstring m_strDefaultPath;


private:
    HRESULT Initialize(TYPE eType);

public:
    static CDialog* Create(TYPE eType);
    virtual void Free() override;
};

END


/*이건 옵션값인데 결국 안썻대요
enum _FILEOPENDIALOGOPTIONS
   {
      FOS_OVERWRITEPROMPT   = 0x2,
      FOS_STRICTFILETYPES   = 0x4,
      FOS_NOCHANGEDIR   = 0x8,
      FOS_PICKFOLDERS   = 0x20,
      FOS_FORCEFILESYSTEM   = 0x40,
      FOS_ALLNONSTORAGEITEMS   = 0x80,
      FOS_NOVALIDATE   = 0x100,
      FOS_ALLOWMULTISELECT   = 0x200,
      FOS_PATHMUSTEXIST   = 0x800,
      FOS_FILEMUSTEXIST   = 0x1000,
      FOS_CREATEPROMPT   = 0x2000,
      FOS_SHAREAWARE   = 0x4000,
      FOS_NOREADONLYRETURN   = 0x8000,
      FOS_NOTESTFILECREATE   = 0x10000,
      FOS_HIDEMRUPLACES   = 0x20000,
      FOS_HIDEPINNEDPLACES   = 0x40000,
      FOS_NODEREFERENCELINKS   = 0x100000,
      FOS_OKBUTTONNEEDSINTERACTION   = 0x200000,
      FOS_DONTADDTORECENT   = 0x2000000,
      FOS_FORCESHOWHIDDEN   = 0x10000000,
      FOS_DEFAULTNOMINIMODE   = 0x20000000,
      FOS_FORCEPREVIEWPANEON   = 0x40000000,
      FOS_SUPPORTSTREAMABLEITEMS   = 0x80000000
   } ;
typedef DWORD FILEOPENDIALOGOPTIONS;
*/