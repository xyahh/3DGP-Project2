#pragma once
#include "Player.h"
#include "RailObject.h"

_3DGP_BEGIN_

class WagonPlayer : public Player
{

public:
	WagonPlayer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pGraphicsRootSignature);
	virtual ~WagonPlayer();

	virtual Camera* ChangeCamera(Camera::MODE NewCameraMode, float DeltaTime);

	UINT GetWagonNumber() const { return m_WagonNumber; }
	void SetWagonNumber(UINT WagonNumber) { m_WagonNumber = WagonNumber; }

protected:
	UINT m_WagonNumber;
};

_3DGP_END_

