#include "stdafx.h"
#include "GameFramework.h"
#include "GameplayScene.h"

_3DGP_USE_

int main()
{
	//GameFramework::Get()->EnableDebugMode();
	GameFramework::Get()->Run<GameplayScene>("Hello", 500, 500);
}