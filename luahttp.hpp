#pragma once
#include "includes.hpp"


using LuaCFunction = int (*)(lua_State* L);

void IncludeLuaHttp(lua_State* L);