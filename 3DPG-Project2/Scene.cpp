#include "stdafx.h"
#include "Scene.h"

_3DGP_USE_
DX_USE

MWRL ComPtr<ID3D12RootSignature> Scene::CreateRootSignature(ID3D12Device* pDevice)
{
	MWRL ComPtr<ID3D12RootSignature> pRootSignature = NULL;

	D3D12_ROOT_PARAMETER RootParameters[2];

	/* we place the object matrix first since it is the one we are changing most often. */
	RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameters[0].Constants.Num32BitValues = 16;	// one matrix 4x4 = 16 (32 bit values)
	RootParameters[0].Constants.ShaderRegister = 0;		// b 0 -> cbGameObjectInfo
	RootParameters[0].Constants.RegisterSpace = 0;		
	RootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	RootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameters[1].Constants.Num32BitValues = 32;	// two matrix 4x4 = 16 * 2 = 32 (32 bit values)
	RootParameters[1].Constants.ShaderRegister = 1;		// b 1 -> cbCameraInfo
	RootParameters[1].Constants.RegisterSpace = 0;
	RootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlags = // the three matrices (gmtxWorld, gmtxView and gmtxProj will be used in VS only)
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	::ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));

	RootSignatureDesc.NumParameters = _countof(RootParameters);
	RootSignatureDesc.pParameters = RootParameters;
	RootSignatureDesc.NumStaticSamplers = 0;
	RootSignatureDesc.pStaticSamplers = NULL;
	RootSignatureDesc.Flags = RootSignatureFlags;

	MWRL ComPtr<ID3DBlob> pSignatureBlob;
	MWRL ComPtr<ID3DBlob> pErrorBlob;

	ThrowIfFailed(D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignatureBlob, &pErrorBlob));
	ThrowIfFailed(pDevice->CreateRootSignature(0, pSignatureBlob->GetBufferPointer(), 
		pSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature)));

	return pRootSignature;
}

ID3D12RootSignature* Scene::GetRootSignature() const
{
	return m_RootSignature.Get();
}
