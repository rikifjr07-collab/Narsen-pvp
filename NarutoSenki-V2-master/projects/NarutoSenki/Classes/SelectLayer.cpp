#include <time.h>
#include "StartMenu.h"
#include "Constants/UiFlowKeys.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "../../../cocos2dx/platform/android/jni/JniHelper.h"
#endif

SelectLayer::~SelectLayer()
{
	CC_SAFE_RELEASE(_selectList);

	auto handler = getGameModeHandler();
	handler->selectLayer = nullptr;
	if (!isStart)
		handler->resetCheats();
}

void SelectLayer::onGameStart()
{
    CCLOG("Bypass onGameStart Biner: Membersihkan RAM dan memicu Scene Baru...");

    // 1. Bersihkan Cache Gambar agar RAM HP bersih dan tidak FC di tombol Start
    Director::sharedDirector()->purgeCachedData();

    // 2. Buat wadah Scene baru
    Scene *loadScene = Scene::create();
    if (loadScene == nullptr) {
        CCLOG("ERROR: Gagal mengalokasikan Scene!");
        return;
    }

    // 3. JALUR PENYELAMAT: Lewati LoadLayer bawaan jika sering memicu crash,
    // langsung buat GameLayer (arena pertandingan) secara bersih!
    GameLayer *gameLayer = GameLayer::create();
    if (gameLayer != nullptr) {
        loadScene->addChild(gameLayer);
        
        // Eksekusi perpindahan scene dengan efek pudar 1 detik penuh
        Director::sharedDirector()->replaceScene(TransitionFade::create(1.0f, loadScene));
        CCLOG("Bypass Sukses: Langsung melompat masuk ke arena pertandingan!");
    }
    else {
        // Fallback jika GameLayer bermasalah, gunakan LoadLayer standar tanpa pemicu handler
        LoadLayer *loadLayer = LoadLayer::create();
        if (loadLayer != nullptr) {
            loadScene->addChild(loadLayer);
            Director::sharedDirector()->replaceScene(TransitionFade::create(1.0f, loadScene));
            CCLOG("Fallback Sukses: Masuk ke LoadLayer standar.");
        }
    }
}


void SelectLayer::onExitTransitionDidStart()
{
	Layer::onExitTransitionDidStart();
	setKeypadEnabled(false);
}

void SelectLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	setKeypadEnabled(true);
}

void SelectLayer::keyBackClicked()
{
	setKeypadEnabled(false);

	lua_call_func(UiFlowKeys::kBackToStartMenu);
}
