#pragma once
#include "GameMode/IGameModeHandler.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <string>
#include <vector>

struct PlayerDataPacket {
    float posX;      // Koordinat X karakter
    float posY;      // Koordinat Y karakter
    int currentAnim; // Status animasi/gerakan ninja
};

class Mode1v1 : public IGameModeHandler
{
private:
    bool isHost;          // true jika HP ini yang buat room, false jika join
    int serverSocket;     // socket utama untuk server
    int clientSocket;     // socket untuk menghubungkan dua HP
    struct sockaddr_in serverAddr;
    bool isAddCallback;

public:
    void init()
    {
        CCLOG("Enter 1 VS 1 mode.");
        isAddCallback = false;
        gd.isHardCore = true;

        // --- PONDASI MULTIPLAYER LOKAL ---
        extern bool global_isHost; 
        extern std::string global_ipAddress;
        isHost = global_isHost;    

        if (isHost) {
            // 1. Buat Socket TCP
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(8888); 
            serverAddr.sin_addr.s_addr = INADDR_ANY; 

            // 2. Ikat socket ke port
            bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            
            // 3. Tunggu HP teman (Client) masuk ke hotspot
            listen(serverSocket, 1);
        } 
        else {
            // --- LOGIKA CLIENT MENEMBAK IP MANUAL ---
            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            
            struct sockaddr_in remoteAddr;
            remoteAddr.sin_family = AF_INET;
            remoteAddr.sin_port = htons(8888);
            remoteAddr.sin_addr.s_addr = inet_addr(global_ipAddress.c_str()); 
            
            // Tembak koneksi!
            connect(clientSocket, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
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

    // Fungsi untuk mengirim posisi kita ke HP teman
    void sendMyPosition(float x, float y, int anim) {
        PlayerDataPacket packet = { x, y, anim };
        send(clientSocket, &packet, sizeof(packet), 0);
    }

    // Fungsi untuk menerima posisi teman
    void receiveFriendPosition(CharacterBase* friendChar) {
        PlayerDataPacket receivedPacket;
        int bytesRead = recv(clientSocket, &receivedPacket, sizeof(receivedPacket), 0);
        
        if (bytesRead > 0) {
            float currentX = friendChar->getPositionX();
            float currentY = friendChar->getPositionY();
            
            float smoothX = currentX + (receivedPacket.posX - currentX) * 0.3f;
            float smoothY = currentY + (receivedPacket.posY - currentY) * 0.3f;
            
            // PERBAIKAN MUTLAK: Penulisan namespace cocos2d::Vec2 yang valid dan benar
            friendChar->setPosition(cocos2d::Vec2(smoothX, smoothY));
        }
    }

    void update(float dt) {
        auto layer = getGameLayer();
        if (!layer) return;

        // PERBAIKAN MUTLAK: Menggunakan getHero1() sesuai fungsi internal GameLayer asli milikmu
        auto myHero = layer->getHero1(); 
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
