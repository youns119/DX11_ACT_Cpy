#pragma once

#include "Service_FileData.h"
#include "FData.h"

BEGIN(Client)

class CService_Skill : public IService_FileData<FSkillData>
{
private:
    CService_Skill();
    virtual ~CService_Skill() = default;

public:
    virtual void Add_Path(const _tchar* szFilePath) { m_vecPath.push_back(szFilePath); }
    virtual HRESULT Load_Data() override;
    virtual const vector<FSkillData>& Get_Datas(_int iPathIndex) { return m_vecDatas[iPathIndex]; }
    virtual const FSkillData& Get_Data(_int iPathIndex, _int iDataIndex) { return m_vecDatas[iPathIndex][iDataIndex]; }

private:
    vector<const _tchar*> m_vecPath;
    vector<vector<FSkillData>> m_vecDatas = {};

public:
    static CService_Skill* Create();
    virtual void Free() override;

};

END