#pragma once
#include "GameMode/IGameModeHandler.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

struct PlayerDataPacket {
    float posX;      
    float posY;      
    int currentAnim; 
};

class Mode1v1 : public IGameModeHandler
{
private:
  bool isHost;          
  int serverSocket;     
  int clientSocket;     
  struct sockaddr_in serverAddr;
  bool isAddCallback;

public:
    void init()
    {
        CCLOG("Enter 1 VS 1 mode.");
        isAddCallback = false;
        gd.isHardCore = true;

        extern bool global_isHost; 
        extern std::string global_ipAddress;
        isHost = global_isHost;    

        if (isHost) {
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(8888); 
            serverAddr.sin_addr.s_addr = INADDR_ANY; 

            bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            listen(serverSocket, 1);
        }
        else {
            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            
            struct sockaddr_in clientServerAddr;
            clientServerAddr.sin_family = AF_INET;
            clientServerAddr.sin_port = htons(8888);
            clientServerAddr.sin_addr.s_addr = inet_addr(global_ipAddress.c_str()); 
            
            connect(clientSocket, (struct sockaddr*)&clientServerAddr, sizeof(clientServerAddr));
        }
    }

	void onInitHeros()
	{
		initHeros(1, 1);
	}

	void onGameStart()
	{
	}

	void onGameOver()
	{
	}

    void sendMyPosition(float x, float y, int anim) {
        PlayerDataPacket packet = { x, y, anim };
        send(clientSocket, &packet, sizeof(packet), 0);
    }

    void receiveFriendPosition(CharacterBase* friendChar) {
        PlayerDataPacket receivedPacket;
        int bytesRead = recv(clientSocket, &receivedPacket, sizeof(receivedPacket), 0);
        
        if (bytesRead > 0) {
            float currentX = friendChar->getPositionX();
            float currentY = friendChar->getPositionY();
            
            float smoothX = currentX + (receivedPacket.posX - currentX) * 0.3f;
            float smoothY = currentY + (receivedPacket.posY - currentY) * 0.3f;
            
            // PERBAIKAN: Dibungkus ke ccp() agar kompatibel dengan Cocos2d-x lama
            friendChar->setPosition(ccp(smoothX, smoothY));
        }
    }

    void update(float dt) {
        auto layer = getGameLayer();
        if (!layer) return;

        // PERBAIKAN: Membaca instance Hero dari array karakter indeks ke-0 jika getHero() tidak ada
        CharacterBase* myHero = nullptr;
        if (layer->_CharacterArray.size() > 0) {
            auto firstChar = layer->_CharacterArray.at(0);
            if (firstChar && firstChar->isPlayer()) {
                myHero = firstChar;
            }
        }

        if (myHero) {
            sendMyPosition(myHero->getPositionX(), myHero->getPositionY(), 0);
        }

        if (layer->_CharacterArray.size() > 1) {
            auto friendHero = layer->_CharacterArray.at(1);
            if (friendHero && !friendHero->isPlayer()) {
                receiveFriendPosition(friendHero);
            }
        }
    }

	void onCharacterInit(CharacterBase *c)
	{
		if (!isAddCallback && !getGameLayer()->isHUDInit())
		{
			isAddCallback = true;
			auto layer = getGameLayer();
			layer->onHUDInitialized(
				[layer]()
				{
					if (!layer)
						return;
					for (auto hero : layer->_CharacterArray)
					{
						hero->setCoin(3000);
						hero->setEXP(2500);
						for (int i = 1; i < 6; i++)
							hero->changeHPbar();
						hero->setHPValue(hero->getMaxHP());
						hero->increaseAllCkrs(25000);
						hero->setRebornTime(10);

						if (hero->isPlayer())
						{
							layer->getHudLayer()->setEXPLose();
							layer->getHudLayer()->coinLabel->setString(to_cstr(hero->getCoin()));
							if (!hero->isEnableSkill04())
								layer->getHudLayer()->skill4Button->setLock();
							if (!hero->isEnableSkill05())
								layer->getHudLayer()->skill5Button->setLock();
						}
					}
				});
		}
	}

	void onCharacterDead(CharacterBase *c)
	{
	}

	void onCharacterReborn(CharacterBase *c)
	{
	}
};
