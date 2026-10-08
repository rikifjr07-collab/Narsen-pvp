#pragma once

/**
 * Naruto Senki V2 Debug
 */

#if COCOS2D_DEBUG

#include "Utils/Cocos2dxHelper.hpp"
#include <string>
#include <sstream>
#include <iostream>

#define DEBUG_NS_BEGIN \
	namespace Debug    \
	{

#define DEBUG_NS_END \
	}

DEBUG_NS_BEGIN

#define LOG_CMD(color_code) "\033[1;" #color_code "m"

/* Style */
#define LOG_BRIGHT LOG_CMD(1)
#define LOG_DIM LOG_CMD(2)

/* Foreground colors */
#define LOG_RESET LOG_CMD(0)
#define LOG_BLACK LOG_CMD(30)
#define LOG_RED LOG_CMD(31)
#define LOG_GREEN LOG_CMD(32)
#define LOG_YELLOW LOG_CMD(33)
#define LOG_BLUE LOG_CMD(34)
#define LOG_PURPLE LOG_CMD(35)
#define LOG_CYAN LOG_CMD(36)
#define LOG_WHITE LOG_CMD(37)

/* Special commands */
#define LOG_ITALIC LOG_CMD(03)
#define LOG_UNDERLINE LOG_CMD(04)

/* Background colors */
#define LOG_BG_BLACK LOG_CMD(40)
#define LOG_BG_RED LOG_CMD(41)
#define LOG_BG_GREEN LOG_CMD(42)
#define LOG_BG_YELLOW LOG_CMD(43)
#define LOG_BG_BLUE LOG_CMD(44)
#define LOG_BG_PURPLE LOG_CMD(45)
#define LOG_BG_CYAN LOG_CMD(46)
#define LOG_BG_WHITE LOG_CMD(47)

#define LOG_END "\033[0m"

#define LOG_ERROR LOG_RED
#define LOG_WARN LOG_YELLOW
#define LOG_INFO LOG_GREEN

// === PERBAIKAN FINAL: Pengganti NSLOG Tanpa Library fmt Bawaan C++ Murni ===
inline void nslog_helper(std::ostringstream&) {}

template <typename T, typename... Args>
inline void nslog_helper(std::ostringstream& ss, T&& first, Args&&... args) {
    ss << std::forward<T>(first);
    nslog_helper(ss, std::forward<Args>(args)...);
}

template <typename... Args>
static inline void NSLOG(const char* _Fmt, Args&&... args) {
    std::ostringstream ss;
    nslog_helper(ss, std::forward<Args>(args)...);
    puts(ss.str().c_str());
}

#define LOG(fmt, ...) NSLOG("NS LOG: ", fmt, ##__VA_ARGS__)

DEBUG_NS_END

#endif
