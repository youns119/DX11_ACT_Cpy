#include "Engine_Defines.h"

namespace Engine
{
	std::wstring CUtils::ConvertToWideChar(const std::string& strMultiByte)
	{
		const char* pSour = strMultiByte.c_str();
		int iSourLen = (int)strMultiByte.size();
		wchar_t szDest[MAX_PATH] {};
		MultiByteToWideChar(CP_ACP, 0, pSour, iSourLen, szDest, MAX_PATH);
		return wstring(szDest);
	}
	std::string CUtils::ConvertToMultibyteChar(const std::wstring& strWide)
	{
		const wchar_t* pSour = strWide.c_str();
		int iSourLen = (int)strWide.size();
		char szDest[MAX_PATH] {};
		WideCharToMultiByte(CP_ACP, 0, pSour, iSourLen, szDest, MAX_PATH, nullptr, nullptr);
		return string(szDest);
	}
	std::wstring CUtils::ConvertToWideChar(const std::string& strMultiByte, size_t iLength)
	{
		const char* pSour = strMultiByte.c_str();
		int iSourLen = (int)strMultiByte.size();
		wchar_t* pDest = new wchar_t[iLength+1];
		MultiByteToWideChar(CP_ACP, 0, pSour, iSourLen, pDest, (int)iLength + 1);
		wstring strConverted = wstring(pDest);
		Safe_Delete_Array(pDest);
		return strConverted;
	}
	std::string CUtils::ConvertToMultibyteChar(const std::wstring& strWide, size_t iLength)
	{
		const wchar_t* pSour = strWide.c_str();
		int iSourLen = (int)strWide.size();
		vector<char> Dest;
		Dest.resize(iLength+1);
		WideCharToMultiByte(CP_ACP, 0, pSour, iSourLen, Dest.data(), (int)iLength+1, nullptr, nullptr);

		//char* pDest = new char[iLength];
		//WideCharToMultiByte(CP_ACP, 0, pSour, iSourLen, pDest, iLength, nullptr, nullptr);
		//string strConverted = string(pDest);
		//Safe_Delete(pDest);
		return string(Dest.data());
	}
	float CUtils::Compute_Random_Normal()
	{
		static random_device rd;
		static mt19937 gen(rd());
		static uniform_real_distribution<float> dis(0.f, 1.f);

		return dis(gen);
	}
	float CUtils::Compute_Random(const float fMin, const float fMax)
	{
		assert(fMin <= fMax);

		return fMin + Compute_Random_Normal() * (fMax - fMin);
	}
	int CUtils::Compute_Random(int iMin, int iMax)
	{
		static random_device rd;
		static mt19937 gen(rd());
		uniform_int_distribution<> dis(iMin, iMax);

		return dis(gen);
	}
	float CUtils::Lerp_Float(float fSrc, float fDst, float fRatio)
	{
		return (1 - fRatio) * fSrc + fRatio * fDst;
	}
	_vector CUtils::Lerp_Parabolic(const _fvector vSrc, const _gvector vDst, _float fHeight, _float fRatio)
	{
		_vec3 vPoint = XMVectorLerp(vSrc, vDst, fRatio);
		vPoint.y += 4 * fHeight * fRatio * (1.f - fRatio);

		return vPoint._vector();
	}
	float CUtils::Clamp(float fSr, float fMin, float fMax)
	{
		return max(fMin, min(fSr, fMax));
	}
	bool CUtils::Is_FileExist(const std::wstring& strFilePath)
	{
		return GetFileAttributes(strFilePath.c_str()) != INVALID_FILE_ATTRIBUTES;
	}
	bool CUtils::Is_FileExist(const std::string& strFilePath)
	{
		return GetFileAttributesA(strFilePath.c_str()) != INVALID_FILE_ATTRIBUTES;
	}
	bool CUtils::Is_Left(_vec3 vLook, _vec3 vPosition, _vec3 vTargetPos)
	{
		_vec3 vDir = vPosition - vTargetPos;

		return 0.f > _vec3::Cross(vLook, vDir).y;
	}
	bool CUtils::Is_Left(_fvector vSrcVector, _gvector vDstVector)
	{
		// 위의 Is_Left 함수의 다른 버전
		// 주로 캐릭터의 Look 방향벡터와 원하는 방향벡터 사이를 따지기 위해 사용
		_vector vCross = XMVector3Cross(vSrcVector, vDstVector);
		_float fDot = XMVectorGetX(XMVector3Dot(vCross, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

		return fDot < 0.0f;
	}
	_float CUtils::GetBetweenAngle(_fvector vSrcVector, _gvector vDstVector)
	{
		// 두 벡터 사이의 각도 계산 (-180.f ~ 180.f 사이의 Degree 값으로 결과 반환)
		_bool bIsLeft = Is_Left(vSrcVector, vDstVector);

		_vector vNorm1 = XMVector3Normalize(vSrcVector);
		_vector vNorm2 = XMVector3Normalize(vDstVector);

		_float fDot = XMVectorGetX(XMVector3Dot(vNorm1, vNorm2));
		fDot = max(-1.0f, min(1.0f, fDot));

		_float fDegree = XMConvertToDegrees(acosf(fDot));

		if (bIsLeft) fDegree = -fDegree;

		return fDegree;
	}
	void CUtils::ValidateFileName(string& strFileName)
	{
		// 파일 이름이 유효한지 확인
		// strInvalid 에 있는 문자들이 들어있으면 다 '_' 문자로 바꿔준다
		string strInvalid = "\\/:*?\"<>| ";
		size_t iPos = 0;

		while ((iPos = strFileName.find_first_of(strInvalid, iPos)) != string::npos)
		{
			strFileName.replace(iPos, 1, "_");
			iPos++;
		}
	}
	_bool CUtils::CreateDirectories(const string& strDirectory)
	{
		// 원하는 위치에 폴더를 생성
		size_t Pos {};
		string strPath;

		while ((Pos = strDirectory.find_first_of("\\/", Pos)) != string::npos)
		{
			strPath = strDirectory.substr(0, Pos++);

			if (!strPath.empty() && GetFileAttributesA(strPath.c_str()) == INVALID_FILE_ATTRIBUTES)
				if (!CreateDirectoryA(strPath.c_str(), NULL))
					if (GetLastError() != ERROR_ALREADY_EXISTS)
						return false;
		}

		if (!strDirectory.empty() && GetFileAttributesA(strDirectory.c_str()) == INVALID_FILE_ATTRIBUTES)
			if (!CreateDirectoryA(strDirectory.c_str(), NULL))
				if (GetLastError() != ERROR_ALREADY_EXISTS)
					return false;

		return true;
	}
	void CUtils::TraverseDirectory(const _wstring& strDirectory, vector<_wstring>& vecFilePath)
	{
		// 원하는 주소의 폴더 이하의 모든 파일들을 탐색해서 벡터에 넣어준다
		WIN32_FIND_DATA tFindData {};
		HANDLE hFind = FindFirstFile((strDirectory + L"/*").c_str(), &tFindData);

		if (hFind == INVALID_HANDLE_VALUE)
			return;
		do
		{
			const _wstring strItemName = tFindData.cFileName;

			if (strItemName == L"." || strItemName == L"..")
				continue;

			_wstring strFullPath = strDirectory + L"/" + strItemName;

			if (tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				TraverseDirectory(strFullPath, vecFilePath);
			else
				vecFilePath.push_back(strFullPath);
		} while (FindNextFile(hFind, &tFindData) != 0);

		FindClose(hFind);
	}
	void CUtils::TraverseDirectory_Ext(const _wstring& strDirectory, vector<_wstring>& vecFilePath, const vector<_wstring>& vecExt)
	{
		// 위의 TraverseDirectory의 예외 설정 버전
		// 특정 확장자의 파일은 탐색하더라도 벡터에 넣고싶지는 않을 경우 쓴다
		WIN32_FIND_DATA tFindData {};
		HANDLE hFind = FindFirstFile((strDirectory + L"/*").c_str(), &tFindData);

		if (hFind == INVALID_HANDLE_VALUE)
			return;
		do
		{
			const _wstring strItemName = tFindData.cFileName;

			if (strItemName == L"." || strItemName == L"..")
				continue;

			_wstring strFullPath = strDirectory + L"/" + strItemName;

			if (tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				TraverseDirectory_Ext(strFullPath, vecFilePath, vecExt);
			}
			else
			{
				for (const auto& Ext : vecExt)
				{
					if (ExtCheck(strFullPath, Ext))
					{
						vecFilePath.push_back(strFullPath);
						break;
					}
				}
			}
		} while (FindNextFile(hFind, &tFindData) != 0);

		FindClose(hFind);
	}
	_bool CUtils::ExtCheck(const _wstring& strPath, const _wstring& strExt)
	{
		// 위의 TraverseDirectory_Ext에서 예외를 검사하기 위해 사용
		if (strPath.length() < strExt.length())
			return false;

		return strPath.compare(strPath.length() - strExt.length(), strExt.length(), strExt) == 0;
	}
	_vector CUtils::Get_MatrixScale(_fmatrix matSrc)
	{
		_vector vScaleX = matSrc.r[0];
		_vector vScaleY = matSrc.r[1];
		_vector vScaleZ = matSrc.r[2];

		return XMVectorSet
		(
			XMVectorGetX(XMVector3Length(vScaleX)),
			XMVectorGetX(XMVector3Length(vScaleY)),
			XMVectorGetX(XMVector3Length(vScaleZ)),
			1.0f
		);
	}
	_vector CUtils::Get_MatrixQuaternion(_fmatrix matSrc)
	{
		_vector vScale = Get_MatrixScale(matSrc);

		_matrix matRotation = matSrc;
		matRotation.r[0] = XMVectorDivide(matSrc.r[0], XMVectorSplatX(vScale));
		matRotation.r[1] = XMVectorDivide(matSrc.r[1], XMVectorSplatY(vScale));
		matRotation.r[2] = XMVectorDivide(matSrc.r[2], XMVectorSplatZ(vScale));

		return XMQuaternionRotationMatrix(matRotation);
	}

	void CUtils::Split_Path(const _string& strFilePath, _string& strDrive, _string& strDirectory, _string& strFileName, _string& strEXT)
	{
		_char		szDrive[MAX_PATH] = "";
		_char		szDirectory[MAX_PATH] = "";
		_char		szFileName[MAX_PATH] = "";
		_char		szEXT[MAX_PATH] = "";

		_splitpath_s(strFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);

		strDrive = szDrive;
		strDirectory = szDirectory;
		strFileName = szFileName;
		strEXT = szEXT;
	}
	void CUtils::Split_Path(const _wstring& strFilePath, _wstring& strDrive, _wstring& strDirectory, _wstring& strFileName, _wstring& strEXT)
	{
		_tchar		szDrive[MAX_PATH] = L"";
		_tchar		szDirectory[MAX_PATH] = L"";
		_tchar		szFileName[MAX_PATH] = L"";
		_tchar		szEXT[MAX_PATH] = L"";

		_wsplitpath_s(strFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);

		strDrive = szDrive;
		strDirectory = szDirectory;
		strFileName = szFileName;
		strEXT = szEXT;
	}
	_string CUtils::Change_EXT(const _string& strFilePath, const _string& strEXT)
	{
		size_t iDot = strFilePath.find_first_of('.');
		return strFilePath.substr(0, iDot) + strEXT;
	}
	_float2 CUtils::Get_WorldToScreenPos(_fvector vPos, _fmatrix matView, _cmatrix matProj, _float2 vScreenSize)
	{
		_matrix matVP = XMMatrixMultiply(matView, matProj);

		_vector vClipPos = XMVector3TransformCoord(vPos, matVP);

		_float fNdcX = _vec3(vClipPos).x;
		_float fNdcY = _vec3(vClipPos).y;
		_float fNdcZ = _vec3(vClipPos).z;

		_float2 vScreen = _float2((fNdcX + 1.f) * 0.5f * vScreenSize.x, (1.f - fNdcY) * 0.5f * vScreenSize.y);

		return vScreen;
	}
}