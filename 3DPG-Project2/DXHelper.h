#pragma once
#include <stdexcept>

#define RANDOM_COLOR DX XMFLOAT4(\
							rand() / float(RAND_MAX), \
							rand() / float(RAND_MAX), \
							rand() / float(RAND_MAX), \
							rand() / float(RAND_MAX))

// From DXSampleHelper.h 
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
		throw std::exception();
}

inline ID3DBlob* CompileShader(const std::wstring& filename,const std::string& entrypoint, const std::string& target)
{
	UINT uCompileFlags = 0;
#if defined(_DEBUG)
	uCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* pByteCode = nullptr;
	ID3DBlob* pErrors = nullptr;

	ThrowIfFailed(D3DCompileFromFile(filename.c_str(), NULL, NULL, entrypoint.c_str(), target.c_str(), uCompileFlags, 0, &pByteCode, &pErrors));

	if (pErrors)
	{
		OutputDebugStringA((char*)pErrors->GetBufferPointer());
		pErrors->Release();
	}

	return pByteCode;
}

inline ID3DBlob* LoadBinary(const std::wstring& filename)
{
	std::ifstream in(filename, std::ios::binary);
	in.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)in.tellg();
	in.seekg(0, std::ios_base::beg);

	ID3DBlob* pBlob = NULL;
	ThrowIfFailed(D3DCreateBlob(size, &pBlob));

	in.read((char*)pBlob->GetBufferPointer(), size);
	in.close();

	return pBlob;
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

			//Using UINT8 in case we want to only Upload part of the Resource to the Default HEAP.
			UINT8* BufferData = NULL; 
			D3D12_RANGE ReadRange{ 0, 0 };
			(*ppUploadBuffer)->Map(0, &ReadRange, (void**)&BufferData);
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
		UINT8* pBufferData{ NULL };
		pBuffer->Map(0, &ReadRange, (void**)&pBufferData);
		memcpy(pBufferData, pData, nBytes);
		pBuffer->Unmap(0, NULL);
	}
	break;
	}

	return pBuffer;
}
