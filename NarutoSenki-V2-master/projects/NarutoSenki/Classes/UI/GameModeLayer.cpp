#include "StartMenu.h"
#include "UI/GameModeLayer.h"
#include "UI/ModeMenuButton.hpp"
#include "Constants/UiFlowKeys.hpp"

extern const GameData kDefaultGameData;
bool global_isHost = true; // Sakelar global: true = Host, false = Client
std::string global_ipAddress = "192.168.43.1"; // Default IP Hotspot lokal

bool GameModeLayer::init()
{
	// PERBAIKAN UTAMA: Init layer wajib di baris paling pertama!
	if (!Layer::init()) {
		return false;
	}

	auto bgSprite = Sprite::create("red_bg.png");
	if (bgSprite) {
		FULL_SCREEN_SPRITE(bgSprite);
		bgSprite->setAnchorPoint(Vec2(0, 0));
		bgSprite->setPosition(Vec2(0, 0));
		addChild(bgSprite, -100);
	}

	// menu bars
	auto menu_bar_b = Sprite::create("menu_bar2.png");
	if (menu_bar_b) {
		menu_bar_b->setAnchorPoint(Vec2(0, 0));
		FULL_SCREEN_SPRITE(menu_bar_b);
		addChild(menu_bar_b, 2);
	}

	auto menu_bar_t = Sprite::create("menu_bar3.png");
	if (menu_bar_t) {
		menu_bar_t->setAnchorPoint(Vec2(0, 0));
		menu_bar_t->setPosition(Vec2(0, winSize.height - menu_bar_t->getContentSize().height));
		FULL_SCREEN_SPRITE(menu_bar_t);
		addChild(menu_bar_t, 2);
	}

	auto modemenu_title = Sprite::createWithSpriteFrameName("startmenu_title.png");
	if (modemenu_title) {
		modemenu_title->setAnchorPoint(Vec2(0, 0));
		modemenu_title->setPosition(Vec2(2, winSize.height - modemenu_title->getContentSize().height - 2));
		addChild(modemenu_title, 3);
	}

	initModeData();

		const int offset = (winSize.width - 460) / 2 + 100 / 2;
	const float posY = (winSize.height / 2) + 30;
	
	// PERBAIKAN: Mengganti format() C++20 dengan CCString bawaan Cocos2d-x agar pembacaan aset aman
	for (int i = 0; i < 3; i++)
	{
		CCString* imgPath = CCString::createWithFormat("GameMode/%d.png", i + 1);
		auto mode_btn = ModeMenuButton::create(imgPath->getCString());
		if (mode_btn) {
			mode_btn->mode = (GameMode)i;
			mode_btn->setDelegate(this);
			mode_btn->setPositionX(offset);
			mode_btn->setPositionY((posY + 55 + 7.5f) - i * (55 + 7.5f));
			menuButtons[i] = mode_btn;
			addChild(mode_btn);
		}
	}
	for (int i = 3; i < 6; i++)
	{
		CCString* imgPath = CCString::createWithFormat("GameMode/%d.png", i + 1);
		auto mode_btn = ModeMenuButton::create(imgPath->getCString());
		if (mode_btn) {
			mode_btn->mode = (GameMode)i;
			mode_btn->setDelegate(this);
			mode_btn->setPositionX(offset + 10 + (i - 2) * (80 + 5));
			mode_btn->setPositionY(posY);
			menuButtons[i] = mode_btn;
			addChild(mode_btn);
		}
	}
	for (int i = 6; i < GameMode::__Internal_Max_Length; i++)
	{
		CCString* imgPath = CCString::createWithFormat("GameMode/%d.png", i + 1);
		auto mode_btn = ModeMenuButton::create(imgPath->getCString());
		if (mode_btn) {
			mode_btn->mode = (GameMode)i;
			mode_btn->setDelegate(this);
			mode_btn->setPositionX(offset + 20 + (80 + 5) * 4);
			mode_btn->setPositionY((posY + 47) - (i - 6) * (86 + 8.0f));
			menuButtons[i] = mode_btn;
			addChild(mode_btn);
		}
	}

	
	for (size_t i = 0; i < menuButtons.size(); i++)
	{
		if (menuButtons.at(i) && modes[i].isLocked)
		{
			menuButtons.at(i)->useMask2 = modes.at(i).useMask2;
			menuButtons.at(i)->lock();
		}
	}

	menuLabel = CCLabelTTF::create();
	if (menuLabel) {
		menuLabel->setAnchorPoint(Vec2(0, 0));
		menuLabel->setPosition(Vec2(10, 2));
		addChild(menuLabel, 5);
	}

	// init return button secara aman dari null pointer crash
	auto return_sprite = Sprite::create("UI/return_btn.png");
	if (!return_sprite) {
		return_sprite = Sprite::create("menu_bar2.png"); // Fallback jika aset gambar hilang agar tidak FC
	}
	
	auto return_img = MenuItemSprite::create(return_sprite, nullptr, nullptr, this, menu_selector(GameModeLayer::backToMenu));
	if (return_img) {
		Menu *return_btn = Menu::create(return_img, nullptr);
		if (return_btn) {
			return_btn->setAnchorPoint(Vec2(1, 0.5f));
			return_btn->setPosition(winSize.width - 38, 65);
			addChild(return_btn, 5);
		}
	}

	return true;
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

void GameModeLayer::onHostSelected(Ref* sender)
{
	global_isHost = true;
	global_ipAddress = "0.0.0.0"; 
	CCLOG("Multiplayer: Menjadi HOST");
	
	auto handler = getGameModeHandler();
	if(handler) handler->init();
}

void GameModeLayer::onClientSelected(Ref* sender)
{
	global_isHost = false;
	CCLOG("Multiplayer: Menjadi CLIENT - Membuka Input IP");

	if(sender) {
		auto parentMenu = ((CCNode*)sender)->getParent();
		if(parentMenu) parentMenu->removeFromParent();
	}

	auto labelClient1 = CCLabelTTF::create("Gabung Hotspot (IP: 192.168.43.1)", "Arial", 26);
	auto labelClient2 = CCLabelTTF::create("Gabung Wi-Fi Rumah (IP: 192.168.1.1)", "Arial", 26);

	auto cBtn1 = CCMenuItemLabel::create(labelClient1, this, menu_selector(GameModeLayer::onJoinHotspot));
	auto cBtn2 = CCMenuItemLabel::create(labelClient2, this, menu_selector(GameModeLayer::onJoinWifi));

	auto subMenu = Menu::create(cBtn1, cBtn2, nullptr);
	subMenu->alignItemsVerticallyWithPadding(20.0f);
	subMenu->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(subMenu, 1000);
}

void GameModeLayer::onJoinHotspot(Ref* sender)
{
	global_ipAddress = "192.168.43.1";
	auto handler = getGameModeHandler();
	if(handler) handler->init();
}

void GameModeLayer::onJoinWifi(Ref* sender)
{
	global_ipAddress = "192.168.1.1";
	auto handler = getGameModeHandler();
	if(handler) handler->init();
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

		if (mode == GameMode::OneVsOne) 
		{
			auto hostBtn = CCMenuItemFont::create("Buat Room (Host)", this, menu_selector(GameModeLayer::onHostSelected));
			auto clientBtn = CCMenuItemFont::create("Gabung Room (Client)", this, menu_selector(GameModeLayer::onClientSelected));

			hostBtn->setFontSize(32);
			clientBtn->setFontSize(32);

			auto mpMenu = Menu::create(hostBtn, clientBtn, nullptr);
			mpMenu->alignItemsVerticallyWithPadding(20.0f);
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
