#pragma once
#include "GameMode/IGameModeHandler.hpp"

class Mode3v3 : public IGameModeHandler
{
public:
	void init()
	{
		CCLOG("Enter 3 VS 3 mode secara Offline.");
		gd.isHardCore = false;
		gd.use4v4SpawnLayout = false;
	}

	void onInitHeros()
	{
		initHeros(3, 3);
	}

	void onGameStart() {}
	void onGameOver() {}
	void onCharacterInit(CharacterBase *c) {}
	void onCharacterDead(CharacterBase *c) {}
	void onCharacterReborn(CharacterBase *c) {}
};
