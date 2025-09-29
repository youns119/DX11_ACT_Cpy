#include "pch.h"
#include "FileData_Locator.h"
#include "FData.h"

// 정적 멤버 변수 초기화 (템플릿이므로 명시적인 인스턴스화 필요)
template <typename T>
IService_FileData<T>* CFileData_Locator<T>::s_pService = nullptr;

// 사용할 타입들을 명시적으로 인스턴스화
template class CFileData_Locator<FDialogueData>;
template class CFileData_Locator<FItemData>;
template class CFileData_Locator<FSkillData>;
template class CFileData_Locator<FSheleportData>;
template class CFileData_Locator<FShellDescData>;