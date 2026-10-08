#pragma once
#include "cocos2d.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace toml {
    class value {
    public:
        template<typename T>
        T as_integer() const { return 0; }
        
        template<typename T>
        T as_floating() const { return 0.0; }
        
        string as_string() const { return ""; }
        bool as_boolean() const { return false; }

        // Mengatasi error: no member named 'as_table'
        const std::map<string, value>& as_table() const {
            static std::map<string, value> dummy_table;
            return dummy_table;
        }
    };

    static inline value parse(const string&) {
        return value();
    }
}

// Mengatasi semua error 'no member named try_set_or / try_set' di namespace tomlex/toml
namespace tomlex {
    template<typename T, typename U>
    static inline void try_set_or(const T&, const char*, U& val, const U&) {}

    template<typename T, typename U>
    static inline void try_set_or16(const T&, const char*, U& val, const U&) {}

    template<typename T, typename U>
    static inline void try_set(const T&, const char*, U& val) {}

    template<typename T, typename U>
    static inline void try_setitab(const T&, const char*, U& val) {}
}

namespace toml {
    using namespace tomlex;
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
