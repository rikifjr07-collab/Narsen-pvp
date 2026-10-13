#pragma once
#include "GameMode/IGameModeHandler.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
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
    bool isConnected;

    // FUNGSI UTAS LATAR BELAKANG: Mengurus jaringan terpisah agar game tidak diam/freeze
    void startNetworkThread() {
        std::thread([this]() {
            extern std::string global_ipAddress;
            if (isHost) {
                serverSocket = socket(AF_INET, SOCK_STREAM, 0);
                if (serverSocket >= 0) {
                    int opt = 1;
                    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
                    serverAddr.sin_family = AF_INET;
                    serverAddr.sin_port = htons(8888); 
                    serverAddr.sin_addr.s_addr = INADDR_ANY; 
                    
                    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) >= 0) {
                        listen(serverSocket, 1);
                        struct sockaddr_in clientAddr;
                        socklen_t clientLen = sizeof(clientAddr);
                        // accept() berjalan di background, tidak akan mengunci layar start game!
                        int incomingSec = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
                        if (incomingSec >= 0) {
                            clientSocket = incomingSec;
                            fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                            isConnected = true;
                        }
                    }
                }
            } else {
                clientSocket = socket(AF_INET, SOCK_STREAM, 0);
                if (clientSocket >= 0) {
                    struct sockaddr_in clientServerAddr;
                    clientServerAddr.sin_family = AF_INET;
                    clientServerAddr.sin_port = htons(8888);
                    clientServerAddr.sin_addr.s_addr = inet_addr(global_ipAddress.c_str()); 
                    
                    // Mencoba terhubung tanpa membuat game stuck
                    if (connect(clientSocket, (struct sockaddr*)&clientServerAddr, sizeof(clientServerAddr)) >= 0) {
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                        isConnected = true;
                    }
                }
            }
        }).detach(); // Melepas utas ke latar belakang secara mandiri
    }

public:
    void init()
    {
        CCLOG("Enter 1 VS 1 mode dengan Isolasi Thread.");
        isAddCallback = false;
        isConnected = false;
        clientSocket = -1;
        serverSocket = -1;
        gd.isHardCore = true;

        extern bool global_isHost; 
        isHost = global_isHost;    

        // LANGSUNG LEMPAR KE BACKGROUND THREAD! 
        // Ini memastikan tombol start langsung memicu LoadLayer seketika!
        startNetworkThread();
    }

    void onInitHeros() { initHeros(1, 1); }
    void onGameStart() {}
    void onGameOver() {
        if (serverSocket >= 0) close(serverSocket);
        if (clientSocket >= 0) close(clientSocket);
    }

    void sendMyPosition(float x, float y, int anim) {
        if (clientSocket < 0 || !isConnected) return;
        PlayerDataPacket packet = { x, y, anim };
        send(clientSocket, &packet, sizeof(packet), MSG_NOSIGNAL);
    }
    void receiveFriendPosition(CharacterBase* friendChar) {
        if (clientSocket < 0 || !isConnected || !friendChar) return;
        PlayerDataPacket receivedPacket;
        int bytesRead = recv(clientSocket, &receivedPacket, sizeof(receivedPacket), MSG_DONTWAIT);
        if (bytesRead > 0) {
            float currentX = friendChar->getPositionX();
            float currentY = friendChar->getPositionY();
            float smoothX = currentX + (receivedPacket.posX - currentX) * 0.3f;
            float smoothY = currentY + (receivedPacket.posY - currentY) * 0.3f;
            friendChar->setPosition(ccp(smoothX, smoothY));
        }
    }

    void update(float dt) {
        auto layer = getGameLayer();
        if (!layer || !isConnected) return;

        CharacterBase* myHero = nullptr;
        if (layer->_CharacterArray.size() > 0) {
            auto firstChar = layer->_CharacterArray.at(0);
            if (firstChar && firstChar->isPlayer()) myHero = firstChar;
        }
        if (myHero) sendMyPosition(myHero->getPositionX(), myHero->getPositionY(), 0);

        if (layer->_CharacterArray.size() > 1) {
            auto friendHero = layer->_CharacterArray.at(1);
            if (friendHero && !friendHero->isPlayer()) receiveFriendPosition(friendHero);
        }
    }

    void onCharacterInit(CharacterBase *c) {
        if (!isAddCallback && getGameLayer() && !getGameLayer()->isHUDInit()) {
            isAddCallback = true;
            auto layer = getGameLayer();
            layer->onHUDInitialized([layer]() {
                if (!layer) return;
                for (auto hero : layer->_CharacterArray) {
                    hero->setCoin(3000);
                    hero->setEXP(2500);
                    for (int i = 1; i < 6; i++) hero->changeHPbar();
                    hero->setHPValue(hero->getMaxHP());
                    hero->increaseAllCkrs(25000);
                    hero->setRebornTime(10);
                    if (hero->isPlayer()) {
                        layer->getHudLayer()->setEXPLose();
                        layer->getHudLayer()->coinLabel->setString(to_cstr(hero->getCoin()));
                        if (!hero->isEnableSkill04()) layer->getHudLayer()->skill4Button->setLock();
                        if (!hero->isEnableSkill05()) layer->getHudLayer()->skill5Button->setLock();
                    }
                }
            });
        }
    }

    void onCharacterDead(CharacterBase *c) {}
    void onCharacterReborn(CharacterBase *c) {}
};
