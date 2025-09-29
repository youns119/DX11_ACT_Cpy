#ifndef Engine_Function_h__
#define Engine_Function_h__

//#include "Engine_Macro.h"

namespace Engine
{
	// ���ø��� ����� ������������ �ڷ����� ���������� ���� ��
	// ����� �ν��Ͻ�ȭ �ϱ� ���Ͽ� �����δ� Ʋ

	template<typename T>
	void	Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete [] pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int		iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();

			if (0 == iRefCnt)
				pInstance = nullptr;

			// ���۷��� ������ �Ϻ��ϴٸ� �׸��� Release ���Ŀ� �ش� ��ü�� ���̻� ������� �ʴ°��� Ȯ���ϴٸ�
			// ������ ���ڸ��� �ٷ� nullptr �ھƵ� �������� �ʳ�?
		}
		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int		iRefCnt = 0;

		if (nullptr != pInstance)		
			iRefCnt = pInstance->AddRef();

		return iRefCnt;
	}

	class ENGINE_DLL CUtils
	{
	public:
		static std::wstring ConvertToWideChar(const std::string& strMultiByte);
		static std::string ConvertToMultibyteChar(const std::wstring& strWide);
		static std::wstring ConvertToWideChar(const std::string& strMultiByte, size_t iLength);
		static std::string ConvertToMultibyteChar(const std::wstring& strWide, size_t iLength);

		static float Compute_Random_Normal();
		static float Compute_Random(float fMin, float fMax);
		static int Compute_Random(int iMin, int iMax);
		static float Lerp_Float(float fSrc, float fDst, float fRatio);
		static _vector Lerp_Parabolic(const _fvector vSrc, const _gvector vDst, _float fHeight, _float fRatio);
		static float Clamp(float fSr, float fMin, float fMax);

		static bool Is_FileExist(const std::wstring& strFilePath);
		static bool Is_FileExist(const std::string& strFilePath);

		static bool Is_Left(struct _vec3 vLook, struct _vec3 vPosition, struct _vec3 vTargetPos);
		static bool Is_Left(_fvector vSrcVector, _gvector vDstVector);
		static _float GetBetweenAngle(_fvector vSrcVector, _gvector vDstVector);

		static void ValidateFileName(string& strFileName);
		static _bool CreateDirectories(const string& strDirectory);
		static void TraverseDirectory(const _wstring& strDirectory, vector<_wstring>& vecFilePath);
		static void TraverseDirectory_Ext(const _wstring& strDirectory, vector<_wstring>& vecFilePath, const vector<_wstring>& vecExt);
		static _bool ExtCheck(const _wstring& strPath, const _wstring& strExt);

		static _vector Get_MatrixScale(_fmatrix matSrc);
		static _vector Get_MatrixQuaternion(_fmatrix matSrc);

		static void Split_Path(const _string& strFilePath, _string& strDrive, _string& strDirectory, _string& strFileName, _string& strEXT);
		static void Split_Path(const _wstring& strFilePath, _wstring& strDrive, _wstring& strDirectory, _wstring& strFileName, _wstring& strEXT);
		static _string Change_EXT(const _string& strFilePath, const _string& strEXT);

		static _float2 Get_WorldToScreenPos(_fvector vPos, _fmatrix matView, _cmatrix matProj, _float2 vScreenSize);
	};
}

#endif // Engine_Function_h__
