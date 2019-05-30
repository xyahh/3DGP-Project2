#pragma once

inline WRL ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, 
	void* pData, UINT nBytes, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES ResourceStates, ID3D12Resource** ppUploadBuffer)
{
	WRL ComPtr <ID3D12Resource> pBuffer;

	D3D12_HEAP_PROPERTIES HeapProperties;
	::ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = HeapType;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.VisibleNodeMask = 1;
	HeapProperties.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC ResourceDesc;
	::ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = nBytes;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES ResourceInitialStates;
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD)
		ResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else
		ResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	ThrowIfFailed(pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, 
		ResourceInitialStates, NULL, IID_PPV_ARGS(&pBuffer)));

	if (pData)
	{
		switch (HeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppUploadBuffer)
			{
				HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
				ThrowIfFailed(pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(ppUploadBuffer)));

				D3D12_RANGE ReadRange	{ 0, 0 };
				UINT8* pBufferDataBegin = NULL;
				(*ppUploadBuffer)->Map(0, &ReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppUploadBuffer)->Unmap(0, NULL);

				pCommandList->CopyResource(pBuffer.Get(), *ppUploadBuffer);

				D3D12_RESOURCE_BARRIER ResourceBarrier;
				::ZeroMemory(&ResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				ResourceBarrier.Transition.pResource = pBuffer.Get();
				ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				ResourceBarrier.Transition.StateAfter = ResourceStates;
				ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

				pCommandList->ResourceBarrier(1, &ResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE ReadRange{ 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pBuffer->Map(0, &ReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}

	return pBuffer;
}