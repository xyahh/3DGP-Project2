#pragma once
#include "GameObject.h"
#include "Camera.h"

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_RIGHT		0x04
#define DIR_LEFT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20

_3DGP_BEGIN_

class Player : public GameObject
{
public:
	Player();
	virtual ~Player();

	DX XMFLOAT3 GetPosition() const
	{
		return m_Position;
	}

	DX XMFLOAT3 GetRight() const
	{
		return m_Right;
	}

	DX XMFLOAT3 GetUp() const
	{
		return m_Up;
	}

	DX XMFLOAT3 GetLook() const
	{
		return m_Look;
	}

	void SetFriction(float Friction)
	{
		m_Friction = Friction;
	}

	void SetGravity(const DX XMFLOAT3& Gravity)
	{
		m_Gravity = Gravity;
	}

	void SetMaxVelocityXZ(float MaxVelocity)
	{
		m_MaxVelocityXZ = MaxVelocity;
	}

	void SetMaxVelocityY(float MaxVelocity)
	{
		m_MaxVelocityY = MaxVelocity;
	}

	void SetVelocity(const DX XMFLOAT3& Velocity)
	{
		m_Velocity = Velocity;
	}

	void SetPosition(const DX XMFLOAT3& Position)
	{
		m_Position = Position;
	}

	DX XMFLOAT3 GetVelocity() const
	{
		return m_Velocity;
	}

	void GetRotationValues(float* pPitchOut, float* pYawOut, float* pRollOut) const
	{
		if (pPitchOut)	*pPitchOut = m_Rotation.x;
		if (pYawOut)	*pYawOut   = m_Rotation.y;
		if (pRollOut)	*pRollOut  = m_Rotation.z;
	}

	Camera* GetCamera() const
	{
		return m_Camera;
	}

	void SetCamera(Camera* pCamera)
	{
		m_Camera = pCamera;
	}

	void Move(ULONG Direction, float Distance, bool bVelocity = false);
	void Move(const DX XMFLOAT3& Shift, bool bVelocity = false);

	void Rotate(float Pitch, float Yaw, float Roll);
	
	void Update(float DeltaTime);


	virtual void OnPlayerUpdateCallback(float DeltaTime) {}
	void SetPlayerUpdatedContext(LPVOID pContext) { m_PlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float DeltaTime) {}
	void SetCameraUpdatedContext(LPVOID pContext) { m_CameraUpdatedContext = pContext; }


	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);

	virtual Camera* ChangeCamera(Camera::MODE NewCameraMode, float DeltaTime)
	{
		return NULL;
	}

	virtual void PreRender();
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera = NULL);

protected:

	void OnCameraChange(Camera::MODE CurrentCameraMode, Camera::MODE NewCameraMode);

protected:

	DX XMFLOAT3		m_Position;

	DX XMFLOAT3		m_Right;
	DX XMFLOAT3		m_Up;
	DX XMFLOAT3		m_Look;

	DX XMFLOAT3		m_Rotation;

	DX XMFLOAT3		m_Velocity;
	DX XMFLOAT3		m_Gravity;
	float			m_MaxVelocityXZ;
	float			m_MaxVelocityY;

	float			m_Friction;

	LPVOID			m_PlayerUpdatedContext;
	LPVOID			m_CameraUpdatedContext;

	Camera*			m_Camera	{ NULL };
};

_3DGP_END_

