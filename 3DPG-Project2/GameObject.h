#pragma once
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

_3DGP_BEGIN_

class GameObject
{
public:

	GameObject();
	virtual ~GameObject();

	void AddRef() { ++m_ReferenceCount; }
	void Release() { if (--m_ReferenceCount <= 0) delete this; }

	void ReleaseUploadBuffers();
	
	void SetMesh(Mesh* pMesh);
	void SetShader(Shader* pShader);

	virtual void Update(float DeltaTime);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera);

	void Rotate(const DX XMFLOAT3& Axis, float Angle);

	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void ReleaseShaderVariables();

	DX XMFLOAT3 GetPosition() const;
	DX XMFLOAT3 GetRight() const;
	DX XMFLOAT3 GetUp() const;
	DX XMFLOAT3 GetLook() const;

	DX XMVECTOR XM_CALLCONV GetPositionVector() const;
	DX XMVECTOR XM_CALLCONV GetRightVector() const;
	DX XMVECTOR XM_CALLCONV GetUpVector() const;
	DX XMVECTOR XM_CALLCONV GetLookVector() const;

	void SetPosition(const DX XMFLOAT3& Position);
	void XM_CALLCONV SetPosition(DX XMVECTOR_P0 Position);
	void SetPosition(float x, float y, float z);

	void MoveStrafe(float Distance);
	void MoveUp(float Distance);
	void MoveForward(float Distance);

	void Rotate(float Pitch, float Yaw, float Roll);

protected:
	void SetRight(const DX XMFLOAT3& Right);
	void SetUp(const DX XMFLOAT3& Up);
	void SetLook(const DX XMFLOAT3& Look);


	void XM_CALLCONV SetRight(DX XMVECTOR_P0 Right);
	void XM_CALLCONV SetUp(DX XMVECTOR_P0 Up);
	void XM_CALLCONV SetLook(DX XMVECTOR_P0 Look);


private:

	int m_ReferenceCount{ 0 };

protected:

	DX XMFLOAT4X4	m_World;
	Mesh*			m_Mesh		{ NULL };
	Shader*			m_Shader	{ NULL };



};

_3DGP_END_

