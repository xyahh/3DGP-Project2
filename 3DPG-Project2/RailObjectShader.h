#pragma once
#include "ObjectsShader.h"
#include "WagonPlayer.h"

#define RAIL_DEF_COLOR RGB(100, 100, 100)
#define RAIL_TAG_COLOR RGB(255, 0, 0)

_3DGP_BEGIN_

constexpr float BLOCK_LENGTH{ 5.f };
constexpr float RAILS_PER_SEC{ 16.f };
constexpr float RAIL_SPAWN_RATE{ 1.f / RAILS_PER_SEC };


class RailObjectShader : public ObjectsShader
{
public:
	RailObjectShader();
	~RailObjectShader();

	void AdjustPlayerPosition(WagonPlayer* pPlayer);
};

_3DGP_END_

