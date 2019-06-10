#pragma once
#include "GameObject.h"
#include "Camera.h"
_3DGP_BEGIN_

class Player : public GameObject
{
public:
	Player();
	virtual ~Player();

	void SetFriction(float Friction);

	void SetGravity(const DX XMFLOAT3& Gravity);

	void SetMaxVelocityXZ(float MaxVelocity);

	void SetMaxVelocityY(float MaxVelocity);

	void SetVelocity(const DX XMFLOAT3& Velocity);

	DX XMFLOAT3 GetVelocity() const;

	void GetRotationValues(float* pPitchOut, float* pYawOut, float* pRollOut) const;

	Camera* GetCamera() const;

	void SetCamera(Camera* pCamera);

	void Move(const DX XMFLOAT3& Shift, bool bVelocity = false);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Update(float DeltaTime);

	virtual void OnPlayerUpdateCallback(float DeltaTime);
	void SetPlayerUpdatedContext(LPVOID pContext);

	virtual void OnCameraUpdateCallback(float DeltaTime);
	void SetCameraUpdatedContext(LPVOID pContext);

	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);

	virtual Camera* ChangeCamera(Camera::MODE NewCameraMode, float DeltaTime);

	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera = NULL);

	void OnCameraChange(Camera::MODE CurrentCameraMode, Camera::MODE NewCameraMode);

protected:

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

