#pragma once

#include "Service_FileData.h"
#include "FData.h"

BEGIN(Client)

class CService_Shell : public IService_FileData<FShellDescData>
{
private:
    CService_Shell();
    virtual ~CService_Shell() = default;

public:
    virtual void Add_Path(const _tchar* szFilePath) { m_vecPath.push_back(szFilePath); }
    virtual HRESULT Load_Data() override;
    virtual const vector<FShellDescData>& Get_Datas(_int iPathIndex) { return m_vecDatas[iPathIndex]; }
    virtual const FShellDescData& Get_Data(_int iPathIndex, _int iDataIndex) { return m_vecDatas[iPathIndex][iDataIndex]; }

private:
    vector<const _tchar*> m_vecPath;
    vector<vector<FShellDescData>> m_vecDatas = {};

public:
    static CService_Shell* Create();
    virtual void Free() override;

};

END