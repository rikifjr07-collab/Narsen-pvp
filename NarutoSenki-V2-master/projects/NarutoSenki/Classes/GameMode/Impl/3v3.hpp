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
		// JALUR PALING AMAN: Menggunakan fungsi pembersih bawaan dari IGameModeHandler 
		// tanpa menyentuh nama variabel internal vector agar terhindar dari error typo compiler
		this->clearHeros();

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
