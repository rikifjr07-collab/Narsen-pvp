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

/*----------------------
init MenuButton ;
----------------------*/

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

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	// JniMethodInfo minfo;
	// bool isHave = JniHelper::getStaticMethodInfo(minfo, "net/zakume/game/NarutoSenki", "showBanner", "()V");
	// if (isHave)
	// {
	// 	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	// }
#endif
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
			_startMenu->onTrainingCallBack(); // Dialihkan ke training secara aman agar tombol bereaksi
			break;
		case MenuButtonType::HardCore:
			SimpleAudioEngine::sharedEngine()->playEffect(SELECT_SOUND);
			auto frame = getSpriteFrame("menu05_text.png");
			if (frame) _startMenu->menuText->setDisplayFrame(frame);
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

/*----------------------
init StartMenu layer;
----------------------*/

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
	if (gold_left) {
		gold_left->setAnchorPoint(Vec2(0, 0));
		gold_left->setPosition(Vec2(0, 20));
		addChild(gold_left, 1);
	}

	Sprite *gold_right = Sprite::createWithSpriteFrameName("gold_right.png");
	if (gold_right) {
		gold_right->setAnchorPoint(Vec2(0, 1));
		gold_right->setPosition(Vec2(winSize.width - gold_right->getContentSize().width - 20, winSize.height - 20));
		addChild(gold_right, 1);
	}

	Sprite *cloud_left = Sprite::createWithSpriteFrameName("cloud.png");
	if (cloud_left) {
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
	if (cloud_right) {
		cloud_right->setPosition(Vec2(winSize.width - cloud_right->getContentSize().width,
									  winSize.height - (cloud_right->getContentSize().height + 15)));
		cloud_right->setAnchorPoint(Vec2(0, 0));
		addChild(cloud_right, 1);

		auto cmv2 = MoveBy::create(1, Vec2(15, 0));
		auto cseq2 = RepeatForever::create(newSequence(cmv2, cmv2->reverse()));
		cloud_right->runAction(cseq2);
	}

	Sprite *menu_bar_b = Sprite::create("menu_bar2.png");
	if (menu_bar_b) {
		menu_bar_b->setAnchorPoint(Vec2(0, 0));
		FULL_SCREEN_SPRITE(menu_bar_b);
		addChild(menu_bar_b, 2);
	}

	Sprite *menu_bar_t = Sprite::create("menu_bar3.png");
	if (menu_bar_t) {
		menu_bar_t->setAnchorPoint(Vec2(0, 0));
		menu_bar_t->setPosition(Vec2(0, winSize.height - menu_bar_t->getContentSize().height));
		FULL_SCREEN_SPRITE(menu_bar_t);
		addChild(menu_bar_t, 2);
	}

	Sprite *startmenu_title = Sprite::createWithSpriteFrameName("startmenu_title.png");
	if (startmenu_title) {
		startmenu_title->setAnchorPoint(Vec2(0, 0));
		startmenu_title->setPosition(Vec2(2, winSize.height - startmenu_title->getContentSize().height - 2));
		addChild(startmenu_title, 3);
	}

	auto gamemode_btn = MenuButton::create("menu01.png");
	if (gamemode_btn) {
		gamemode_btn->setDelegate(this);
		gamemode_btn->setBtnType(MenuButtonType::Custom);
		gamemode_btn->setScale(0.5f);
		gamemode_btn->setPositionY(_pos03);
		_menuArray.push_back(gamemode_btn);
	}

	auto credits_btn = MenuButton::create("menu04.png");
	if (credits_btn) {
		credits_btn->setDelegate(this);
		credits_btn->setBtnType(MenuButtonType::Credits);
		credits_btn->setScale(0.5f);
		credits_btn->setVisible(false);
		credits_btn->_isBottom = true;
		credits_btn->setPositionY(_pos02);
		_menuArray.push_back(credits_btn);
	}

	auto training_btn = MenuButton::create("menu02.png");
	if (training_btn) {
		training_btn->setDelegate(this);
		training_btn->setBtnType(MenuButtonType::Training);
		training_btn->_isTop = true;
		training_btn->setPositionY(_pos02);
		_menuArray.push_back(training_btn);
	}

	auto exit_btn = MenuButton::create("menu03.png");
	if (exit_btn) {
		exit_btn->setDelegate(this);
		exit_btn->setBtnType(MenuButtonType::Exit);
		exit_btn->setScale(0.5f);
		exit_btn->setPositionY(_pos01);
		_menuArray.push_back(exit_btn);
	}

	menuText = Sprite::createWithSpriteFrameName("menu02_text.png");
	if (menuText) {
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
	if (versionLabel) {
		versionLabel->setScale(0.3f);
		versionLabel->setPosition(winSize.width - 25, 10);
		addChild(versionLabel, 5);
	}

	Sprite *avator = Sprite::createWithSpriteFrameName("avator1.png");
		Vector<SpriteFrame *> frames;
	Vector<FiniteTimeAction *> list;
	int idx = 0;
	while (++idx < 5)
	{
		// Menggunakan CCString bawaan v2 untuk format string nama file gambar
		CCString* nameStr = CCString::createWithFormat("avator%d.png", idx);
		
		// Memperbaiki getInstance() menjadi sharedSpriteFrameCache() untuk Cocos2d-x v2
		auto frame = SpriteFrameCache::sharedSpriteFrameCache()->getSpriteFrameByName(nameStr->getCString());
		if (frame) {
			frames.pushBack(frame);
		}
		auto tempAnimation = Animation::createWithSpriteFrames(frames, 0.1f);
		auto tempAction = Animate::create(tempAnimation);
		if (tempAction) list.pushBack(tempAction);
		
		auto fadeIn = FadeIn::create(0.8f);
		auto delay = DelayTime::create(1.0f);
		auto fadeOut = FadeOut::create(0.5f);
		list.pushBack(fadeIn);
		list.pushBack(delay);
		list.pushBack(fadeOut);
	}

}
	MenuItem *news_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("news_btn.png"), nullptr, this, menu_selector(StartMenu::onNewsBtn));
	Menu *menu = Menu::create(news_btn, nullptr);
	news_btn->setAnchorPoint(Vec2(0, 0.5f));
	menu->setPosition(15, winSize.height - 50);
	addChild(menu, 5);

	setNotice();

	login_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("login_btn.png"), nullptr, this, menu_selector(StartMenu::onLoginBtn));
	Menu *menu2 = Menu::create(login_btn, nullptr);
    login_btn->setAnchorPoint(Vec2(1, 0.5f));
menu2->setPosition(winSize.width - 15, winSize.height - 50);
addChild(menu2, 5);
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
void StartMenu::onLoginBtn(CCObject *sender)
{
auto tip = CCTips::create("ServerMainten");
addChild(tip, 5000);
return;
}
void StartMenu::update(float dt)
{
if (!noticeLabel) {
return;
}
float currentX = noticeLabel->getPositionX();
float lableX = noticeLabel->getContentSize().width;
if (noticeLabel->getPositionX() >= -lableX) {
noticeLabel->setPositionX(noticeLabel->getPositionX() - 0.6f);
}
else {
noticeLabel->setPositionX(190);
}
}
void StartMenu::setNotice()
{
if (!notice_layer)
{
notice_layer = Layer::create();
Sprite *notice_bg = Sprite::createWithSpriteFrameName("notice_bg.png");
notice_bg->setAnchorPoint(Vec2(0, 0));
notice_bg->setPosition(Vec2(15, 228));
notice_layer->addChild(notice_bg);
ClippingNode *clipper = ClippingNode::create();
Node *stencil = Sprite::createWithSpriteFrameName("notice_mask.png");
stencil->setAnchorPoint(Vec2(0, 0));
clipper->setStencil(stencil);
auto strings = CCDictionary::createWithContentsOfFile("Config/strings.xml");
auto reply = ((CCString *)strings->objectForKey("Notice"))->m_sString.c_str();
noticeLabel = CCLabelTTF::create(reply, FONT_NAME, 12);
noticeLabel->setAnchorPoint(Vec2(0, 0));
clipper->addChild(noticeLabel);
clipper->setPosition(Vec2(35, 228));
notice_layer->addChild(clipper);
addChild(notice_layer);
}
}
void StartMenu::onNewsBtn(CCObject *sender)
{
// Logika tombol berita dikosongkan secara aman
}
void StartMenu::onHardCoreOn(CCObject *sender)
{
SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
if (hardCoreLayer)
{
hardCoreLayer->removeAllChildren();
hardCoreLayer->removeFromParent();
hardCoreLayer = nullptr;
}
}
void StartMenu::onHardCoreOff(CCObject *sender)
{
SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/cancel.ogg");
if (hardCoreLayer)
{
hardCoreLayer->removeAllChildren();
hardCoreLayer->removeFromParent();
hardCoreLayer = nullptr;
}
}
void StartMenu::onHardLayerCallBack()
{
if (UserDefault::sharedUserDefault()->getBoolForKey("isHardCore") == false)
{
if (!hardCoreLayer)
{
hardCoreLayer = Layer::create();
Sprite *confirm_bg = Sprite::createWithSpriteFrameName("confirm_bg.png");
confirm_bg->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
Sprite *hardcore_title = Sprite::createWithSpriteFrameName("hardcore_title.png");
hardcore_title->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + 38));
Sprite *hardcore_text = Sprite::createWithSpriteFrameName("hardcore_text.png");
hardcore_text->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + 8));
MenuItem *yes_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("yes_btn1.png"), Sprite::createWithSpriteFrameName("yes_btn2.png"), this, menu_selector(StartMenu::onHardCoreOn));
MenuItem *no_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("no_btn1.png"), Sprite::createWithSpriteFrameName("no_btn2.png"), this, menu_selector(StartMenu::onHardCoreOff));
Menu *confirm_menu = Menu::create(yes_btn, no_btn, nullptr);
confirm_menu->alignItemsHorizontallyWithPadding(24);
confirm_menu->setPosition(Vec2(winSize.width / 2, winSize.height / 2 - 30));
hardCoreLayer->addChild(confirm_bg, 600);
hardCoreLayer->addChild(confirm_menu, 650);
hardCoreLayer->addChild(hardcore_title, 650);
hardCoreLayer->addChild(hardcore_text, 650);
addChild(hardCoreLayer, 700);
}
}
}
void StartMenu::onTrainingCallBack()
{
auto modeScene = Scene::create();
auto gameModeLayer = GameModeLayer::create();
modeScene->addChild(gameModeLayer);
Director::sharedDirector()->replaceScene(TransitionFade::create(1.25f, modeScene));
}
void StartMenu::onCreditsCallBack()
{
SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
Scene *creditsScene = Scene::create();
CreditsLayer *creditsLayer = CreditsLayer::create();
creditsScene->addChild(creditsLayer);
Director::sharedDirector()->replaceScene(TransitionFade::create(1.25f, creditsScene));
}
void StartMenu::scrollMenu(int posY)
{
if (posY > _pos02 || (isDrag && isClockwise))
{
for (auto menu : _menuArray)
{
if (menu->getPositionY() == _pos01)
{
auto spn = MoveTo::create(0.5, Vec2(105, _pos02));
reorderChild(menu, 1);
menu->_isBottom = true;
auto fo = FadeOut::create(0.3f);
auto seq = Spawn::createWithTwoActions(spn, fo);
menu->runAction(seq);
}
else if (menu->getPositionY() == _pos03)
{
auto spn = Spawn::createWithTwoActions(
MoveTo::create(0.5, Vec2(105, _pos02)),
ScaleTo::create(0.5, 1));
reorderChild(menu, 3);
menu->_isTop = true;
auto call = CallFunc::create(std::bind(&MenuButton::playSound, menu));
auto seq = newSequence(spn, call);
menu->runAction(seq);
}
else if (menu->getPositionY() == _pos02 && menu->_isBottom)
{
auto spn = MoveTo::create(0.5, Vec2(105, _pos03));
reorderChild(menu, 1);
menu->setVisible(true);
auto fi = FadeIn::create(0.3f);
auto seq = Spawn::createWithTwoActions(spn, fi);
menu->_isBottom = false;
menu->runAction(seq);
}
else if (menu->getPositionY() == _pos02 && !menu->_isBottom)
{
auto spn = Spawn::createWithTwoActions(
MoveTo::create(0.5, Vec2(105, _pos01)),
ScaleTo::create(0.5, 0.5));
reorderChild(menu, 1);
menu->_isTop = false;
menu->runAction(spn);
}
}
}
else
{
for (auto menu : _menuArray)
{
if (menu->getPositionY() == _pos01)
{
auto spn = Spawn::createWithTwoActions(
MoveTo::create(0.5, Vec2(105, _pos02)),
ScaleTo::create(0.5, 1));
reorderChild(menu, 3);
menu->_isTop = true;
auto call = CallFunc::create(std::bind(&MenuButton::playSound, menu));
auto seq = newSequence(spn, call);
menu->runAction(seq);
}
else if (menu->getPositionY() == _pos03)
{
auto spn = MoveTo::create(0.5, Vec2(105, _pos02));
reorderChild(menu, 1);
menu->_isBottom = true;
auto fo = FadeOut::create(0.3f);
auto seq = Spawn::createWithTwoActions(spn, fo);
menu->runAction(seq);
}
else if (menu->getPositionY() == _pos02 && menu->_isBottom)
{
auto spn = MoveTo::create(0.5, Vec2(105, _pos01));
reorderChild(menu, 2);
menu->setVisible(true);
auto fi = FadeIn::create(0.3f);
auto seq = Spawn::createWithTwoActions(spn, fi);
menu->_isBottom = false;
menu->runAction(seq);
}
else if (menu->getPositionY() == _pos02 && !menu->_isBottom)
{
auto spn = Spawn::createWithTwoActions(
MoveTo::create(0.5, Vec2(105, _pos03)),
ScaleTo::create(0.5, 0.5));
reorderChild(menu, 2);
menu->_isTop = false;
menu->runAction(spn);
}
}
}
std::string src;
for (auto menu : _menuArray)
{
if (menu->_isTop)
{
switch (menu->getBtnType())
{
case MenuButtonType::Training:
src = "menu02_text.png";
break;
case MenuButtonType::Custom:
src = "menu01_text.png";
break;
case MenuButtonType::Credits:
src = "menu04_text.png";
break;
case MenuButtonType::Exit:
src = "menu03_text.png";
break;
default:
break;
}
}
}
auto frame = getSpriteFrame(src);
if (frame && menuText) menuText->setDisplayFrame(frame);
}
void StartMenu::keyBackClicked()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
// Logika dialog keluar HP diserahkan ke sistem utama Android secara aman
Director::sharedDirector()->end();
exit(0);
#else
Director::sharedDirector()->end();
exit(0);
#endif
}
void StartMenu::setCheats(int cheats)
{
Cheats = cheats;
}
void StartMenu::onExitCallBack()
{
keyBackClicked();
}
void StartMenu::enterSelectLayer()
{
	// Membuat objek CCScene kosong secara aman sesuai standar v2
	cocos2d::CCScene* scene = cocos2d::CCScene::create();
	if (scene) {
		// Menginisialisasi objek SelectLayer
		SelectLayer* layer = SelectLayer::create();
		if (layer) {
			// Menempelkan layer ke dalam scene
			scene->addChild(layer);
			// Melakukan transisi scene melalui sharedDirector() bawaan v2
			cocos2d::CCDirector::sharedDirector()->replaceScene(scene);
		
	}
}


