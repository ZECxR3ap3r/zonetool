// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

#include <Windows.h>

#include <initializer_list>
#include <functional>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <memory>
#include <vector>
#include <mutex>
#include <map>

#include "Json.hpp"
using Json = nlohmann::json;
using Orderer_Json = nlohmann::ordered_json;

#undef xor
#undef and

#define MAX_ZONE_SIZE 1024 * 1024 * 200

namespace ZoneTool
{
#pragma push(pack, 1)
	struct XFileHeader
	{
		char header[8];
		std::int32_t version;
		std::uint8_t allowOnlineUpdate;
		std::uint32_t dwHighDateTime;
		std::uint32_t dwLowDateTime;
	};

	template <std::size_t num_streams>
	struct XZoneMemory
	{
		std::uint32_t size;
		std::uint32_t externalsize;
		std::uint32_t streams[num_streams];
	};

	struct XAssetStreamFile
	{
		unsigned int fileIndex;
		unsigned int offset;
		unsigned int offsetEnd;
	};

	struct XZoneInfo
	{
		const char* zone;
		std::int32_t loadFlags;
		std::int32_t unloadFlags;
	};

	struct XZoneInfoInternal
	{
		char name[64];
		int flags;
		int isBaseMap;
	};

	struct ScriptStringList
	{
		int count;
		const char** strings;
	};

	struct XAssetList
	{
		ScriptStringList stringList;
		int assetCount;
		void* assets;
	};

	enum class zone_target
	{
		pc,
		xbox360,
		ps3,
	};
	enum class zone_target_version
	{
		iw3_alpha_253,
		iw3_alpha_290,
		iw3_alpha_328,
		iw4_alpha_482,
		iw4_alpha_491,
		iw4_release,
		iw4_release_console,
		iw5_release,
		max,
	};

	static std::string zone_target_version_str[8] = 
	{
		"iw3_alpha_253",
		"iw3_alpha_290",
		"iw3_alpha_328",
		"iw4_alpha_482",
		"iw4_alpha_491",
		"iw4_release",
		"iw4_release_console",
		"iw5_release",
	};
	
	static void endian_convert(void* data, const std::size_t size)
	{
		if (size <= 0)
		{
			return;
		}
		
		// clone data
		const auto data_clone = new char[size];
		memcpy(data_clone, data, size);

		// prepare pointers for magic
		const auto data_clone_ptr = reinterpret_cast<std::uint8_t*>(data_clone);
		const auto data_ptr = reinterpret_cast<std::uint8_t*>(data);

		for (auto i = 0u; i < size; i++)
		{
			data_ptr[i] = data_clone_ptr[size - i - 1];
		}

		delete[] data_clone;
	}
	template <typename T> static void endian_convert(T* data)
	{
		return endian_convert((void*)data, sizeof T);
	}
#pragma push(pop)
}

#include "IPatch.hpp"
#include "CSV.hpp"
#include "Utils/Swizzle.hpp"
#include "Zone/ZoneMemory.hpp"
#include "Zone/ZoneBuffer.hpp"
#include "Utils/PakFile.hpp"
#include "Zone/Zone.hpp"
#include "IAsset.hpp"
#include "Utils/FileReader.hpp"
#include "Utils/FileSystem.hpp"
#include "Utils/Function.hpp"
#include "Utils/Memory.hpp"
#include "Utils/BinaryDumper.hpp"
#include "Utils/Expressions.hpp"
#include "Linker.hpp"

#define MAKE_STRING(__data__) #__data__

#define ASSET_SIZE(__size__) \
	void _assert_size() \
	{ \
		static_assert(sizeof(*this) == __size__, __FUNCTION__": Invalid struct size.\n"); \
	}

// Simple helper to set console color
inline void SetColor(WORD color)
{
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

inline int GetColorByContent(const char* fmt, int defaultColor) {
	std::string s(fmt);

	if (s.find("ISound") != std::string::npos) return 10;
	if (s.find("ITechset") != std::string::npos) return 11;
	if (s.find("IMaterial") != std::string::npos) return 13;
	if (s.find("IXModel") != std::string::npos) return 3;
	if (s.find("IXAnimParts") != std::string::npos) return 2;
	if (s.find("IFxEffectDef") != std::string::npos) return 6;
	if (s.find("Weaponfield") != std::string::npos ||
		s.find("IXSurface") != std::string::npos) return 8;
	if (s.find("IStringTable") != std::string::npos) return 14;
	if (s.find("UI_ParseMenuInternal") != std::string::npos) return 9;

	return defaultColor;
}

inline const char* CleanFuncName(const char* name) {
	if (strncmp(name, "ZoneTool::IW5::", 15) == 0) return name + 15;
	return name;
}

#define ZONETOOL_LOG(color, tag, __FMT__, ...) \
    do { \
        SetColor(GetColorByContent(__FUNCTION__, color)); \
        printf("[%-11s] | %-25s | " __FMT__ "\n", tag, CleanFuncName(__FUNCTION__), __VA_ARGS__); \
        SetColor(15); \
    } while(0)

// Standardizing the calls
#define ZONETOOL_INFO(__FMT__, ...)        ZONETOOL_LOG(15, "INFO", __FMT__, __VA_ARGS__)
#define ZONETOOL_WARNING(__FMT__, ...)     ZONETOOL_LOG(14, "WARNING", __FMT__, __VA_ARGS__)
#define ZONETOOL_WEAPONINFO(__FMT__, ...)  ZONETOOL_LOG(9,  "WEAPONFIELD", __FMT__, __VA_ARGS__)
#define ZONETOOL_ERROR(__FMT__, ...)       ZONETOOL_LOG(79,  "ERROR", __FMT__, __VA_ARGS__)
#define ZONETOOL_FATAL(__FMT__, ...)       ZONETOOL_LOG(79,  "FATAL", __FMT__, __VA_ARGS__)

/*
 *	Debugging purposes
 */

// #define FILEPOINTERS_DEBUG
#ifdef FILEPOINTERS_DEBUG
#define START_LOG_STREAM \
		auto streamStartPos = buf->get_stream_pos();

#define END_LOG_STREAM \
		streamStartPos = buf->get_stream_pos() - streamStartPos; \
		ZONETOOL_INFO("Streamsize consumed for asset is %u", streamStartPos);
#else
#define START_LOG_STREAM
#define END_LOG_STREAM
#endif

template <typename T1, typename T2>
std::size_t Difference(const T1& t1, const T2& t2)
{
	return std::uintptr_t(t1) - std::uintptr_t(t2);
}

template <typename ... Args>
std::string va(const std::string& format, Args ... args)
{
	size_t size = _snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::vector<char> buf;
	buf.resize(size);
	_snprintf(buf.data(), size, format.c_str(), args ...);
	return std::string(buf.data(), buf.data() + size - 1);
}

static std::vector<std::string> split(const std::string& rawInput, const std::vector<char>& delims)
{
	std::vector<std::string> strings;

	auto findFirstDelim = [](const std::string& input, const std::vector<char>& delims) -> std::pair<char, std::size_t>
	{
		auto firstDelim = 0;
		auto firstDelimIndex = static_cast<std::size_t>(-1);
		auto index = 0u;

		for (auto& delim : delims)
		{
			if ((index = input.find(delim)) != std::string::npos)
			{
				if (firstDelimIndex == -1 || index < firstDelimIndex)
				{
					firstDelim = delim;
					firstDelimIndex = index;
				}
			}
		}

		return {firstDelim, firstDelimIndex};
	};

	std::string input = rawInput;

	while (!input.empty())
	{
		auto splitDelim = findFirstDelim(input, delims);
		if (splitDelim.first != 0)
		{
			strings.push_back(input.substr(0, splitDelim.second));
			input = input.substr(splitDelim.second + 1);
		}
		else
		{
			break;
		}
	}

	strings.push_back(input);
	return strings;
}

static std::vector<std::string> split(const std::string& str, char delimiter)
{
	return split(str, std::vector<char>({delimiter}));
}

template <typename T>
static std::shared_ptr<T> RegisterPatch()
{
	return std::make_shared<T>();
}

class CSV
{
public:
	CSV(const CSV& csv) : columns(csv.columns)
	{
	}

	CSV(const std::string& table)
	{
		std::ifstream iFile(table);

		if (iFile.is_open())
		{
			while (!iFile.eof())
			{
				std::string row;
				iFile >> row;

				auto cols = split(row, ',');
				columns.push_back(cols);
			}

			iFile.close();
		}
	}

	std::string Get(const std::uint32_t& row, const std::uint32_t& column)
	{
		// if (columns.empty()) return "";
		// if (columns.size() <= row) return "";
		// if (columns[column].size() <= column) return "";

		return columns[row][column];
	}

	std::uint32_t Rows()
	{
		return columns.size();
	}

	std::uint32_t Columns(std::uint32_t row = 0)
	{
		if (columns.empty()) return 0;
		return columns[row].size();
	}

private:
	std::vector<std::vector<std::string>> columns;
};
