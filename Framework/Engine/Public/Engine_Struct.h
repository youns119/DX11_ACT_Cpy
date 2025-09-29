#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	struct ENGINE_DESC
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		unsigned int	iNumLevels;
		bool			isWindowed;
		unsigned int	iViewportWidth;
		uint32_t		iViewportHeight;

	};

	struct LIGHT_DESC
	{
		LIGHT_TYPE eType { LIGHT_TYPE::MAX };

		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;
		XMFLOAT2		vCosCutOff;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT3		vAttenuation;

		static LIGHT_DESC Lerp(const LIGHT_DESC& Src, const LIGHT_DESC& Dst, _float fFactor)
		{
			LIGHT_DESC Desc = Dst;
			
			XMStoreFloat4(&Desc.vDirection, XMVectorLerp(XMLoadFloat4(&Src.vDirection), XMLoadFloat4(&Dst.vDirection), fFactor));
			XMStoreFloat4(&Desc.vDiffuse, XMVectorLerp(XMLoadFloat4(&Src.vDiffuse), XMLoadFloat4(&Dst.vDiffuse), fFactor));
			XMStoreFloat4(&Desc.vAmbient, XMVectorLerp(XMLoadFloat4(&Src.vAmbient), XMLoadFloat4(&Dst.vAmbient), fFactor));

			return Desc;
		}
	};

	struct ENGINE_DLL VTXPOS
	{
		XMFLOAT3	vPosition;

		const static _uint iNumElements = 1;
		const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXPOINT
	{
		XMFLOAT3	vPosition;
		float	fPSize;

		const static _uint iNumElements = 2;
		const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXPOSTEX
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexCoord;

		const static _uint iNumElements = 2;
		const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXCUBE
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vTexCoord;

		const static unsigned int					iNumElements = 2;
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	};

	struct ENGINE_DLL VTXNORTEX
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexCoord;

		const static _uint iNumElements = 3;
		const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXMESH
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexCoord;
		XMFLOAT3	vTangent;

		const static _uint iNumElements = 4;
		const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXANIMMESH
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexCoord;
		XMFLOAT3	vTangent;

		XMUINT4		vBlendIndex;
		XMFLOAT4	vBlendWeight;

		const static _uint iNumElements = 6;
		const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXTRAIL
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vLifeTime;

		const static _uint iNumElements = 2;
		const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct KEYFRAME
	{
		XMFLOAT3 vScale;
		XMFLOAT4 vRotation;
		XMFLOAT3 vTranslation;

		using _scaleUnit = double;
		_scaleUnit dTrackPosition; // double ? 
	};

	struct ENGINE_DLL VTXINSTANCE
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;

		//constexpr static unsigned int iNumElements = 4;
		//const static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	/* for. Instancing */
	struct ENGINE_DLL VTXPARTICLE
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;

		XMFLOAT2	vLifeTime;
		float		fSpeed;
	};
	
	struct ENGINE_DLL VTXPARTICLE_MESH
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;

		XMFLOAT4    vQuaternion;

		XMFLOAT2	vLifeTime;
		float		fSpeed;
	};

	struct ENGINE_DLL VTXMESH_INSTANCE
	{
		const static unsigned int					iNumElements = 8;
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	};

	struct ENGINE_DLL VTXMESH_PARTICLE
	{
		const static unsigned int					iNumElements = 11;
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	};

	/* 그리기용 정점과 인스턴싱용 정점을 합한 정보 */
	struct ENGINE_DLL VTXPOSTEX_PARTICLE // rect 를 그리지만 particle로 그림(instancing 하겟다는 소리)
	{
		const static unsigned int					iNumElements = 7;
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	};

	struct ENGINE_DLL VTXPOINT_PARTICLE
	{
		const static unsigned int					iNumElements = 8;
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	};

	/* 벡터 연산자 오버라이드 */
	struct ENGINE_DLL _vec2
	{
		_vec2() : x(0.f), y(0.f) {}
		_vec2(float _x, float _y) : x(_x), y(_y) {}
		_vec2(double _x, double _y) : x(static_cast<float>(_x)), y(static_cast<float>(_y)) {}
		_vec2(const XMFLOAT2& rhs) : x(rhs.x), y(rhs.y) {}
		_vec2(const FXMVECTOR& rhs) : x(XMVectorGetX(rhs)), y(XMVectorGetY(rhs)) {}
		_vec2(const _vec2& rhs) = default;
		_vec2(float fScalar) : x(fScalar), y(fScalar) {}
		~_vec2() = default;

		float x, y;

		float LengthSquared() const
		{
			return XMVectorGetX(XMVector4LengthSq(XMVectorSet(x, y, 0, 0.f)));
		}

		float Length() const
		{
			return XMVectorGetX(XMVector2Length(XMVectorSet(x, y, 0.f, 0.f)));
		}

		_vec2 Normalized()const
		{
			XMFLOAT2 vNormalized {};
			XMStoreFloat2(&vNormalized, XMVector2Normalize(XMVectorSet(x, y, 0.f, 0.f)));
			return { vNormalized.x, vNormalized.y };
		}

		static float Dot(const _vec2& vSrc, const _vec2& vDst)
		{
			return XMVectorGetX(XMVector2Dot(XMVectorSet(vSrc.x, vSrc.y, 0.f, 0.f), XMVectorSet(vDst.x, vDst.y, 0.f, 0.f)));
		}

		static _vec2 Max(const _vec2& vSrc, const _vec2& vDst)
		{
			return XMVectorMax(vSrc._vector(), vDst._vector());
		}

		static _vec2 Min(const _vec2& vSrc, const _vec2& vDst)
		{
			return XMVectorMin(vSrc._vector(), vDst._vector());
		}

		static _vec2 Lerp(const _vec2& vSrc, const _vec2& vDst, _float fFactor)
		{
			return XMVectorLerp(vSrc._vector(), vDst._vector(), fFactor);
		}

		_vector _vector()const
		{
			return XMVectorSet(x, y, 0.f, 0.f);
		}

		_vec2 operator+(const _vec2& vSrc)const
		{
			return { x + vSrc.x, y + vSrc.y };
		}
		_vec2 operator-(const _vec2& vSrc) const
		{
			return { x - vSrc.x, y - vSrc.y };
		}
		friend _vec2 operator*(float fScalar, const _vec2& vec)
		{
			return { vec.x * fScalar, vec.y * fScalar };
		}
		friend _vec2 operator*(const _vec2& vec, float fScalar)
		{
			return { vec.x * fScalar, vec.y * fScalar };
		}
		_vec2 operator/(float fScalar) const
		{
			return { x / fScalar, y / fScalar };
		}

		_vec2& operator=(const _vec2& vSrc)
		{
			x = vSrc.x;
			y = vSrc.y;
			return *this;
		}
		_vec2& operator+=(const _vec2& vOther)
		{
			return (*this) = (*this) + vOther;
		}
		_vec2& operator-=(const _vec2& vOther)
		{
			return (*this) = (*this) - vOther;
		}
		_vec2& operator*=(float fScalar)
		{
			return (*this) = (*this) * fScalar;
		}
		_vec2& operator/=(float fScalar)
		{
			return (*this) = (*this) / fScalar;
		}

	};

	struct ENGINE_DLL _vec3
	{
		_vec3() : x(0.f), y(0.f), z(0.f) {}
		_vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		_vec3(double _x, double _y, double _z) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(static_cast<float>(_z)) {}
		_vec3(const XMFLOAT3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
		_vec3(const FXMVECTOR& rhs) : x(XMVectorGetX(rhs)), y(XMVectorGetY(rhs)), z(XMVectorGetZ(rhs)) {}
		_vec3(const _vec3& rhs) = default;
		_vec3(float fScalar) : x(fScalar), y(fScalar), z(fScalar) {}
		_vec3(const struct _vec4& rhs);
		~_vec3() = default;

		float x, y, z;


		float LengthSquared() const
		{
			return XMVectorGetX(XMVector4LengthSq(XMVectorSet(x, y, z, 0.f)));
		}

		float Length()const
		{
			return XMVectorGetX(XMVector3Length(XMVectorSet(x, y, z, 0.f)));
		}

		_vec3 Normalized()const
		{
			XMFLOAT3 vNormalized {};
			XMStoreFloat3(&vNormalized, XMVector3Normalize(XMVectorSet(x, y, z, 0.f)));
			return vNormalized;
		}

		static float Dot(const _vec3& vSrc, const _vec3& vDst)
		{
			return XMVectorGetX(XMVector3Dot(XMVectorSet(vSrc.x, vSrc.y, vSrc.z, 0.f), XMVectorSet(vDst.x, vDst.y, vDst.z, 0.f)));
		}

		static _vec3 Cross(const _vec3& vSrc, const _vec3& vDst)
		{
			XMFLOAT3 vCross {};
			XMStoreFloat3(&vCross, XMVector3Cross(XMVectorSet(vSrc.x, vSrc.y, vSrc.z, 0.f), XMVectorSet(vDst.x, vDst.y, vDst.z, 0.f)));
			return vCross;
		}

		static _vec3 Max(const _vec3& vSrc, const _vec3& vDst)
		{
			return XMVectorMax(vSrc._vector(), vDst._vector());
		}

		static _vec3 Min(const _vec3& vSrc, const _vec3& vDst)
		{
			return XMVectorMin(vSrc._vector(), vDst._vector());
		}

		static _vec3 Lerp(const _vec3& vSrc, const _vec3& vDst, _float fFactor)
		{
			return XMVectorLerp(vSrc._vector(), vDst._vector(), fFactor);
		}

		_vec3 TransformCoord(struct _mat TransformMatrix);
		_vec3 TransformNormal(struct _mat TransformMatrix);

		_vector _vector()const
		{
			return XMVectorSet(x, y, z, 0.f);
		}
		XMFLOAT3 _float3()const
		{
			return XMFLOAT3(x, y, z);
		}

		_vec3 operator+(const _vec3& vSrc)const
		{
			return { x + vSrc.x, y + vSrc.y, z + vSrc.z };
		}
		_vec3 operator-(const _vec3& vSrc)const
		{
			return { x - vSrc.x, y - vSrc.y, z - vSrc.z };
		}
		friend _vec3 operator*(float fScalar, const _vec3& vec)
		{
			return { vec.x * fScalar, vec.y * fScalar, vec.z * fScalar };
		}
		friend _vec3 operator*(const _vec3& vec, float fScalar)
		{
			return { vec.x * fScalar, vec.y * fScalar, vec.z * fScalar };
		}
		_vec3 operator/(float fScalar)const
		{
			return { x / fScalar, y / fScalar, z / fScalar };
		}

		_vec3& operator=(const _vec3& vSrc)
		{
			x = vSrc.x;
			y = vSrc.y;
			z = vSrc.z;
			return *this;
		}
		_vec3& operator+=(const _vec3& vOther)
		{
			return (*this) = (*this) + vOther;
		}
		_vec3& operator-=(const _vec3& vOther)
		{
			return (*this) = (*this) - vOther;
		}
		_vec3& operator*=(float fScalar)
		{
			return (*this) = (*this) * fScalar;
		}
		_vec3& operator/=(float fScalar)
		{
			return (*this) = (*this) / fScalar;
		}
	};

	struct ENGINE_DLL _vec4
	{
		_vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
		_vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		_vec4(double _x, double _y, double _z, double _w) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(static_cast<float>(_z)), w(static_cast<float>(_w)) {}
		_vec4(const XMFLOAT4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
		_vec4(const FXMVECTOR& rhs) : x(XMVectorGetX(rhs)), y(XMVectorGetY(rhs)), z(XMVectorGetZ(rhs)), w(XMVectorGetW(rhs)) {}
		_vec4(const _vec4& rhs) = default;
		_vec4(float fScalar) : x(fScalar), y(fScalar), z(fScalar), w(fScalar) {}
		_vec4(const _vec3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(0.f) {}
		_vec4(const _vec3& rhs, _float _w) : x(rhs.x), y(rhs.y), z(rhs.z), w(_w) {}
		~_vec4() = default;

		float x, y, z, w;


		float LengthSquared() const
		{
			return XMVectorGetX(XMVector4LengthSq(XMVectorSet(x, y, z, w)));
		}

		float Length() const
		{
			return XMVectorGetX(XMVector4Length(XMVectorSet(x, y, z, w)));
		}

		_vec4 Normalized() const
		{
			XMFLOAT4 vNormalized {};
			XMStoreFloat4(&vNormalized, XMVector4Normalize(XMVectorSet(x, y, z, w)));
			return vNormalized;
		}

		static float Dot(const _vec4& vSrc, const _vec4& vDst)
		{
			return XMVectorGetX(XMVector4Dot(XMVectorSet(vSrc.x, vSrc.y, vSrc.z, vSrc.w), XMVectorSet(vDst.x, vDst.y, vDst.z, vDst.w)));
		}

		static _vec4 Max(const _vec4& vSrc, const _vec4& vDst)
		{
			return XMVectorMax(vSrc._vector(), vDst._vector());
		}

		static _vec4 Min(const _vec4& vSrc, const _vec4& vDst)
		{
			return XMVectorMin(vSrc._vector(), vDst._vector());
		}

		static _vec4 Lerp(const _vec4& vSrc, const _vec4& vDst, _float fFactor)
		{
			return XMVectorLerp(vSrc._vector(), vDst._vector(), fFactor);
		}


		_vec4 TransformCoord(struct _mat TransformMatrix);
		_vec4 TransformNormal(struct _mat TransformMatrix);
		_vec4 Transform(struct _mat TranformMatrix);

		XMVECTOR _vector() const
		{
			return XMVectorSet(x, y, z, w);
		}
		XMFLOAT4 _float4() const
		{
			return XMFLOAT4(x, y, z, w);
		}

		_vec4 operator+(const _vec4& vSrc)
		{
			return { x + vSrc.x, y + vSrc.y, z + vSrc.z, w + vSrc.w };
		}
		_vec4 operator-(const _vec4& vSrc)
		{
			return { x - vSrc.x, y - vSrc.y, z - vSrc.z, w - vSrc.w };
		}
		friend _vec4 operator*(float fScalar, const _vec4& vec)
		{
			return { vec.x * fScalar, vec.y * fScalar, vec.z * fScalar, vec.w * fScalar };
		}
		friend _vec4 operator*(const _vec4& vec, float fScalar)
		{
			return { vec.x * fScalar, vec.y * fScalar, vec.z * fScalar, vec.w * fScalar };
		}
		_vec4 operator/(float fScalar)
		{
			return { x / fScalar, y / fScalar, z / fScalar, w / fScalar };
		}

		_vec4& operator=(const _vec4& vSrc)
		{
			x = vSrc.x;
			y = vSrc.y;
			z = vSrc.z;
			w = vSrc.w;
			return *this;
		}
		_vec4& operator+=(const _vec4& vOther)
		{
			return (*this) = (*this) + vOther;
		}
		_vec4& operator-=(const _vec4& vOther)
		{
			return (*this) = (*this) - vOther;
		}
		_vec4& operator*=(float fScalar)
		{
			return (*this) = (*this) * fScalar;
		}
		_vec4& operator/=(float fScalar)
		{
			return (*this) = (*this) / fScalar;
		}
	};

	struct ENGINE_DLL _mat
	{
		_mat() : r {} { r[0].x = 1.f; r[1].y = 1.f; r[2].z = 1.f; r[3].w = 1.f; }
		_mat(const _vec4& vRight, const _vec4& vUp, const _vec4& vLook, const _vec4& vPosition) : r { vRight, vUp, vLook, vPosition } {}
		_mat(const XMFLOAT4X4& rhs)
		{
			memcpy(&r[0], rhs.m[0], sizeof(float) * 4);
			memcpy(&r[1], rhs.m[1], sizeof(float) * 4);
			memcpy(&r[2], rhs.m[2], sizeof(float) * 4);
			memcpy(&r[3], rhs.m[3], sizeof(float) * 4);
		}
		_mat(const FXMMATRIX& rhs) : right(rhs.r[0]), up(rhs.r[1]), look(rhs.r[2]), position(rhs.r[3]) {}
		_mat(const _mat& rhs) = default;
		~_mat() = default;

		union
		{
			_vec4 r[4];
			struct
			{
				_vec4 right;
				_vec4 up;
				_vec4 look;
				_vec4 position;
			};
		};

		XMMATRIX _matrix() const
		{
			return XMMATRIX(right._vector(), up._vector(), look._vector(), position._vector());
		}

		XMFLOAT4X4 _float4x4() const
		{
			XMFLOAT4X4 Matrix;
			XMStoreFloat4x4(&Matrix, this->_matrix());
			return Matrix;
		}

		void _float4x4(XMFLOAT4X4& Out) const
		{
			XMStoreFloat4x4(&Out, this->_matrix());
		}

		_mat Inverse() const
		{
			return XMMatrixInverse(nullptr, this->_matrix());
		}

		_bool Decompose(_vec3& vOutScale, _vec4& vOutQuat, _vec3& vOutPos) const
		{
			_vector vScale, vQuat, vPos;
			_bool bResult;
			bResult = XMMatrixDecompose(&vScale, &vQuat, &vPos, this->_matrix());

			if (true == bResult)
			{
				vOutScale = vScale;
				vOutQuat = vQuat;
				vOutPos = vPos;
			}

			return bResult;
		}

		static _mat TranslationMatrix(const _vec3& vPos)
		{
			return XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
		}

		static _mat RotationMatrix(const _vec3& vRotation)
		{
			return XMMatrixRotationRollPitchYaw(vRotation.x, vRotation.y, vRotation.z);
		}

		static _mat RotationMatrix(const _vec4& vQuaternion)
		{
			return XMMatrixRotationQuaternion(vQuaternion._vector());
		}

		static _mat ScalingMatrix(const _vec3& vScale)
		{
			return XMMatrixScaling(vScale.x, vScale.y, vScale.z);
		}

		_mat operator*(_mat Src) const
		{
			//XMMATRIX(right._vector(), up._vector(), look._vector(), position._vector()) *
			//	XMMATRIX(Src.right._vector(), Src.up._vector(), Src.look._vector(), Src.position._vector());
			return this->_matrix() * Src._matrix();
		}
		_mat operator=(_mat Src)
		{
			right = Src.right;
			up = Src.up;
			look = Src.look;
			position = Src.position;
			return *this;
		}
	};

}
#endif // Engine_Struct_h__
