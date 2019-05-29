#include "stdafx.h"
#include "GameFramework.h"
#include "GameplayScene.h"

_3DGP_USE_

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	GameFramework::Get()->EnableDebugMode();
	GameFramework::Get()->Run<GameplayScene>("Hello", 500, 500);
}