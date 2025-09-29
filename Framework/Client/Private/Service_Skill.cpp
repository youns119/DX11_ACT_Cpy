#include "pch.h"
#include "Service_Skill.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <locale>

CService_Skill::CService_Skill()
{
}

HRESULT CService_Skill::Load_Data()
{
    for (_int i = 0; i < m_vecPath.size(); i++)
    {
        const _tchar* szFilePath = m_vecPath[i];

        vector<FSkillData> vecData;

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

            FSkillData data;

            std::wistringstream iss(wLine);
            _wstring strSkillName;
            _wstring strDescription;

            // 텍스쳐 번호
            iss >> data.iSkillNum;

            // 이름
            iss >> strSkillName;
            wcscpy_s(data.szSkillName, strSkillName.c_str());

            // 설명
            std::getline(iss, strDescription);

            if (!strDescription.empty() && strDescription[0] == L' ')
                strDescription.erase(0, 1);

            wcscpy_s(data.szSkillDescription, strDescription.c_str());

            vecData.push_back(data);
        }

        m_vecDatas.push_back(vecData);

        file.close();
    }
 
    return S_OK;
}

CService_Skill* CService_Skill::Create()
{
    return new CService_Skill();
}

void CService_Skill::Free()
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
