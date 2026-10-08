#pragma once
#include "GameMode/GameModeImpl.h"

class ModeMenuButton;

class GameModeLayer : public Layer
{
public:
	static const int kMenuCount = 5;

	bool init();
	void backToMenu(Ref *sender);

	// Tambahan Deklarasi Tombol Multiplayer Resmi agar Bisa Dibaca di File CPP
	void onHostClick(Ref* sender);
	void onClient1Click(Ref* sender);
	void onClient2Click(Ref* sender);

	void initModeData();
	bool pushMode(const GameModeData &data);
	void removeMode(const GameModeData &data);
	void selectMode(GameMode mode);

	CREATE_FUNC(GameModeLayer);

private:
	inline bool setSelect(GameMode mode);

	CCLabelTTF *menuLabel = nullptr;

	vector<ModeMenuButton *> menuButtons = vector<ModeMenuButton *>(static_cast<size_t>(GameMode::__Internal_Max_Length));
	vector<GameModeData> modes = vector<GameModeData>(static_cast<size_t>(GameMode::__Internal_Max_Length));
};
