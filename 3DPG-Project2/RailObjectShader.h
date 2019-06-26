#pragma once
#include "InstancingShader.h"
#include "WagonPlayer.h"
#include "ColumnObject.h"
#include "HeightMapTerrain.h"

#define RAIL_DEF_COLOR RGB(100, 100, 100)
#define RAIL_TAG_COLOR RGB(255, 0, 0)

_3DGP_BEGIN_

constexpr float BLOCK_LENGTH{ 50.f };
constexpr float RAILS_PER_SEC{ 16.f };
constexpr float RAIL_SPAWN_RATE{ 1.f / RAILS_PER_SEC };
constexpr float RAIL_LIFETIME{ 10.f };
constexpr UINT	RAILS_PER_COLUMN{ 20 };

class RailObjectShader : public InstancingShader
{
public:
	RailObjectShader(HeightMapTerrain** Terrain);
	~RailObjectShader();
	
	virtual void	BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void	ReleaseObjects();

	void			AdjustPlayerPosition(WagonPlayer* pPlayer);
	void			SetSpawnOrientation(const DX XMFLOAT3& Rotation);

	virtual void	CreateShaderVariables(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList);
	virtual void	UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void	ReleaseShaderVariables();

	virtual void	Update(float DeltaTime);
	virtual void	Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation);

	void			SpawnRail();
	void			SpawnColumn(const RailObject& ConnectedRail);

protected:
	HeightMapTerrain**			m_Terrain{ NULL };
	STD vector<RailObject>		m_RailObjects;
	STD vector<ColumnObject>	m_ColumnObjects;
	
	Mesh*					m_ColumnA{ NULL };
	Mesh*					m_ColumnB{ NULL };

	Mesh*					m_RailMesh{ NULL };
	float					m_SpawnTimer{ 0.f };
	DX XMFLOAT3				m_SpawnOrientation;

	MWRL ComPtr<ID3D12Resource>		m_ColumnAResource;
	VS_VB_INSTANCE*					m_ColumnAMapping;
	D3D12_VERTEX_BUFFER_VIEW		m_ColumnAInstancingBufferView;

	MWRL ComPtr<ID3D12Resource>		m_ColumnBResource;
	VS_VB_INSTANCE*					m_ColumnBMapping;
	D3D12_VERTEX_BUFFER_VIEW		m_ColumnBInstancingBufferView;

};

_3DGP_END_

