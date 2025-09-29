#pragma once

#include "Service_FileData.h"
#include "FData.h"

BEGIN(Client)

class CService_Dialogue : public IService_FileData<FDialogueData>
{
private:
    CService_Dialogue();
    virtual ~CService_Dialogue() = default;

public:
    virtual void Add_Path(const _tchar* szFilePath) { m_vecPath.push_back(szFilePath); }
    virtual HRESULT Load_Data() override;
    virtual const vector<FDialogueData>& Get_Datas(_int iPathIndex) { return m_vecDatas[iPathIndex]; }
    virtual const FDialogueData& Get_Data(_int iPathIndex, _int iDataIndex) { return m_vecDatas[iPathIndex][iDataIndex]; }

private:
    vector<const _tchar*> m_vecPath;
    vector<vector<FDialogueData>> m_vecDatas = {};

public:
    static CService_Dialogue* Create();
    virtual void Free() override;
};

END