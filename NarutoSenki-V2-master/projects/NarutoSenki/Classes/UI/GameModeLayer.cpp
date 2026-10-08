#include "StartMenu.h"
#include "UI/GameModeLayer.h"
#include "UI/ModeMenuButton.hpp"
#include "Constants/UiFlowKeys.hpp"

extern const GameData kDefaultGameData;
bool global_isHost = true; 
std::string global_ipAddress = "192.168.43.1"; 

// Fungsi Utama Pemicu Jaringan Game
void triggerMultiplayerStart(bool hostMode, const char* targetIp)
{
	global_isHost = hostMode;
	global_ipAddress = targetIp;
	CCLOG("Multiplayer Configured: Host=%d, IP=%s", hostMode, targetIp);
	
	auto handler = getGameModeHandler();
	if (handler) {
		handler->init();
	}
}

// === PENERAPAN FUNGSI KLIK RESMI SESUAI DEKLARASI HEADER ===
void GameModeLayer::onHostClick(Ref* sender)
{
	triggerMultiplayerStart(true, "0.0.0.0");
}

void GameModeLayer::onClient1Click(Ref* sender)
{
	triggerMultiplayerStart(false, "192.168.43.1");
}

void GameModeLayer::onClient2Click(Ref* sender)
{
	triggerMultiplayerStart(false, "192.168.1.1");
}

bool GameModeLayer::init()
{
	auto bgSprite = Sprite::create("red_bg.png");
	FULL_SCREEN_SPRITE(bgSprite);
	bgSprite->setAnchorPoint(Vec2(0, 0));
	bgSprite->setPosition(Vec2(0, 0));
	addChild(bgSprite, -100);

	// menu bars
	auto menu_bar_b = Sprite::create("menu_bar2.png");
	menu_bar_b->setAnchorPoint(Vec2(0, 0));
	FULL_SCREEN_SPRITE(menu_bar_b);
	addChild(menu_bar_b, 2);

	auto menu_bar_t = Sprite::create("menu_bar3.png");
	menu_bar_t->setAnchorPoint(Vec2(0, 0));
	menu_bar_t->setPosition(Vec2(0, winSize.height - menu_bar_t->getContentSize().height));
	FULL_SCREEN_SPRITE(menu_bar_t);
	addChild(menu_bar_t, 2);

	auto modemenu_title = Sprite::createWithSpriteFrameName("startmenu_title.png");
	modemenu_title->setAnchorPoint(Vec2(0, 0));
	modemenu_title->setPosition(Vec2(2, winSize.height - modemenu_title->getContentSize().height - 2));
	addChild(modemenu_title, 3);

	initModeData();

	const int offset = (winSize.width - 460) / 2 + 100 / 2;
	const float posY = (winSize.height / 2) + 30;
	for (int i = 0; i < 3; i++)
	{
		auto mode_btn = ModeMenuButton::create(format("GameMode/{}.png", i + 1));
		mode_btn->mode = (GameMode)i;
		mode_btn->setDelegate(this);
		mode_btn->setPositionX(offset);
		mode_btn->setPositionY((posY + 55 + 7.5f) - i * (55 + 7.5f));
		menuButtons[i] = mode_btn;
		addChild(mode_btn);
	}
	for (int i = 3; i < 6; i++)
	{
		auto mode_btn = ModeMenuButton::create(format("GameMode/{}.png", i + 1));
		mode_btn->mode = (GameMode)i;
		mode_btn->setDelegate(this);
		mode_btn->setPositionX(offset + 10 + (i - 2) * (80 + 5));
		mode_btn->setPositionY(posY);
		menuButtons[i] = mode_btn;
		addChild(mode_btn);
	}
	for (size_t i = 6; i < GameMode::__Internal_Max_Length; i++)
	{
		auto mode_btn = ModeMenuButton::create(format("GameMode/{}.png", i + 1));
		mode_btn->mode = (GameMode)i;
		mode_btn->setDelegate(this);
		mode_btn->setPositionX(offset + 20 + (80 + 5) * 4);
		mode_btn->setPositionY((posY + 47) - (i - 6) * (86 + 8.0f));
		menuButtons[i] = mode_btn;
		addChild(mode_btn);
	}

	for (size_t i = 0; i < menuButtons.size(); i++)
	{
		if (modes[i].isLocked)
		{
			menuButtons.at(i)->useMask2 = modes.at(i).useMask2;
			menuButtons.at(i)->lock();
		}
	}

	menuLabel = CCLabelTTF::create();
	menuLabel->setAnchorPoint(Vec2(0, 0));
	menuLabel->setPosition(Vec2(10, 2));
	addChild(menuLabel, 5);

	// init return button
	auto return_img = MenuItemSprite::create(Sprite::create("UI/return_btn.png"), nullptr, nullptr, this, menu_selector(GameModeLayer::backToMenu));
	Menu *return_btn = Menu::create(return_img, nullptr);
	return_btn->setAnchorPoint(Vec2(1, 0.5f));
	return_btn->setPosition(winSize.width - 38, 65);
	addChild(return_btn, 5);

	return Layer::init();
}

void GameModeLayer::backToMenu(Ref *sender)
{
	SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/cancel.ogg");

	auto menuScene = Scene::create();
	auto menuLayer = StartMenu::create();
	menuScene->addChild(menuLayer);
	Director::sharedDirector()->replaceScene(TransitionFade::create(1.0f, menuScene));
}

void GameModeLayer::initModeData()
{
	auto lang = Application::sharedApplication()->getCurrentLanguage();
	if (lang == LanguageType::kLanguageChinese)
	{
		modes[GameMode::OneVsOne] = {"1 VS 1", ""};
		modes[GameMode::Classic] = {"3 VS 3", "经典模式"};
		modes[GameMode::FourVsFour] = {"4 VS 4", ""};
		modes[GameMode::HardCore_4Vs4] = {"硬核模式 (4 VS 4)", "禁用装备"};
		modes[GameMode::Boss] = {"Boss模式 (3 VS 1)", ""};
		modes[GameMode::Clone] = {"克隆模式 (3 VS 3)", ""};
		modes[GameMode::Deathmatch] = {"死亡竞赛 (3 VS 3)", ""};
		modes[GameMode::RandomDeathmatch] = {"随机死亡竞赛 (3 VS 3)", ""};
	}
	else // English
	{
		modes[GameMode::OneVsOne] = {"1 VS 1", ""};
		modes[GameMode::Classic] = {"3 VS 3", "Classic Mode"};
		modes[GameMode::FourVsFour] = {"4 VS 4", ""};
		modes[GameMode::HardCore_4Vs4] = {"HardCore (4 VS 4)", "Disabled gear"};
		modes[GameMode::Boss] = {"Boss (3 VS 3)", ""};
		modes[GameMode::Clone] = {"Clone (3 VS 3)", ""};
		modes[GameMode::Deathmatch] = {"Deathmatch (3 VS 3)", ""};
		modes[GameMode::RandomDeathmatch] = {"Random Deathmatch (3 VS 3)", ""};
	}

	modes[GameMode::Boss].isLocked = true;
	modes[GameMode::Deathmatch].isLocked = true;
	modes[GameMode::Deathmatch].useMask2 = true;

	for (size_t i = 0; i < GameMode::__Internal_Max_Length; i++)
	{
		auto &data = modes.at(i);
		if (data.isLocked)
			data.description += " (In developtment)";
	}
}

bool GameModeLayer::pushMode(const GameModeData &data)
{
	return true;
}

void GameModeLayer::removeMode(const GameModeData &data)
{
}

void GameModeLayer::selectMode(GameMode mode)
{
	auto data = modes[(size_t)mode];
	string label = data.title;
	if (data.description.size() > 0)
	{
		label += " | ";
		label += data.description;
	}
	menuLabel->setString(label.c_str());

	if (setSelect(mode))
	{
		for (size_t i = 0; i < modes.size(); i++)
			modes.at(i).isLocked = true;
		CCLOG("Selected %s mode", data.title.c_str());

		s_GameMode = mode;
		bool enableCustomSelect = false;
		if (Cheats < kMaxCheats && (mode == GameMode::FourVsFour || mode == GameMode::HardCore_4Vs4))
		{
			enableCustomSelect = false;
		}
		else if (mode != GameMode::RandomDeathmatch && mode != GameMode::Clone)
		{
			enableCustomSelect = Cheats >= kMaxCheats;
		}

		auto pStack = get_luastack;
		auto state = pStack->getLuaState();
		lua_getglobal(state, UiFlowKeys::kEnterSelectLayer);
		pStack->pushInt(mode);
		pStack->pushBoolean(enableCustomSelect);
		pStack->executeFunction(2);

		auto handler = getGameModeHandler();
		handler->setOldCheats(Cheats);

		// === PERBAIKAN MULTIPLAYER RESMI: Menggunakan Fungsi Member Kelas yang Sah & Bersih dari Typo ===
		if (mode == GameMode::OneVsOne) 
		{
			auto labelHost = CCLabelTTF::create("Buat Room (Host)", "Arial", 28);
			auto labelClient1 = CCLabelTTF::create("Gabung (IP: 192.168.43.1)", "Arial", 26);
			auto labelClient2 = CCLabelTTF::create("Gabung (IP: 192.168.1.1)", "Arial", 26);

			// Memanggil fungsi member resmi lewat macro menu_selector bawaan Cocos asli (Bebas Eror Void Pointer)
			auto hostBtn = MenuItemLabel::create(labelHost, this, menu_selector(GameModeLayer::onHostClick));
			auto clientBtn1 = MenuItemLabel::create(labelClient1, this, menu_selector(GameModeLayer::onClient1Click));
			auto clientBtn2 = MenuItemLabel::create(labelClient2, this, menu_selector(GameModeLayer::onClient2Click));

			auto mpMenu = Menu::create(hostBtn, clientBtn1, clientBtn2, nullptr);
			mpMenu->alignItemsVerticallyWithPadding(25.0f);
			mpMenu->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
			this->addChild(mpMenu, 999); 
			
			return; 
		}

		handler->init();
	}
}

bool GameModeLayer::setSelect(GameMode mode)
{
	auto &data = modes.at(mode);
	if (!data.isLocked && data.hasSelected)
		return true;

	for (size_t i = 0; i < modes.size(); i++)
		modes.at(i).hasSelected = false;
	data.hasSelected = true;
	return false;
}
