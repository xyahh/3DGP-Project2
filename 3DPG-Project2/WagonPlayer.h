#pragma once
#include "Player.h"

_3DGP_BEGIN_

class WagonPlayer : public Player
{
public:
	WagonPlayer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pGraphicsRootSignature);
	virtual ~WagonPlayer();

	virtual Camera* ChangeCamera(Camera::MODE NewCameraMode, float DeltaTime);

};

_3DGP_END_

