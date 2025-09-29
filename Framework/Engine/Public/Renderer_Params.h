#pragma once

namespace Engine
{
	struct FOG_PARAMS
	{
		_vec4 vFogColor { 0.2f, 0.5f, 0.8f, 1.f };
		_vec4 vAbyssalColor { 0.f, 0.f, 0.3f, 1.f };
		_vec2 vRange { 10.f, 300.f };
		_vec2 vAbyssalRange { -1000.f, -10.f };

		static FOG_PARAMS Lerp(const FOG_PARAMS& Src, const FOG_PARAMS& Dst, _float fFactor)
		{
			FOG_PARAMS Params {};
			Params.vFogColor = _vec4::Lerp(Src.vFogColor, Dst.vFogColor, fFactor);
			Params.vAbyssalColor = _vec4::Lerp(Src.vAbyssalColor, Dst.vAbyssalColor, fFactor);
			Params.vRange = _vec2::Lerp(Src.vRange, Dst.vRange, fFactor);
			Params.vAbyssalRange = _vec2::Lerp(Src.vAbyssalRange, Dst.vAbyssalRange, fFactor);

			return Params;
		}
	};

	struct BLOOM_PARAMS
	{
		_float fThreshold { 1.26f };

		static BLOOM_PARAMS Lerp(const BLOOM_PARAMS& Src, const BLOOM_PARAMS& Dst, _float fFactor)
		{
			BLOOM_PARAMS Params {};
			Params.fThreshold = CUtils::Lerp_Float(Src.fThreshold, Dst.fThreshold, fFactor);

			return Params;
		}
	};

	struct COLORCORRECTION_PARAMS
	{
		_float fContrast { 1.f };
		_float fBrightness { 0.f };
		_float fSaturation { 1.f };
		_float fTemperature { 6500.f };
		_float fTint { 0.f };
		_float fExposure { 1.f };
		_float fGamma { 1.f };
		_float IsToneMapping { 1.f };

		static COLORCORRECTION_PARAMS Lerp(const COLORCORRECTION_PARAMS& Src, const COLORCORRECTION_PARAMS& Dst, float fFactor)
		{
			COLORCORRECTION_PARAMS Params {};

			_vec4 SrcUp = _vec4(Src.fContrast, Src.fBrightness, Src.fSaturation, Src.fTemperature);
			_vec4 DstUp = _vec4(Dst.fContrast, Dst.fBrightness, Dst.fSaturation, Dst.fTemperature);
			_vec4 LerpedUp = _vec4::Lerp(SrcUp, DstUp, fFactor);

			_vec4 SrcDown = _vec4(Src.fTint, Src.fExposure, Src.fGamma, Src.IsToneMapping);
			_vec4 DstDown = _vec4(Dst.fTint, Dst.fExposure, Dst.fGamma, Dst.IsToneMapping);
			_vec4 LerpedDown = _vec4::Lerp(SrcDown, DstDown, fFactor);

			Params.fContrast = LerpedUp.x;
			Params.fBrightness = LerpedUp.y;
			Params.fSaturation = LerpedUp.z;
			Params.fTemperature = LerpedUp.w;

			Params.fTint = LerpedDown.x;
			Params.fExposure = LerpedDown.y;
			Params.fGamma = LerpedDown.z;
			Params.IsToneMapping = LerpedDown.w;

			return Params;
		}
	};

	struct SSAO_PARAMS
	{
		_float fRadius { 1.f };
		_float fBias { 0.05f };
	};

	struct RADIALBLUR_PARAMS
	{
		_vec2 vCenter { 640.f, 360.f };
		_float fDensity { 2.f };
		_float fIntensity { 0.f };
		_float fRadius { 0.f };
	};

	struct BILATERALBLUR_PARAMS
	{
		_float fDepthSigma = 0.0001f;
	};

	struct SHADOWMASK_PARAMS
	{
		_vec4 vShadowTexelSize;
		_vec4 vNoiseTexelSize;
		_float fNumFilters;
		_float fRadius = 7.f;
	};
}
