#pragma once
#include "Mesh.h"
#include "Camera.h"

_3DGP_BEGIN_

class Shader;

class GameObject
{
public:

	GameObject();
	virtual ~GameObject();

	void AddRef() { ++m_ReferenceCount; }
	void Release() { if (--m_ReferenceCount <= 0) delete this; }

	void ReleaseUploadBuffers();
	
	virtual void SetMesh(Mesh* pMesh);
	virtual void SetShader(Shader* pShader);
	virtual void Update(float DeltaTime);
	virtual void PreRender();
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera);

	void Rotate(DX XMFLOAT3* Axis, float Angle);

private:

	int m_ReferenceCount{ 0 };

protected:

	DX XMFLOAT4X4	m_World;
	Mesh*			m_Mesh		{ NULL };
	Shader*			m_Shader	{ NULL };



};

_3DGP_END_

