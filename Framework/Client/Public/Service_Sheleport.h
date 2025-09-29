#pragma once

#include "Service_FileData.h"
#include "FData.h"

BEGIN(Client)

class CService_Sheleport : public IService_FileData<FSheleportData>
{
private:
    CService_Sheleport();
    virtual ~CService_Sheleport() = default;

public:
    virtual void Add_Path(const _tchar* szFilePath) { m_vecPath.push_back(szFilePath); }
    virtual HRESULT Load_Data() override;
    virtual const vector<FSheleportData>& Get_Datas(_int iPathIndex) { return m_vecDatas[iPathIndex]; }
    virtual const FSheleportData& Get_Data(_int iPathIndex, _int iDataIndex) { return m_vecDatas[iPathIndex][iDataIndex]; }

private:
    vector<const _tchar*> m_vecPath;
    vector<vector<FSheleportData>> m_vecDatas = {};

public:
    static CService_Sheleport* Create();
    virtual void Free() override;

};

END