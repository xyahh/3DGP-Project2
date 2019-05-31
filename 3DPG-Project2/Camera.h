#pragma once

_3DGP_BEGIN_

struct VS_CB_CAMERA_INFO
{
	DX XMFLOAT4X4 m_View;
	DX XMFLOAT4X4 m_Proj;
};

class Camera
{
public:
	Camera();
	~Camera();

	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);

	void GenerateViewMatrix(const DX XMFLOAT3& Position, const DX XMFLOAT3& LookAt, const DX XMFLOAT3& Up);
	void GenerateProjMatrix(float Near, float Far, float AspectRatio, float FOVAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int Width, int Height, float MinDepth=0.f, float MaxDepth=1.f);
	void SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom);

	virtual void UpdateViewportsAndScissorRects(ID3D12GraphicsCommandList* pCommandList);

protected:

	DX XMFLOAT4X4	m_View;
	DX XMFLOAT4X4	m_Proj;

	D3D12_VIEWPORT	m_Viewport;
	D3D12_RECT		m_ScissorRect;

};

_3DGP_END_
