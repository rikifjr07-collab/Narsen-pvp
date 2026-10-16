#pragma once
#include "GameMode/IGameModeHandler.hpp"

class Mode3v3 : public IGameModeHandler
{
public:
	void init()
	{
		CCLOG("Enter 3 VS 3 mode.");
		// Mengatur konfigurasi dasar mode tim 3v3
		gd.isHardCore = false; 
		gd.use4v4SpawnLayout = false;
	}

	void onInitHeros()
	{
		// PENGAMAN UTAMA: Bersihkan total array jagoan lawas di memori
		// agar tidak ada sisa pointer kosong (null pointer) yang memicu FC
		this->herosArray.clear();

		// Panggil fungsi inisialisasi basis C++ untuk mendaftarkan ulang 3 player & 3 com
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
