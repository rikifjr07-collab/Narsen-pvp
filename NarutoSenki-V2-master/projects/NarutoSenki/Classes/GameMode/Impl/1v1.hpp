#pragma once
#include "GameMode/IGameModeHandler.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
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
        // Contoh: Kita set HP ini sebagai Host secara default untuk tes
        extern bool global_isHost; // Mengambil sakelar dari GameModeLayer
        extern std::string global_ipAddress;
        isHost = global_isHost;    // Otomatis mengikuti pilihan tombol menu!


        if (isHost) {
            // 1. Buat Socket TCP
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(8888); // Port game multiplayer kita
            serverAddr.sin_addr.s_addr = INADDR_ANY; // Menerima koneksi hotspot

            // 2. Ikat socket ke port
            bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            
            // 3. Tunggu HP teman (Client) masuk ke hotspot
            listen(serverSocket, 1);
            
            // Catatan: Di dunia nyata, fungsi accept() harus ditaruh di dalam thread terpisah
            // agar layar HP tidak membeku (freeze) saat menunggu teman masuk.
         else {
            // --- LOGIKA CLIENT MENEMBAK IP MANUAL ---
            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            
            struct sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(8888);
            
            // Konversi teks string IP manual menjadi alamat jaringan biner
            serverAddr.sin_addr.s_addr = inet_addr(global_ipAddress.c_str()); 
            
            // Tembak koneksi!
            connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
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
        // Kirim paket data lewat socket jaringan
        send(clientSocket, &packet, sizeof(packet), 0);
    }

    // Fungsi untuk menerima posisi teman (Biar tidak teleport)
    void receiveFriendPosition(CharacterBase* friendChar) {
        PlayerDataPacket receivedPacket;
        int bytesRead = recv(clientSocket, &receivedPacket, sizeof(receivedPacket), 0);
        
        if (bytesRead > 0) {
            // --- RUMUS INTERPOLASI (SMOOTHING) AGAR TIDAK TELEPORT ---
            // Kita tidak langsung set koordinat secara ekstrem, tapi digeser halus (Lerp)
            float currentX = friendChar->getPositionX();
            float currentY = friendChar->getPositionY();
            
            // Rumus: Posisi Baru = Posisi Sekarang + (Selisih Posisi Target * Kecepatan Geser)
            float smoothX = currentX + (receivedPacket.posX - currentX) * 0.3f;
            float smoothY = currentY + (receivedPacket.posY - currentY) * 0.3f;
            
            // Terapkan posisi halus ke karakter teman di layar kita
            friendChar->setPosition(smoothX, smoothY);
        }
    }
    void update(float dt) {
        auto layer = getGameLayer();
        if (!layer) return;

        // 1. Ambil data karakter utama kita (Player 1)
        auto myHero = layer->getHero(); 
        if (myHero) {
            // Kirim posisi koordinat X, Y, dan status animasi kita ke HP teman
            sendMyPosition(myHero->getPositionX(), myHero->getPositionY(), 0);
        }

        // 2. Ambil data karakter teman (Player 2)
        // Catatan: Indeks karakter teman di dalam array biasanya bergantung pada room
        if (layer->_CharacterArray.size() > 1) {
            auto friendHero = layer->_CharacterArray.at(1);
            if (friendHero && !friendHero->isPlayer()) {
                // Jalankan fungsi smoothing agar gerakan karakter teman mulus di layar kita
                receiveFriendPosition(friendHero);
            }
        }
    }

	void onCharacterInit(CharacterBase *c)
	{
		if (!isAddCallback && !getGameLayer()->isHUDInit())
		{
			// NOTE: Following code won't get correct value
			// Because this function `onCharacterInit` is not in current stack
			// So this value always nullptr or wrong value
			//
			// getGameLayer()->onHUDInitialized(
			// 	[&c]()
			// 	{
			// 		c->setEXP(2500);
			// 		for (int i = 1; i < 6; i++)
			// 			c->changeHPbar();
			// 		c->increaseAllCkrs(50000);
			// 	});
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
