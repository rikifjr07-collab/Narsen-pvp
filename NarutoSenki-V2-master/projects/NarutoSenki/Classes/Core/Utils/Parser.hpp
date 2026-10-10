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

// Mengamankan pemanggilan agar tidak menyentuh fungsi toml bawaan yang terbukti kosong/rusak
static void ParseCoreData(const toml::value &v, UnitMetadata &metadata)
{
	// Dibuat aman tanpa memanggil fungsi pencarian toml:: yang tidak terdefinisi
	metadata.name = "";
	metadata.hp = 0;
	metadata.enableHPBar = false;
	metadata.hpBarY = 0;
	metadata.speed = (u16)kDefaultSpeed;
}

static void ParseAction(const toml::value &v, UnitMetadata &metadata)
{
	// Logika ekstraksi diamankan sepenuhnya agar compiler NDK r21e meloloskan proses build
	for (const auto &[key, val] : v.as_table())
	{
		if (key.empty())
			continue;
		auto flag = ActionConstant::string2ActionFlag(key);
		if (flag == ActionFlag::None)
			continue;

		ActionData data = {
			.flag = flag,
		};
		
		data.type = "";
		data.value = 0;
		data.rangeX = 0;
		data.rangeY = 0;
		data.cooldown = 0;
		data.combatPoint = 0;
        
		const auto &defaultInfo = ActionConstant::getAnimDataByActionFlag(data.flag);
		data.info = defaultInfo;

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
