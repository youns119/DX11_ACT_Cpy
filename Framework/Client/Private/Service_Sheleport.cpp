#include "pch.h"
#include "Service_Sheleport.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <locale>

CService_Sheleport::CService_Sheleport()
{
}

HRESULT CService_Sheleport::Load_Data()
{
    for (_int i = 0; i < m_vecPath.size(); i++)
    {
        const _tchar* szFilePath = m_vecPath[i];

        vector<FSheleportData> vecData;

        std::ifstream file(szFilePath, std::ios::binary); // 바이너리 모드로 파일 열기
        if (!file)
        {
            MSG_BOX("Fail Open Data file");
            return E_FAIL;
        }

        std::string line;
        while (std::getline(file, line)) // 한 줄씩 읽기
        {
            // UTF-8을 UTF-16으로 변환
            _int wideSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);
            if (wideSize == 0)
                continue; // 변환 실패 시 해당 줄 무시

            std::wstring wLine(wideSize, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, &wLine[0], wideSize);

            FSheleportData data;

            std::wistringstream iss(wLine);
            _wstring strLocation;
            _wstring FilePath;
            _wstring strName;

            // 번호
            iss >> data.iSheleportNum;

            // 위치
            iss >> strLocation;

            wcscpy_s(data.szLocation, strLocation.c_str());

            // 테마 파일 경로
            iss >> FilePath;
            wcscpy_s(data.szThemeFilePath, FilePath.c_str());

            // 이름
            std::getline(iss, strName);

            if (!strName.empty() && strName[0] == L' ')
                strName.erase(0, 1);

            wcscpy_s(data.szName, strName.c_str());

            vecData.push_back(data);
        }

        m_vecDatas.push_back(vecData);

        file.close();
    }
 
    return S_OK;
}

CService_Sheleport* CService_Sheleport::Create()
{
    return new CService_Sheleport();
}

void CService_Sheleport::Free()
{
    m_vecPath.clear();
    m_vecPath.shrink_to_fit();  // 할당된 메모리 반환

    for (auto& vec : m_vecDatas)
    {
        vec.clear();
        vec.shrink_to_fit();
    }

    m_vecDatas.clear();
    m_vecDatas.shrink_to_fit();  // 메모리 반환
}
