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
	virtual void PreRender();
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera);

	void Rotate(const DX XMFLOAT3& Axis, float Angle);

	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void ReleaseShaderVariables();

	DX XMFLOAT3 GetPosition();
	DX XMFLOAT3 GetLook();
	DX XMFLOAT3 GetUp();
	DX XMFLOAT3 GetRight();

	void SetPosition(const DX XMFLOAT3& Position);
	void SetPosition(float x, float y, float z);

	void MoveStrafe(float Distance);
	void MoveUp(float Distance);
	void MoveForward(float Distance);

	void Rotate(float Pitch, float Yaw, float Roll);

private:

	int m_ReferenceCount{ 0 };

protected:

	DX XMFLOAT4X4	m_World;
	Mesh*			m_Mesh		{ NULL };
	Shader*			m_Shader	{ NULL };



};

_3DGP_END_

