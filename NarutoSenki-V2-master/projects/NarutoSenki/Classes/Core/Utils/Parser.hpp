#pragma once
#include "Defines.h"
#include "Data/UnitData.h"
#include "Utils/TomlHelper.hpp"

#define PARSER_NS_BEGIN  \
	namespace UnitParser \
	{
#define PARSER_NS_END }

PARSER_NS_BEGIN

struct ActionData
{
	using FrameVector = vector<pair<string, string>>;

	ActionFlag flag;
	bool hasData;

	string type;
	bool isDouble;
	u16 value;
	u16 rangeX;
	u16 rangeY;
	u16 cooldown;
	u16 combatPoint;

	ActionConstant::AnimationInfo info;
	FrameVector frames;
};

struct UnitMetadata
{
	string name;
	u32 hp;
	// u16 width;
	// u16 height;
	u16 speed;

	bool enableHPBar;
	// i16 hpBarX;
	i16 hpBarY;

	vector<ActionData> actions;

	bool getAction(ActionFlag flag, ActionData &action) noexcept
	{
		for (auto &actionData : actions)
		{
			if (actionData.flag == flag)
			{
				action = actionData;
				return true;
			}
		}
		return false;
	}
};

class ParserCache
{
private:
	static inline vector<UnitMetadata> _cache;

public:
	static inline void clear() noexcept
	{
		vector<UnitMetadata>().swap(_cache);
	}

	static inline void set(const UnitMetadata &data)
	{
		_cache.push_back(data);
	}

	static inline bool get(const string &name, UnitMetadata &metadata) noexcept
	{
		for (const auto &data : _cache)
		{
			if (data.name == name)
			{
				metadata = data;
				return true;
			}
		}
		return false;
	}
};

static const std::string nullstr;

static void ParseCoreData(const toml::value &v, UnitMetadata &metadata)
{
	if (!v.is_table()) return;
	
	// Menggunakan fungsi standar library toml bawaan untuk menghindari error try_set_or
	metadata.name = toml::find_or<string>(v, "name", "");
	metadata.hp = toml::find_or<u32>(v, "hp", 0);
	
	if (v.contains("hpBar"))
	{
		metadata.enableHPBar = true;
		const auto &hpBar = toml::find(v, "hpBar");
		metadata.hpBarY = toml::find_or<i16>(hpBar, "y", 0);
	}
	else
	{
		metadata.enableHPBar = false;
	}
	metadata.speed = toml::find_or<u16>(v, "speed", (u16)kDefaultSpeed);
}

static void ParseAction(const toml::value &v, UnitMetadata &metadata)
{
	if (!v.is_table()) return;

	for (const auto &[key, value] : v.as_table())
	{
		if (key.empty() || !value.is_table() || value.size() == 0)
			continue;
		auto flag = ActionConstant::string2ActionFlag(key);
		if (flag == ActionFlag::None)
			continue;

		ActionData data = {
			.flag = flag,
		};
		
		data.type = toml::find_or<string>(value, "type", "");
		data.value = toml::find_or<u16>(value, "value", 0);
		data.rangeX = toml::find_or<u16>(value, "rangeX", 0);
		data.rangeY = toml::find_or<u16>(value, "rangeY", 0);
		data.cooldown = toml::find_or<u16>(value, "cd", 0);
		data.combatPoint = toml::find_or<u16>(value, "combatPoint", 0);
        
		const auto &defaultInfo = ActionConstant::getAnimDataByActionFlag(data.flag);
		if (value.contains("anim"))
		{
			const auto &anim = toml::find(value, "anim");
			data.info.fps = toml::find_or<uint8_t>(anim, "fps", defaultInfo.fps);
			data.info.isLoop = toml::find_or<bool>(anim, "loop", defaultInfo.isLoop);
			data.info.isReturnToIdle = toml::find_or<bool>(anim, "returnIdle", defaultInfo.isReturnToIdle);
		}
		else
		{
			data.info = defaultInfo;
		}

		// Alternatif pembacaan array frame yang jauh lebih aman bagi compiler
		if (value.contains("frame"))
		{
			const auto &frameArray = toml::find<vector<toml::value>>(value, "frame");
			auto &frameVector = data.frames;
			frameVector.resize(frameArray.size());
			size_t i = 0;
			for (const auto &fVal : frameArray)
			{
				std::string f = fVal.as_string();
				auto found = f.find_first_of('=', 1);
				if (found != string::npos)
				{
					auto eventName = tomlex::trim(f.substr(0, found));
					frameVector[i++] = make_pair(eventName, f);
				}
				else
				{
					frameVector[i++] = make_pair(nullstr, f);
				}
			}
		}
		metadata.actions.push_back(data);
	}
}

static inline bool fromCache(const string &unitName, UnitMetadata &metadata)
{
	return ParserCache::get(unitName, metadata);
}

static inline UnitMetadata fromToml(const string &fname, bool cache = false)
{
	const auto path = FileUtils::sharedFileUtils()->fullPathForFilename(fname.c_str());
	if (!FileUtils::sharedFileUtils()->isFileExist(path))
	{
		CCLOGERROR("[ERROR] UnitParser::fromToml not found file %s", path.c_str());
		return {};
	}
	
	const auto data = toml::parse(path);

	UnitMetadata metadata;
	ParseCoreData(data, metadata);
	ParseAction(data, metadata);
	if (cache)
		ParserCache::set(metadata);
	return metadata;
}

PARSER_NS_END

#undef PARSER_NS_BEGIN
#undef PARSER_NS_END
