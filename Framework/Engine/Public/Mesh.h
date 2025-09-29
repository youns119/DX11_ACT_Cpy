#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CModel;
class CBone;

class CMesh final : public CVIBuffer
{
	using super = CVIBuffer;

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<CBone*>& Bones);
	HRESULT Initialize_Prototype_From_File(CModel* pModel, const HANDLE& hFile, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_InputAssembler() override;
	virtual HRESULT Render() override;


	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);

	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	const vector<_uint>& Get_BoneIndices() const { return m_BoneIndices; }
	const vector<VTXANIMMESH>&	Get_VertexData() const { return m_Vertices; }
	const vector<_uint>&		Get_IndexData() const { return m_Indices; }

	_string Get_Name() const { return _string(m_szName); }
	_bool Is_Render() const { return m_isRender; }
	
	void Is_Render(_bool isRender) { m_isRender = isRender; }

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override;

	//HRESULT Save(const HANDLE& hFile, _bool isNonAnim, _fmatrix PreTransformMatrix);
	HRESULT Save(const HANDLE& hFile, CModel* pModel, _fmatrix PreTransformMatrix);

private:
	// _string m_strName;
	_char m_szName[MAX_PATH] {};

	_uint m_iMaterialIndex { 0 };

	_uint m_iNumBones { 0 }; // 이 mesh에 영향을 주는 bone 의 개수

	vector<_uint> m_BoneIndices; // aiBone의 이름으로 찾아낸 model에서 가지고 있는 bone vector에서의 인덱스 

	//_float4x4 m_BoneMatrices[256] {};
	vector<_float4x4> m_BoneMatrices;

	vector<_float4x4> m_OffsetMatrices;

	//vector<VTXMESH>		m_Vertices;
	vector<VTXANIMMESH>		m_Vertices;
	vector<_uint>		m_Indices;

	_bool m_isRender { TRUE };

private:
	HRESULT Ready_VertexBuffer_For_PreTransform(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_NonAnim(CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<CBone*>& Bones);
	HRESULT Ready_VertexBuffer_For_Anim(CModel* pModel, const aiMesh* pAIMesh);

	void Calculate_Normal_From_Vertex();
	void Calculate_Tangent_From_Vertex();

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<CBone*>& Bones);
	static CMesh* Create_From_File(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel* pModel, const HANDLE& hFile, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
