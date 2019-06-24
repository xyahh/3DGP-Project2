#pragma once

_3DGP_BEGIN_

class Mesh
{
public:
	Mesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~Mesh();

	void AddRef() { ++m_ReferenceCount; }
	void Release() { if (--m_ReferenceCount <= 0) delete this; }

	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, UINT InstanceCount);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, UINT InstanceCount, D3D12_VERTEX_BUFFER_VIEW InstancingBufferView);

	DX BoundingOrientedBox GetBoundingBox() const { return m_BoundingBox; }

protected:

	DX BoundingOrientedBox		m_BoundingBox;

	MWRL ComPtr<ID3D12Resource> m_VertexBuffer;
	MWRL ComPtr<ID3D12Resource> m_VertexUploadBuffer;
							  
	MWRL ComPtr<ID3D12Resource> m_IndexBuffer;
	MWRL ComPtr<ID3D12Resource> m_IndexUploadBuffer;


	D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology	{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	UINT m_Slot			{ 0 };
	UINT m_VertexCount	{ 0 };
	UINT m_Offset		{ 0 };

	UINT m_IndexCount	{ 0 };
	UINT m_StartIndex	{ 0 };
	UINT m_BaseVertex	{ 0 };


private:
	int m_ReferenceCount{ 0 };
};

_3DGP_END_

