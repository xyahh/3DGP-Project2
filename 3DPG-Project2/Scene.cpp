#include "stdafx.h"
#include "Scene.h"

_3DGP_USE_
DX_USE

ID3D12RootSignature* Scene::CreateRootSignature(ID3D12Device* pDevice)
{
	ID3D12RootSignature *pRootSignature = NULL;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;

	::ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.NumParameters = 0;
	RootSignatureDesc.pParameters = NULL;
	RootSignatureDesc.NumStaticSamplers = 0;
	RootSignatureDesc.pStaticSamplers = NULL;
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pSignatureBlob = NULL;
	ID3DBlob* pErrorBlob = NULL;

	ThrowIfFailed(D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignatureBlob, &pErrorBlob));
	ThrowIfFailed(pDevice->CreateRootSignature(0, pSignatureBlob->GetBufferPointer(), 
		pSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pRootSignature));

	if (pSignatureBlob) pSignatureBlob->Release();
	if (pErrorBlob) pErrorBlob->Release();

	return pRootSignature;
}

ID3D12RootSignature* Scene::GetRootSignature() const
{
	return m_RootSignature;
}
