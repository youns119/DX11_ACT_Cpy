#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)

template <typename T>
class IService_FileData
{
public:
    virtual ~IService_FileData() = default;

    virtual void Add_Path(const _tchar* szFilePath) = 0;
    virtual HRESULT Load_Data() = 0;
    virtual const vector<T>& Get_Datas(_int iPathIndex) = 0;
    virtual const T& Get_Data(_int iPathIndex, _int iDataIndex) = 0;
    virtual void Free() = 0;
};

END