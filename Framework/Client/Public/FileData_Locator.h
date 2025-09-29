#pragma once

#include "Client_Defines.h"
#include "Service_FileData.h"

BEGIN(Client)

template <typename T>
class CFileData_Locator
{
public:
    static void Register_FileData(IService_FileData<T>* pService)
    { 
        Release_Service();
        s_pService = pService; 
    }
    static IService_FileData<T>* Get_FileData() { return s_pService; }
    static void Release_Service()
    { 
        if (s_pService)
        {
            s_pService->Free();
            Safe_Delete(s_pService);
        }
    }

private:
    static IService_FileData<T>* s_pService;
};
END