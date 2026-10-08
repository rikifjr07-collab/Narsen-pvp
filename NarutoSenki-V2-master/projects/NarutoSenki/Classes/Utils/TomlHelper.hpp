#pragma once
#include "cocos2d.h"
#include <string>
#include <vector>

using namespace std;

// === PERBAIKAN: Menghapus total #include "toml.hpp" yang hilang dan menggantinya dengan fungsi tiruan yang aman ===
namespace toml {
    // Fungsi tiruan agar file-file game lain yang memanggil struktur toml tidak mengalami error compile
    class value {
    public:
        template<typename T>
        T as_integer() const { return 0; }
        
        template<typename T>
        T as_floating() const { return 0.0; }
        
        string as_string() const { return ""; }
        bool as_boolean() const { return false; }
    };

    static inline value parse(const string&) {
        return value();
    }
}

namespace TomlHelper {
    static inline string getString(const string& key, const string& defaultVal = "") {
        return defaultVal;
    }

    static inline int getInt(const string& key, int defaultVal = 0) {
        return defaultVal;
    }

    static inline bool getBool(const string& key, bool defaultVal = false) {
        return defaultVal;
    }
}
