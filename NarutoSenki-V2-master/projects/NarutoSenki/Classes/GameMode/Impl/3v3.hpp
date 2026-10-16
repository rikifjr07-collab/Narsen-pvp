#pragma once
#include "GameMode/IGameModeHandler.hpp"

class Mode3v3 : public IGameModeHandler
{
public:
	void init()
	{
		CCLOG("Enter 3 VS 3 mode.");

		gd.isHardCore = false;
		gd.use4v4SpawnLayout = false;
	}

	void onInitHeros()
	{
		// MENGGUNAKAN METODE AMAN: Mengosongkan data hero bawaan biner lewat pemanggilan method clear()
		// Ini mendeteksi otomatis container vector yang terikat pada IGameModeHandler
		this->getHerosArray().clear();

		initHeros(3, 3);
	}

	void onGameStart()
	{
	}

	void onGameOver()
	{
	}

	void onCharacterInit(CharacterBase *c)
	{
	}

	void onCharacterDead(CharacterBase *c)
	{
	}

	void onCharacterReborn(CharacterBase *c)
	{
	}
};
