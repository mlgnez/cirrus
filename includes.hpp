#pragma once


#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
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
namespace fs = std::filesystem;

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