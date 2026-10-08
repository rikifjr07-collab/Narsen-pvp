#pragma once
#include "Utils/Debug/Debug.h"
#include <string>
#include <sstream>
#include <iomanip>

#if COCOS2D_DEBUG

DEBUG_NS_BEGIN

#include "CharacterBase.h"

// Fungsi pembantu untuk membuat teks rata kiri dengan lebar tertentu (menggantikan fungsi {:<9} milik fmt)
static inline std::string rataKiri(const std::string& teks, int lebar) {
    std::ostringstream ss;
    ss << std::left << std::setw(lebar) << teks;
    return ss.str();
}

static inline std::string rataKiriInt(int nilai, int lebar) {
    std::ostringstream ss;
    ss << std::left << std::setw(lebar) << nilai;
    return ss.str();
}

void PrintDamgeInfo(CharacterBase *attacked, CharacterBase *realAttacker, CharacterBase *attacker, int attackValue, uint32_t damage, const string &effectType)
{
	if (attacked->isFlog() || attacker->isFlog())
		return;

	auto state = damage < attacked->getHP() ? "attacked" : "killed  ";
	
	std::ostringstream logStream;
	
	if (realAttacker != attacker)
	{
		logStream << "Unit " << rataKiri(realAttacker->getName(), 9) << ":" << realAttacker->m_uID
		          << " -> " << rataKiri(attacker->getName(), 9) << ":" << attacker->getCharId()
		          << " " << state << " " << rataKiri(attacked->getName(), 9) << ":" << attacked->getCharId()
		          << " HP: " << rataKiriInt(attacked->getHP(), 4) << " -" << rataKiriInt(damage, 4)
		          << "HP [ Atk: " << rataKiriInt(attackValue, 4) << ", Type: " << effectType << "]";
		          
		CCLOG("%s", logStream.str().c_str());
	}
	else
	{
		logStream << "Unit " << rataKiri(attacker->getName(), 9) << ":" << attacker->getCharId()
		          << " " << state << " " << rataKiri(attacked->getName(), 9) << ":" << attacked->getCharId()
		          << " HP: " << rataKiriInt(attacked->getHP(), 4) << " -" << rataKiriInt(damage, 4)
		          << "HP [ Atk: " << rataKiriInt(attackValue, 4) << ", Type: " << effectType << "]";
		          
		CCLOG("%s", logStream.str().c_str());
	}
}

DEBUG_NS_END

#endif
