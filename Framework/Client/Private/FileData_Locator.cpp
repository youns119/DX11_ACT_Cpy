#include "pch.h"
#include "FileData_Locator.h"
#include "FData.h"

// ���� ��� ���� �ʱ�ȭ (���ø��̹Ƿ� ������� �ν��Ͻ�ȭ �ʿ�)
template <typename T>
IService_FileData<T>* CFileData_Locator<T>::s_pService = nullptr;

// ����� Ÿ�Ե��� ��������� �ν��Ͻ�ȭ
template class CFileData_Locator<FDialogueData>;
template class CFileData_Locator<FItemData>;
template class CFileData_Locator<FSkillData>;
template class CFileData_Locator<FSheleportData>;
template class CFileData_Locator<FShellDescData>;