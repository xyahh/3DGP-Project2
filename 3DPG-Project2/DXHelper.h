//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include <stdexcept>

inline std::string HrToString(HRESULT hr)
{
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
	HRESULT Error() const { return m_hr; }
private:
	const HRESULT m_hr;
};

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
		throw HrException(hr);
}

inline ID3DBlob* CompileShader(const std::wstring& filename,const std::string& entrypoint, const std::string& target)
{
	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors;
	ThrowIfFailed(D3DCompileFromFile(filename.c_str(), NULL, NULL, entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors));
	if (errors)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
		errors->Release();
	}
	return byteCode;
}

inline ID3D12Resource * CreateBufferResource(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, void * pData, UINT nBytes, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES ResourceState, ID3D12Resource ** ppUploadBuffer)
{
	ID3D12Resource* pBuffer;

	D3D12_HEAP_PROPERTIES HeapProperties;
	::ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = HeapType;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC ResourceDesc;
	::ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	ResourceDesc.Alignment = 0;
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = nBytes;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES ResourceInitialState;
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD)
		ResourceInitialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	else
		ResourceInitialState = D3D12_RESOURCE_STATE_COPY_DEST;

	pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, ResourceInitialState, NULL, __uuidof(ID3D12Resource), (void**)&pBuffer);

	switch (HeapType)
	{
	case D3D12_HEAP_TYPE_DEFAULT:
		if (ppUploadBuffer)
		{
			HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppUploadBuffer);

			void* BufferData = NULL;
			D3D12_RANGE ReadRange{ 0, 0 };
			(*ppUploadBuffer)->Map(0, &ReadRange, &BufferData);
			memcpy(BufferData, pData, nBytes);
			(*ppUploadBuffer)->Unmap(0, NULL);

			pCommandList->CopyResource(pBuffer, *ppUploadBuffer);

			D3D12_RESOURCE_BARRIER ResourceBarrier;
			::ZeroMemory(&ResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
			ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			ResourceBarrier.Transition.pResource = pBuffer;
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			ResourceBarrier.Transition.StateAfter = ResourceState;
			ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			pCommandList->ResourceBarrier(1, &ResourceBarrier);
		}
		break;
	case D3D12_HEAP_TYPE_UPLOAD:
	{
		D3D12_RANGE ReadRange{ 0, 0 };
		void* pBufferData{ NULL };
		pBuffer->Map(0, &ReadRange, &pBufferData);
		memcpy(pBufferData, pData, nBytes);
		pBuffer->Unmap(0, NULL);
	}
	break;
	}

	return pBuffer;
}
