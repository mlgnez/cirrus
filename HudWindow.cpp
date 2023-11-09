#include "HudWindow.hpp"
using LuaCFunction = int (*)(lua_State* L);

void InjectHudWinSL(lua_State* L);
std::string getStringFromLuaState(lua_State* L, int stackIdx);

// class HudWindow {
HudWindow::HudWindow(HudWinScripts* lua) {
	this->scripts = lua;
	this->persistentData = new PersistentDataStore();

	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	InjectHudWinSL(lua_state);
	IncludeLuaSL(lua_state);
}

void HudWindow::render() {
	if (luaL_dostring(lua_state, scripts->prerender.c_str()) != LUA_OK) {
		std::cerr << "Failed to prerender HudWindow:" << lua_tostring(lua_state, -1) << std::endl;
	}

	ImGui::SetNextWindowSize(size);

	ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	if (ImGui::IsWindowHovered())
	{
		exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
	}
	else {
		exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
	}

	if (luaL_dostring(lua_state, scripts->render.c_str()) != LUA_OK) {
		std::cerr << "Failed to render HudWindow:" << lua_tostring(lua_state, -1) << std::endl;
	}

	ImGui::SetWindowPos(pos);

	ImGui::End();
}

void HudWindow::awake() {
	if (luaL_dostring(lua_state, scripts->init.c_str()) != LUA_OK) {
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

std::tuple<int, HudWindow*> HudWindowRegistry::registerWindow(HudWinScripts* lua) {
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

void HudWindowRegistry::initLua() {
	std::string path = "./addons";

	if (!fs::exists(path) || !fs::is_directory(path)) {
		if (!fs::create_directory(path)) {
			std::cout << "addons directory could not be created..." << std::endl;
		}
	}

	

	for (const auto& entry : fs::directory_iterator(path)) {
		lua_State* L = luaL_newstate();
		auto path = entry.path().string();

		auto manifestpath = path + "/manifest.lua";
		
		if (!fs::exists(manifestpath) || !fs::is_regular_file(manifestpath)) {
			continue;
		}

		auto manifest_op = readFile(manifestpath);

		if (!manifest_op.has_value()) {
			continue;
		}

		auto manifest = manifest_op.value();

		if (luaL_dostring(L, manifest.c_str()) != LUA_OK) {
			std::cerr << "Manifest failed with:" << lua_tostring(L, -1) << std::endl;
			continue;
		}

		// Required Field
		lua_getglobal(L, "Version");
		auto version = getStringFromLuaState(L, 1);
		lua_pop(L, 1);

		// Required Field
		lua_getglobal(L, "Identifier");
		auto ident = getStringFromLuaState(L, 1);
		lua_pop(L, 1);

		// Required Field
		lua_getglobal(L, "DisplayName");
		auto name = getStringFromLuaState(L, 1);
		lua_pop(L, 1);

		auto addon = new Addon();
		addon->version = version;
		addon->identifier = ident;
		addon->display_name = name;

		addon->scripts = new HudWinScripts();

		addon->scripts->render = readFile(path + "/render.lua", true).value_or("");
		addon->scripts->prerender = readFile(path + "/prerender.lua", true).value_or("");
		addon->scripts->init = readFile(path + "/init.lua").value_or("");

		addons.push_back(addon);
	}

	for (const auto& addon : addons) {
		registerWindow(addon->scripts);
	}
}

HudWindow* HudWindowRegistry::get(int handle) {
	return windows[handle];
}

bool ichar_equals(char a, char b)
{
	return std::tolower(static_cast<unsigned char>(a)) ==
		std::tolower(static_cast<unsigned char>(b));
}

bool iequals(std::string& a, std::string& b) {
	return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), ichar_equals);
}

std::optional<int> HudWindowRegistry::gethandle(std::string name) {
	for (const auto& pair : windows) {
		auto hwinname = pair.second->name;
		if (iequals(name, hwinname)) {
			return std::make_optional(pair.first);
		}
	}

	return {};
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

static int getWindowHandleFromString(lua_State* L) {
	std::string name = getStringFromLuaState(L, 1);

	auto handle = HudWindowRegistry::Singleton->gethandle(name);

	if (handle.has_value()) {
		lua_pushinteger(L, handle.value());
		return 1;
	}

	lua_pushnil(L);

	return 1;
}

static int getPersistentBoolean(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	std::string name = getStringFromLuaState(L, 2);

	auto HudWin = HudWindowRegistry::Singleton->get(handle);
	auto data = HudWin->getPersistentData();

	if (data->flagStorage.contains(name)) {
		lua_pushboolean(L, data->flagStorage[name]);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static int setPersistentBoolean(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	std::string name = getStringFromLuaState(L, 2);
	bool pushdata = lua_toboolean(L, 3);


	auto HudWin = HudWindowRegistry::Singleton->get(handle);
	auto data = HudWin->getPersistentData();

	data->flagStorage[name] = pushdata;

	return 0;
}

// Inject Hud Window Standard Library
void InjectHudWinSL(lua_State* L) {
	std::unordered_map<std::string, LuaCFunction> functionMap;
	// Register Functions Here

	functionMap["getCurrentHandle"] = getCurrentHandle;
	functionMap["getWindowHandleFromString"] = getWindowHandleFromString;
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
	functionMap["setPersistentBoolean"] = setPersistentBoolean;
	functionMap["getPersistentBoolean"] = getPersistentBoolean;


	for (const auto& pair : functionMap) {
		lua_register(L, pair.first.c_str(), pair.second);
	}
}