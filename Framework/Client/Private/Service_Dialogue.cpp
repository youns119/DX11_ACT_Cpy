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
            int wideSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);
            if (wideSize == 0) {
                continue; // ��ȯ ���� �� �ش� �� ����
            }

            std::wstring wLine(wideSize, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, &wLine[0], wideSize);

            FDialogueData data;

            std::wistringstream iss(wLine);
            _wstring strSpeaker;
            _wstring strDialogue;

            // State
            iss >> data.iStateNum;

            // SPEAKER ���� �б�
            iss >> strSpeaker;

            wcscpy_s(data.szSpeaker, strSpeaker.c_str());

            // FaceNum (����) �б�
            iss >> data.iFaceTextureNum;

            // ������ ��� �б�
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
    m_vecPath.shrink_to_fit();  // �Ҵ�� �޸� ��ȯ

    for (auto& vec : m_vecDatas)
    {
        vec.clear();
        vec.shrink_to_fit();
    }

    m_vecDatas.clear();
    m_vecDatas.shrink_to_fit();  // �޸� ��ȯ
}
