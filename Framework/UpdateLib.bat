

xcopy	/y/s		.\Engine\public\*.*					.\EngineSDK\Inc\

xcopy	/y		.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy	/y		.\Engine\Bin\Enginerelease.dll					.\Client\Bin\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Tool_Animation\Bin\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Tool_Map\Bin\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Tool_UI\Bin\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Tool_Effect\Bin\

xcopy	/y		.\Engine\Bin\Engine.lib					.\EngineSDK\Lib\
xcopy	/y		.\Engine\Bin\Enginerelease.lib					.\EngineSDK\Lib\

xcopy	/y		.\Engine\Bin\ShaderFiles\*.*			.\EngineSDK\Hlsl\

xcopy	/y		.\Engine\ThirdPartyLib\fmod_vc.lib		.\EngineSDK\lib\
xcopy	/y		.\Engine\ThirdPartyLib\fmodL_vc.lib		.\EngineSDK\Lib\

xcopy	/y		.\Engine\ThirdPartyDll\*.dll			.\Client\Bin\
xcopy	/y		.\Engine\ThirdPartyDll\*.dll			.\Tool_Animation\Bin\
xcopy	/y		.\Engine\ThirdPartyDll\*.dll			.\Tool_Map\Bin\
xcopy	/y		.\Engine\ThirdPartyDll\*.dll			.\Tool_UI\Bin\
xcopy	/y		.\Engine\ThirdPartyDll\*.dll			.\Tool_Effect\Bin\


