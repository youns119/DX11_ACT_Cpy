#pragma once

#include "Service_FileData.h"
#include "FData.h"

BEGIN(Client)

class CService_Item : public IService_FileData<FItemData>
{
private:
    CService_Item();
    virtual ~CService_Item() = default;

public:
    virtual void Add_Path(const _tchar* szFilePath) { m_vecPath.push_back(szFilePath); }
    virtual HRESULT Load_Data() override;
    virtual const vector<FItemData>& Get_Datas(_int iPathIndex) { return m_vecDatas[iPathIndex]; }
    virtual const FItemData& Get_Data(_int iPathIndex, _int iDataIndex) { return m_vecDatas[iPathIndex][iDataIndex]; }

private:
    vector<const _tchar*> m_vecPath;
    vector<vector<FItemData>> m_vecDatas = {};

public:
    static CService_Item* Create();
    virtual void Free() override;

};

END