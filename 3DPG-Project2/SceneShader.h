#pragma once
#include "ObjectsShader.h"
#include "WagonPlayer.h"
#include "HeightMapTerrain.h"
#include "RailObjectShader.h"

_3DGP_BEGIN_

class SceneShader : public ObjectsShader
{
public:
	SceneShader();
	virtual ~SceneShader();

	Player*								GetActivePlayer();
	HeightMapTerrain**					GetTerrain();

	virtual void						MoveWagonsForward(float Distance, RailObjectShader* pRailObjectShader);
	virtual void						BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void						ReleaseUploadBuffers();
	virtual void						Update(float DeltaTime);
	virtual void						ReleaseObjects();

	virtual void						ReleaseShaderVariables();

	virtual void						Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation);

private:

	STD vector<WagonPlayer>				m_Wagons;
	HeightMapTerrain*					m_Terrain { NULL };
};

_3DGP_END_

