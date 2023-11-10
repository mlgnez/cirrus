#pragma once


#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.h"
#include <mutex>
#include <filesystem>
#include <d3d10_1.h>
#include <d3d10.h>
#include <tchar.h>
#include <iostream>
#include <unordered_map>
#include <tuple>
#include <chrono>
#include <optional>
#include <fstream>
#include <sstream>
#include <type_traits>
#include <algorithm>
#include "Input.hpp"
#include "TimeKeeper.hpp"
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}
#include "luahttp.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

inline std::string getStringFromLuaState(lua_State* L, int stackIdx) {
	if (!lua_isstring(L, stackIdx)) {
		throw std::runtime_error("Stack variable is not a string at given index");
	}

	const char* cstr = lua_tostring(L, stackIdx);

	std::string cppstr(cstr);

	return cppstr;
}

inline std::optional<std::string> readFile(std::string path, bool silent=false) {
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		if(!silent)
			std::cerr << "file not found" << std::endl;
		return {};
	}

	std::ifstream file(path);

	if (!file) {
		if (!silent)
			std::cerr << "failed to load file" << std::endl;
		return {};
	}

	std::stringstream buff;
	buff << file.rdbuf();

	std::string content = buff.str();
	file.close();

	return std::make_optional(content);
}