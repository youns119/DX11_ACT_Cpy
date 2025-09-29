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

        std::ifstream file(szFilePath, std::ios::binary); // ���̳ʸ� ���� ���� ����
        if (!file)
        {
            MSG_BOX("Fail Open Data file");
            return E_FAIL;
        }

        std::string line;
        while (std::getline(file, line)) // �� �پ� �б�
        {
            // UTF-8�� UTF-16���� ��ȯ
            _int wideSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);
            if (wideSize == 0)
                continue; // ��ȯ ���� �� �ش� �� ����

            std::wstring wLine(wideSize, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, &wLine[0], wideSize);

            FSheleportData data;

            std::wistringstream iss(wLine);
            _wstring strLocation;
            _wstring FilePath;
            _wstring strName;

            // ��ȣ
            iss >> data.iSheleportNum;

            // ��ġ
            iss >> strLocation;

            wcscpy_s(data.szLocation, strLocation.c_str());

            // �׸� ���� ���
            iss >> FilePath;
            wcscpy_s(data.szThemeFilePath, FilePath.c_str());

            // �̸�
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
    m_vecPath.shrink_to_fit();  // �Ҵ�� �޸� ��ȯ

    for (auto& vec : m_vecDatas)
    {
        vec.clear();
        vec.shrink_to_fit();
    }

    m_vecDatas.clear();
    m_vecDatas.shrink_to_fit();  // �޸� ��ȯ
}
