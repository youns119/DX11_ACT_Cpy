#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{
	#ifndef			MSG_BOX
	#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
	#endif

	#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
	#define			END						}
	
	#define			USING(NAMESPACE)	using namespace NAMESPACE;

	
	#define			GRAVITY		9.8f
	#define			USE_STRUCTURED_BUFFERS

	
	#ifdef	ENGINE_EXPORTS
	#define ENGINE_DLL		_declspec(dllexport)
	#else
	#define ENGINE_DLL		_declspec(dllimport)
	#endif
	
	#define NO_COPY(CLASSNAME)										\
			private:												\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		
	
	#define DECLARE_SINGLETON(CLASSNAME)							\
			NO_COPY(CLASSNAME)										\
			private:												\
			static CLASSNAME*	m_pInstance;						\
			public:													\
			static CLASSNAME*	GetInstance( void );				\
			static unsigned int DestroyInstance( void );			
	
	#define IMPLEMENT_SINGLETON(CLASSNAME)							\
			CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
			CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
				if(nullptr == m_pInstance) {						\
					m_pInstance = new CLASSNAME;					\
				}													\
				return m_pInstance;									\
			}														\
			unsigned int CLASSNAME::DestroyInstance( void ) {		\
				unsigned int iRefCnt = {};							\
				if(nullptr != m_pInstance)	{						\
					iRefCnt = m_pInstance->Release();				\
					if (0 == iRefCnt)								\
					m_pInstance = nullptr;							\
				}													\
				return iRefCnt;										\
			}

	#define CONCAT_IMPL(x, y) x##y
	#define CONCAT(x, y) CONCAT_IMPL(x, y)

	#if defined(_DEBUG) && defined(PROFILING)
	#define PROFILE_SCOPE(Tag) CScopeTimer CONCAT(_ScopeTimer_, __LINE__) = CGameInstance::GetInstance()->Profile_Scope(Tag)
	#define PROFILE_SCOPE_TAG CScopeTimer CONCAT(_ScopeTimer, __LINE__) = CGameInstance::GetInstance()->Profile_Scope(TEXT(__FUNCTION__));
	#else
	#define PROFILE_SCOPE(Tag) ((void)0)
	#define PROFILE_SCOPE_TAG ((void)0)

#endif
}

#endif // Engine_Macro_h__
