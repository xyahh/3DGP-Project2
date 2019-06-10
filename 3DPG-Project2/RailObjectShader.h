#pragma once
#include "ObjectsShader.h"
#include "WagonPlayer.h"

#define RAIL_DEF_COLOR RGB(100, 100, 100)
#define RAIL_TAG_COLOR RGB(255, 0, 0)

_3DGP_BEGIN_

constexpr float BLOCK_LENGTH{ 25.f };
constexpr float RAILS_PER_SEC{ 16.f };
constexpr float RAIL_SPAWN_RATE{ 1.f / RAILS_PER_SEC };
constexpr float RAIL_LIFETIME{ 10.f };

class RailObjectShader : public ObjectShader
{
public:
	RailObjectShader();
	~RailObjectShader();
	
	virtual void	BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void	ReleaseObjects();
	virtual void	ReleaseUploadBuffers();

	void			AdjustPlayerPosition(WagonPlayer* pPlayer);
	void			SetSpawnRotation(const DX XMFLOAT3& Rotation);

	virtual void	Update(float DeltaTime);
	virtual void	Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation);

	void			SpawnRail();

protected:
	Mesh*					m_RailMesh{ NULL };
	STD vector<TimedObject> m_RailObjects;
	float					m_SpawnTimer{ 0.f };
	DX XMFLOAT3				m_SpawnRotation;
};

_3DGP_END_

