#include "pch.h"
#include "Service_Dialogue.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <locale>

CService_Dialogue::CService_Dialogue()
{
}

HRESULT CService_Dialogue::Load_Data()
{
    for (_int i = 0; i < m_vecPath.size(); i++)
    {
        const _tchar* szFilePath = m_vecPath[i];

        vector<FDialogueData> vecData;

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
            int wideSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);
            if (wideSize == 0) {
                continue; // 변환 실패 시 해당 줄 무시
            }

            std::wstring wLine(wideSize, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, &wLine[0], wideSize);

            FDialogueData data;

            std::wistringstream iss(wLine);
            _wstring strSpeaker;
            _wstring strDialogue;

            // State
            iss >> data.iStateNum;

            // SPEAKER 정보 읽기
            iss >> strSpeaker;

            wcscpy_s(data.szSpeaker, strSpeaker.c_str());

            // FaceNum (숫자) 읽기
            iss >> data.iFaceTextureNum;

            // 나머지 대사 읽기
            std::getline(iss, strDialogue);

            if (!strDialogue.empty() && strDialogue[0] == L' ')
                strDialogue.erase(0, 1);

            wcscpy_s(data.szDialogue, strDialogue.c_str());

            vecData.push_back(data);
        }

        m_vecDatas.push_back(vecData);

        file.close();
    }

    return S_OK;
}

CService_Dialogue* CService_Dialogue::Create()
{
    return new CService_Dialogue();
}

void CService_Dialogue::Free()
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
