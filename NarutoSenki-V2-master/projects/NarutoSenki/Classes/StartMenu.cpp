#include "StartMenu.h"

GameMode s_GameMode = GameMode::Classic;
std::array<std::unique_ptr<IGameModeHandler>, GameMode::__Internal_Max_Length> s_ModeHandlers = {
	std::make_unique<Mode1v1>(),
	std::make_unique<Mode3v3>(),
	std::make_unique<Mode4v4>(),
	std::make_unique<ModeHardCore>(),
	std::make_unique<ModeBoss>(),
	std::make_unique<ModeClone>(false),
	std::make_unique<ModeDeathmatch>(),
	std::make_unique<ModeRandomDeathmatch>(),
};
int Cheats = 0;
bool enableCustomSelect = false;

bool MenuButton::init(const char *szImage)
{
	RETURN_FALSE_IF(!Sprite::init());
	initWithSpriteFrameName(szImage);
	setAnchorPoint(Vec2(0.5, 0));
	return true;
}

void MenuButton::onEnter()
{
	Sprite::onEnter();
	Director::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 10, true);
}

void MenuButton::onExit()
{
	Sprite::onExit();
	Director::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

CCRect MenuButton::getRect()
{
	CCSize size = getContentSize();
	return CCRect(0, 0, size.width, size.height);
}

bool MenuButton::containsTouchLocation(Touch *touch)
{
	return getRect().containsPoint(convertTouchToNodeSpace(touch));
}

void MenuButton::setBtnType(MenuButtonType type)
{
	_type = type;
}

MenuButtonType MenuButton::getBtnType()
{
	return _type;
}

bool MenuButton::ccTouchBegan(Touch *touch, Event *event)
{
	if (!containsTouchLocation(touch))
		return false;
	prePosY = 0;
	return true;
}

void MenuButton::ccTouchMoved(Touch *touch, Event *event)
{
	if (prePosY == 0)
	{
		prePosY = touch->getLocation().y;
	}
	else
	{
		if (getBtnType() != MenuButtonType::HardCore)
		{
			if (abs(touch->getLocation().y - prePosY) > 16)
			{
				if (touch->getLocation().y < prePosY)
				{
					_startMenu->isClockwise = true;
				}
				else
				{
					_startMenu->isClockwise = false;
				}
				_startMenu->isDrag = true;
			}
		}
	}
}

void MenuButton::ccTouchEnded(Touch *touch, Event *event)
{
	if (_isTop && !_startMenu->isDrag)
	{
		switch (_type)
		{
		case MenuButtonType::Training:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->onTrainingCallBack();
			break;
		case MenuButtonType::Credits:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->onCreditsCallBack();
			break;
		case MenuButtonType::Exit:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->onExitCallBack();
			break;
		case MenuButtonType::Custom:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->onTrainingCallBack();
			break;
		case MenuButtonType::HardCore:
			SimpleAudioEngine::sharedEngine()->playEffect(SELECT_SOUND);
			auto frame = getSpriteFrame("menu05_text.png");
			if(frame) _startMenu->menuText->setDisplayFrame(frame);
			_startMenu->onHardLayerCallBack();
			break;
		}
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->playEffect(SELECT_SOUND);
		prePosY = 0;
		_startMenu->scrollMenu(getPositionY());
		_startMenu->isDrag = false;
	}
}

void MenuButton::playSound()
{
	SimpleAudioEngine::sharedEngine()->stopAllEffects();
	switch (_type)
	{
	case MenuButtonType::Training:
		SimpleAudioEngine::sharedEngine()->playEffect(TRAINING_SOUND);
		break;
	case MenuButtonType::Custom:
		SimpleAudioEngine::sharedEngine()->playEffect(NETWORK_SOUND);
		break;
	case MenuButtonType::Credits:
		SimpleAudioEngine::sharedEngine()->playEffect(CREDITS_SOUND);
		break;
	case MenuButtonType::Exit:
		SimpleAudioEngine::sharedEngine()->playEffect(EXIT_SOUND);
		break;
	default:
		break;
	}
}

MenuButton *MenuButton::create(const char *szImage)
{
	auto mb = new MenuButton();
	if (mb && mb->init(szImage))
	{
		mb->autorelease();
		return mb;
	}
	else
	{
		delete mb;
		return nullptr;
	}
}

StartMenu::StartMenu()
{
	isClockwise = false;
	isDrag = false;
	menuText = nullptr;
	hardCoreLayer = nullptr;
	notice_layer = nullptr;
	noticeLabel = nullptr;
	login_btn = nullptr;
}

bool StartMenu::init()
{
	RETURN_FALSE_IF(!Layer::init());
	addSprites("Menu.plist");
	addSprites("Result.plist");
	addSprites("NamePlate.plist");

	Sprite *gold_left = Sprite::createWithSpriteFrameName("gold_left.png");
	if(gold_left) {
		gold_left->setAnchorPoint(Vec2(0, 0));
		gold_left->setPosition(Vec2(0, 20));
		addChild(gold_left, 1);
	}

	Sprite *gold_right = Sprite::createWithSpriteFrameName("gold_right.png");
	if(gold_right) {
		gold_right->setAnchorPoint(Vec2(0, 1));
		gold_right->setPosition(Vec2(winSize.width - gold_right->getContentSize().width - 20, winSize.height - 20));
		addChild(gold_right, 1);
	}

	Sprite *cloud_left = Sprite::createWithSpriteFrameName("cloud.png");
	if(cloud_left) {
		cloud_left->setPosition(Vec2(0, 15));
		cloud_left->setFlipX(true);
		cloud_left->setFlipY(true);
		cloud_left->setAnchorPoint(Vec2(0, 0));
		addChild(cloud_left, 1);

		auto cmv1 = MoveBy::create(1, Vec2(-15, 0));
		auto cseq1 = RepeatForever::create(newSequence(cmv1, cmv1->reverse()));
		cloud_left->runAction(cseq1);
	}

	Sprite *cloud_right = Sprite::createWithSpriteFrameName("cloud.png");
	if(cloud_right) {
		cloud_right->setPosition(Vec2(winSize.width - cloud_right->getContentSize().width,
									  winSize.height - (cloud_right->getContentSize().height + 15)));
		cloud_right->setAnchorPoint(Vec2(0, 0));
		addChild(cloud_right, 1);

		auto cmv2 = MoveBy::create(1, Vec2(15, 0));
		auto cseq2 = RepeatForever::create(newSequence(cmv2, cmv2->reverse()));
		cloud_right->runAction(cseq2);
	}

	Sprite *menu_bar_b = Sprite::create("menu_bar2.png");
	if(menu_bar_b) {
		menu_bar_b->setAnchorPoint(Vec2(0, 0));
		FULL_SCREEN_SPRITE(menu_bar_b);
		addChild(menu_bar_b, 2);
	}

	Sprite *menu_bar_t = Sprite::create("menu_bar3.png");
	if(menu_bar_t) {
		menu_bar_t->setAnchorPoint(Vec2(0, 0));
		menu_bar_t->setPosition(Vec2(0, winSize.height - menu_bar_t->getContentSize().height));
		FULL_SCREEN_SPRITE(menu_bar_t);
		addChild(menu_bar_t, 2);
	}

	Sprite *startmenu_title = Sprite::createWithSpriteFrameName("startmenu_title.png");
	if(startmenu_title) {
		startmenu_title->setAnchorPoint(Vec2(0, 0));
		startmenu_title->setPosition(Vec2(2, winSize.height - startmenu_title->getContentSize().height - 2));
		addChild(startmenu_title, 3);
	}

	auto gamemode_btn = MenuButton::create("menu01.png");
	if(gamemode_btn) {
		gamemode_btn->setDelegate(this);
		gamemode_btn->setBtnType(MenuButtonType::Custom);
		gamemode_btn->setScale(0.5f);
		gamemode_btn->setPositionY(_pos03);
		_menuArray.push_back(gamemode_btn);
	}

	auto credits_btn = MenuButton::create("menu04.png");
	if(credits_btn) {
		credits_btn->setDelegate(this);
		credits_btn->setBtnType(MenuButtonType::Credits);
		credits_btn->setScale(0.5f);
		credits_btn->setVisible(false);
		credits_btn->_isBottom = true;
		credits_btn->setPositionY(_pos02);
		_menuArray.push_back(credits_btn);
	}

	auto training_btn = MenuButton::create("menu02.png");
	if(training_btn) {
		training_btn->setDelegate(this);
		training_btn->setBtnType(MenuButtonType::Training);
		training_btn->_isTop = true;
		training_btn->setPositionY(_pos02);
		_menuArray.push_back(training_btn);
	}

	auto exit_btn = MenuButton::create("menu03.png");
	if(exit_btn) {
		exit_btn->setDelegate(this);
		exit_btn->setBtnType(MenuButtonType::Exit);
		exit_btn->setScale(0.5f);
		exit_btn->setPositionY(_pos01);
		_menuArray.push_back(exit_btn);
	}

	menuText = Sprite::createWithSpriteFrameName("menu02_text.png");
	if(menuText) {
		menuText->setAnchorPoint(Vec2(0, 0));
		menuText->setPosition(Vec2(10, 2));
		addChild(menuText, 5);
	}

	for (auto menu : _menuArray)
	{
		menu->setPositionX(105);
		addChild(menu, 2);
	}
	
	auto versionLabel = CCLabelBMFont::create(VERSION_CODE, Fonts::Default);
	if(versionLabel) {
		versionLabel->setScale(0.3f);
		versionLabel->setPosition(winSize.width - 25, 10);
		addChild(versionLabel, 5);
	}

	Sprite *avator = Sprite::createWithSpriteFrameName("avator1.png");
	if(avator) {
		avator->setAnchorPoint(Vec2(0, 0));
		avator->setOpacity(0);
		avator->setPosition(Vec2(winSize.width - avator->getContentSize().width, 19));
		addChild(avator, 1);

		Vector<SpriteFrame *> frames;
		Vector<FiniteTimeAction *> list;
		int idx = 0;
		while (++idx < 5)
		{
			auto frame = getSpriteFrame("avator{}.png", idx);
			if(frame) frames.pushBack(frame);
			auto tempAnimation = Animation::createWithSpriteFrames(frames, 0.1f);
			auto tempAction = Animate::create(tempAnimation);
			if(tempAction) list.pushBack(tempAction);
			auto fadeIn = FadeIn::create(0.8f);
			auto delay = DelayTime::create(1.0f);
			auto fadeOut = FadeOut::create(0.5f);
			list.pushBack(fadeIn);
			list.pushBack(delay);
			list.pushBack(fadeOut);
		}
		if(list.size() > 0) avator->runAction(RepeatForever::create(Sequence::create(list)));
	}

	auto news_btn_img = Sprite::createWithSpriteFrameName("news_btn.png");
	if(news_btn_img) {
		MenuItem *news_btn = MenuItemSprite::create(news_btn_img, nullptr, this, menu_selector(StartMenu::onNewsBtn));
		Menu *menu = Menu::create(news_btn, nullptr);
		news_btn->setAnchorPoint(Vec2(0, 0.5f));
		menu->setPosition(15, winSize.height - 50);
		addChild(menu, 5);
	}

	setNotice();

	auto login_btn_img = Sprite::createWithSpriteFrameName("login_btn.png");
	if(login_btn_img) {
		login_btn = MenuItemSprite::create(login_btn_img, nullptr, this, menu_selector(StartMenu::onLoginBtn));
		Menu *menu2 = Menu::create(login_btn, nullptr);
		login_btn->setAnchorPoint(Vec2(1, 0.5f));
		menu2->setPosition(winSize.width - 15, winSize.height - 50);
		addChild(menu2, 5);
	}

	scheduleUpdate();
	return true;
}

void StartMenu::onEnter()
{
	Layer::onEnter();
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
	if (UserDefault::sharedUserDefault()->getBoolForKey("isBGM"))
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(MENU_MUSIC, true);
	}
}

void StartMenu::onExit()
{
	Layer::onExit();
}

void StartMenu::onTrainingCallBack() { enterTrainingMode(); }
void StartMenu::onHardCoreOn(Ref *sender) {}
void StartMenu::onHardCoreOff(Ref *sender) {}
void StartMenu::onHardLayerCallBack() {}
void StartMenu::enterCustomMode() {}
void StartMenu::enterTrainingMode() {}
void StartMenu::enterSelectLayer() {
auto scene = SelectLayer::createScene();
Director::getInstance()->replaceScene(scene);
}
void StartMenu::onCreditsCallBack() {}
void StartMenu::onExitCallBack() { Director::sharedDirector()->end(); }
void StartMenu::onNewsBtn(Ref *sender) {}
void StartMenu::onLoginBtn(Ref *sender) {}
void StartMenu::scrollMenu(int posY) {}
void StartMenu::setNotice() {}
void StartMenu::update(float dt) {}
void StartMenu::keyBackClicked() { onExitCallBack(); }
void StartMenu::setCheats(int cheats) { Cheats = cheats; }
