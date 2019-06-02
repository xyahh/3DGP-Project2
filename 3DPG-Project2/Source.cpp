#include "stdafx.h"
#include "GameFramework.h"
#include "GameplayScene.h"

using namespace _3DGP_;

int main()
{
	GameFramework::Get()->Run<GameplayScene>("3DGP Project 2", TRUE, 1920, 1080);
}