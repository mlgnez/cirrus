#include "HudWindow.hpp"
using LuaCFunction = int (*)(lua_State* L);

void InjectHudWinSL(lua_State* L);


// class HudWindow {
HudWindow::HudWindow(HudWinScripts lua) {
	this->scripts = lua;

	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	InjectHudWinSL(lua_state);
}

void HudWindow::render() {
	if (luaL_dostring(lua_state, scripts.prerender.c_str()) != LUA_OK) {
		std::cerr << "Failed to prerender HudWindow:" << lua_tostring(lua_state, -1) << std::endl;
	}

	ImGui::SetNextWindowSize(size);

	ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	pos = ImGui::GetWindowPos();

	if (ImGui::IsWindowHovered())
	{
		exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
	}
	else {
		exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
	}

	if (luaL_dostring(lua_state, scripts.render.c_str()) != LUA_OK) {
		std::cerr << "Failed to initialize HudWindow:" << lua_tostring(lua_state, -1) << std::endl;
	}

	ImGui::SetWindowPos(pos);

	ImGui::End();
}

void HudWindow::awake() {
	if (luaL_dostring(lua_state, scripts.init.c_str()) != LUA_OK) {
		std::cerr << "Failed to initialize HudWindow:" << lua_tostring(lua_state, -1) << std::endl;
	}
}

void HudWindow::setRegistry(HudWindowRegistry* registry) {
	this->registry = registry;
}

void HudWindow::setWidth(float w) {
	if (w < 0) {
		w = 0;
	}
	size.x = w;
}

void HudWindow::setHeight(float h) {
	if (h < 0) {
		h = 0;
	}
	size.y = h;
}
//  };

HudWindowRegistry* HudWindowRegistry::Singleton = nullptr;


// class HudWindowRegistry {
HudWindowRegistry::HudWindowRegistry(InputHelper* input, LONG_PTR exStyle, HWND hwnd, TimeKeeper* timekeeper) {
	this->input = input;
	this->exStyle = exStyle;
	this->hwnd = hwnd;
	this->timekeeper = timekeeper;
	curHandle = -1;
	if (HudWindowRegistry::Singleton == nullptr) {
		isSingletonInstance = true;
		HudWindowRegistry::Singleton = this;
		return;
	}
}

std::tuple<int, HudWindow*> HudWindowRegistry::registerWindow(HudWinScripts lua) {
	auto window = new HudWindow(lua);

	int location = windows.size();
	curHandle = location;
	windows[location] = window;
	window->setRegistry(this);
	window->exStyle = exStyle;
	window->hwnd = hwnd;
	window->input = input;

	window->awake();
	curHandle = -1;

	return std::make_tuple(location, window);
}


void HudWindowRegistry::renderAll() {
	for (const auto& pair : windows) {
		curHandle = pair.first;
		pair.second->render();
	}
}

HudWindow* HudWindowRegistry::get(int handle) {
	return windows[handle];
}
// };



std::string getStringFromLuaState(lua_State* L, int stackIdx) {
	if (!lua_isstring(L, stackIdx)) {
		throw std::runtime_error("Stack variable is not a string at given index");
	}

	const char* cstr = lua_tostring(L, stackIdx);

	std::string cppstr(cstr);

	return cppstr;
}

static int getCurrentHandle(lua_State* L) {
	lua_pushinteger(L, HudWindowRegistry::Singleton->curHandle);

	return 1;
}

static int setHudWindowName(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	std::string name;
	try {
		name = getStringFromLuaState(L, 2);
	}
	catch(std::runtime_error err) {
		return 0;
	}
	

	HudWindowRegistry::Singleton->get(handle)->name = name;

	return 0;
}

static int setWidth(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	float width = luaL_checknumber(L, 2);
	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	HudWin->setWidth(width);

	return 0;
}

static int setHeight(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	float height = luaL_checknumber(L, 2);
	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	HudWin->setHeight(height);

	return 0;
}

static int getWidth(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	lua_pushnumber(L, HudWin->getSize().x);

	return 1;
}

static int getHeight(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	lua_pushnumber(L, HudWin->getSize().y);

	return 1;
}

static int getX(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	lua_pushnumber(L, HudWin->pos.x);

	return 1;
}

static int getY(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	lua_pushnumber(L, HudWin->pos.y);

	return 1;
}

static int setX(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	float x = luaL_checknumber(L, 2);

	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	HudWin->pos.x = x;

	return 0;
}

static int setY(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);

	auto HudWin = HudWindowRegistry::Singleton->get(handle);

	HudWin->pos.y = y;

	return 0;
}

static int isKeyPressed(lua_State* L) {
	int keyCode = luaL_checknumber(L, 1);

	auto input = HudWindowRegistry::Singleton->input;

	lua_pushboolean(L, input->isKeyPressed(keyCode));

	return 1;
}


// Inject Hud Window Standard Library
void InjectHudWinSL(lua_State* L) {
	std::unordered_map<std::string, LuaCFunction> functionMap;
	// Register Functions Here

	functionMap["getCurrentHandle"] = getCurrentHandle;
	functionMap["setHudWindowName"] = setHudWindowName;
	functionMap["setWidth"] = setWidth;
	functionMap["setHeight"] = setHeight;
	functionMap["getWidth"] = getWidth;
	functionMap["getHeight"] = getHeight;
	functionMap["getX"] = getX;
	functionMap["getY"] = getY;
	functionMap["setX"] = setX;
	functionMap["setY"] = setY;
	functionMap["isKeyPressed"] = isKeyPressed;
	functionMap["getDeltaTime"] = [](lua_State* L) { lua_pushnumber(L, HudWindowRegistry::Singleton->timekeeper->deltaTime); return 1; };

	for (const auto& pair : functionMap) {
		lua_register(L, pair.first.c_str(), pair.second);
	}
}