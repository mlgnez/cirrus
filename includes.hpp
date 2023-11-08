#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include <tchar.h>
#include <iostream>
#include <unordered_map>
#include <tuple>
#include "Input.hpp"
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}