#pragma once
#define GET_ASPECT_RATIO(x, y) (float(x) / float(y))

_3DGP_BEGIN_

class Player;

class Camera
{
public:

	enum class MODE
	{
		NONE,
		FIRST_PERSON,
		THIRD_PERSON
	};

private:

	void Init();

public:

	Camera();
	Camera(Camera* pCamera);
	virtual ~Camera();

	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);

	void GenerateViewMatrix();
	void GenerateViewMatrix(const DX XMFLOAT3& Position, const DX XMFLOAT3& LookAt, const DX XMFLOAT3& Up);
	void RegenerateViewMatrix();

	void GenerateProjMatrix(float Near, float Far, float AspectRatio, float FOVAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int Width, int Height, float MinDepth=0.f, float MaxDepth=1.f);
	void SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom);
	virtual void UpdateViewportsAndScissorRects(ID3D12GraphicsCommandList* pCommandList);

	void SetPlayer(Player* pPlayer) 
	{ 
		m_Player = pPlayer; 
	}

	Player* GetPlayer() const 
	{ 
		return m_Player; 
	}

	void SetMode(Camera::MODE Mode) 
	{
		m_CameraMode = Mode;
	}

	Camera::MODE GetMode() const
	{
		return m_CameraMode;
	}

	void SetPosition(const DX XMFLOAT3& Position)
	{
		m_Position = Position;
	}
	DX XMFLOAT3 GetPosition() const
	{
		return m_Position;
	}

	void SetLookAtPosition(const DX XMFLOAT3& LookAtWorld)
	{
		m_Target = LookAtWorld;
	}
	DX XMFLOAT3 GetLookAtPosition() const
	{
		return m_Target;
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

	void SetOffset(const DX XMFLOAT3& Offset)
	{
		m_Offset = Offset;
	}

	DX XMFLOAT3 GetOffset() const
	{
		return m_Offset;
	}

	void SetTimeLag(float TimeLag)
	{
		m_TimeLag = TimeLag;
	}

	float GetTimeLag() const
	{
		return m_TimeLag;
	}

	DX XMFLOAT4X4 GetViewMatrix() const
	{
		return m_View;
	}

	DX XMFLOAT4X4 GetProjMatrix() const
	{
		return m_Proj;
	}

	D3D12_VIEWPORT GetViewport() const 
	{
		return m_Viewport;
	}

	D3D12_RECT GetScissorRect() const
	{
		return m_ScissorRect;
	}

	virtual void Move(const DX XMFLOAT3& Displacement)
	{
		m_Position.x += Displacement.x;
		m_Position.y += Displacement.y;
		m_Position.z += Displacement.z;
	}

	virtual void Rotate(float Pitch, float Yaw, float Roll) {}
	virtual void Update(const DX XMFLOAT3& LookAt, float DeltaTime) {}
	virtual void SetTarget(const DX XMFLOAT3& LookAt) {}

protected:

	Player*			m_Player{ NULL };

	DX XMFLOAT3		m_Position;

	DX XMFLOAT3		m_Right;
	DX XMFLOAT3		m_Up;
	DX XMFLOAT3		m_Look;

	Camera::MODE	m_CameraMode;

	DX XMFLOAT3		m_Target;
	DX XMFLOAT3		m_Offset;
	DX XMFLOAT4		m_Rotation;

	float			m_TimeLag;

	DX XMFLOAT4X4	m_View;
	DX XMFLOAT4X4	m_Proj;

	D3D12_VIEWPORT	m_Viewport;
	D3D12_RECT		m_ScissorRect;

};

_3DGP_END_
