APP_ABI := armeabi-v7a
APP_STL := c++_shared
NDK_TOOLCHAIN_VERSION := 4.9

APP_CPPFLAGS :=  -std=c++2a -frtti -fsigned-char -fexceptions
APP_LDFLAGS := -latomic

APP_SHORT_COMMANDS := true

ifeq ($(NDK_DEBUG),1)
  APP_CPPFLAGS += -DNDEBUG -DCOCOS2D_DEBUG=1
else
  APP_CPPFLAGS += -DCOCOS2D_DEBUG=0
endif
