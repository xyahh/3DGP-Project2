#include "stdafx.h"
#include "Scene.h"

_3DGP_USE_

WRL ComPtr<ID3D12RootSignature> Scene::CreateRootSignature(ID3D12Device* pDevice)
{
	WRL ComPtr<ID3D12RootSignature> pRootSignature;
	
	D3D12_ROOT_SIGNATURE_DESC RootDesc;
	::ZeroMemory(&RootDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootDesc.NumParameters = 0;
	RootDesc.pParameters = NULL;
	RootDesc.NumStaticSamplers = 0;
	RootDesc.pStaticSamplers = NULL;
	RootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	WRL ComPtr<ID3DBlob> pRootBlob;
	WRL ComPtr<ID3DBlob> pErrorBlob;

	ThrowIfFailed(D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1, pRootBlob.GetAddressOf(), pErrorBlob.GetAddressOf()));
	ThrowIfFailed(pDevice->CreateRootSignature(0, pRootBlob->GetBufferPointer(), pRootBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature)));

	return pRootSignature;

}

ID3D12RootSignature* Scene::GetRootSignature() const
{
	return m_RootSignature.Get();
}
