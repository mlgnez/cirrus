#include "LuaSL.hpp"
using LuaCFunction = int (*)(lua_State* L);


static int isOverlapping(lua_State* L) {
    // Check that we have exactly 8 arguments
    if (lua_gettop(L) != 8) {
        lua_pushstring(L, "Function requires 8 parameters.");
        lua_error(L);
    }

    // Read the parameters from the Lua stack
    double x1 = lua_tonumber(L, 1);
    double y1 = lua_tonumber(L, 2);
    double w1 = lua_tonumber(L, 3);
    double h1 = lua_tonumber(L, 4);
    double x2 = lua_tonumber(L, 5);
    double y2 = lua_tonumber(L, 6);
    double w2 = lua_tonumber(L, 7);
    double h2 = lua_tonumber(L, 8);

    // Calculate the right and bottom edges of the boxes
    double right1 = x1 + w1;
    double bottom1 = y1 + h1;
    double right2 = x2 + w2;
    double bottom2 = y2 + h2;

    // Check for overlap
    bool overlap = !(right1 <= x2 || x1 >= right2 || bottom1 <= y2 || y1 >= bottom2);

    // Push the result onto the stack
    lua_pushboolean(L, overlap);

    // Return number of results
    return 1;
}

static int clamp(lua_State* L) {
    double v = lua_tonumber(L, 1);
    double i = lua_tonumber(L, 2);
    double a = lua_tonumber(L, 3);

    lua_pushnumber(L, std::max(i, std::min(v, a)));

    return 1;
}

void IncludeLuaSL(lua_State* L) {
    std::unordered_map<std::string, LuaCFunction> functionMap;

    functionMap["isOverlapping"] = isOverlapping;
    functionMap["clamp"] = clamp;

    for (const auto& pair : functionMap) {
        lua_register(L, pair.first.c_str(), pair.second);
    }
}