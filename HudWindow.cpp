#include "HudWindow.hpp"

using LuaCFunction = int (*)(lua_State* L);

void InjectHudWinSL(lua_State* L);

// class HudWindow {
HudWindow::HudWindow(HudWinScripts* lua) {
	this->scripts = lua;
	this->persistentData = new PersistentDataStore();

	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	IncludeLuaHttp(lua_state);
	InjectHudWinSL(lua_state);
	IncludeLuaSL(lua_state);
}

HudWindow::~HudWindow() {
	delete scripts;
	delete persistentData;
	
	for (auto const& pair : widgetList) {
		for (Widget* widget : pair.second) {
			delete widget;
		}
	}
	widgetList.clear();

	lua_close(lua_state);
}


void HudWindow::render() {
	if (luaL_dostring(lua_state, scripts->prerender.c_str()) != LUA_OK) {
		std::cerr << "Failed to prerender HudWindow:" << lua_tostring(lua_state, -1) << std::endl;
	}
	

	ImGui::SetNextWindowSize(size);

	ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	if (queuedCallbackRunners.size() > 0) {
		callbackmutex.lock();

		Addon* addon = HudWindowRegistry::Singleton->getAddonFromWindow(this);

		for (auto const& callbacks : queuedCallbackRunners) {
			auto path = addon->folderPath + "/callbacks/" + callbacks.callbackPath + ".lua";

			callbacks.callbackSetup(lua_state);

			auto optext = readFile(path);

			if (optext.has_value()) {
				if (luaL_dostring(lua_state, optext.value().c_str()) != LUA_OK) {
					std::cerr << "Failed to run a callback:" << lua_tostring(lua_state, -1) << std::endl;
				}
			}

			callbacks.callbackCleanup(lua_state);
		}

		queuedCallbackRunners.clear();

		callbackmutex.unlock();
	}

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

	for (auto const& prioritygroup : widgetList) {
		for (auto const& widget : prioritygroup.second) {
			ImGui::SetCursorPos(widget->getPos());
			widget->render();
		}
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

void HudWindow::addWidget(std::string identifier, int renderPriority, Widget* widget) {
	if (!widgetList.contains(renderPriority)) {
		widgetList[renderPriority] = {};
	}

	widgetList[renderPriority].push_back(widget);
	widgetIdentifiers[identifier] = widget;
}

template<typename T>
std::optional<T*> HudWindow::getWidget(std::string identifier) {
	static_assert(std::is_base_of<Widget, T>::value, "T must be a derived class of Widget");

	Widget* widget = widgetIdentifiers[identifier];

	T* derived = dynamic_cast<T*>(widget);

	if (derived == nullptr) {
		return {};
	}

	return derived;
}

void HudWindow::addCallback(CallbackFunction callbackfunc) {

	callbackmutex.lock();

	queuedCallbackRunners.push_back(callbackfunc);

	callbackmutex.unlock();
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

	for (const auto& addon : addons) {
		delete addon;
	}
	addons.clear();

	for (const auto& entry : fs::directory_iterator(path)) {
		lua_State* L = luaL_newstate();
		auto path = entry.path().string();

		auto manifestpath = path + "/manifest.json";
		
		if (!fs::exists(manifestpath) || !fs::is_regular_file(manifestpath)) {
			continue;
		}

		auto manifest_op = readFile(manifestpath);

		if (!manifest_op.has_value()) {
			continue;
		}

		auto manifest = json::parse(manifest_op.value());

		

		auto addon = new Addon();
		addon->folderPath = path;
		addon->version = manifest["version"];
		addon->identifier = manifest["identifier"];
		addon->display_name = manifest["name"];

		addon->scripts = new HudWinScripts();

		if (manifest["useRender"]) {
			addon->scripts->render = readFile(path + "/render.lua", true).value_or("");
		}
		if (manifest["usePrerender"]) {
			addon->scripts->prerender = readFile(path + "/prerender.lua", true).value_or("");
		}
		// init is required
		addon->scripts->init = readFile(path + "/init.lua").value_or("");

		addons.push_back(addon);
	}

	for (const auto& window : windows) {
		delete window.second;
	}
	windows.clear();

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
	std::mutex m;

	m.lock();
	for (const auto& pair : windows) {
		auto hwinname = pair.second->name;
		if (iequals(name, hwinname)) {
			return std::make_optional(pair.first);
		}
	}
	m.unlock();

	return {};
}

Addon* HudWindowRegistry::getAddonFromWindow(HudWindow* window) {
	for (auto const& addon : addons) {
		if (window->scriptsEqual(addon->scripts)) {
			return addon;
		}
	}

	return nullptr;
}
// };

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

static int getPersistentFloat(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	std::string name = getStringFromLuaState(L, 2);

	auto HudWin = HudWindowRegistry::Singleton->get(handle);
	auto data = HudWin->getPersistentData();

	if (data->floatStorage.contains(name)) {
		lua_pushnumber(L, data->floatStorage[name]);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static int setPersistentFloat(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	std::string name = getStringFromLuaState(L, 2);
	float pushdata = luaL_checknumber(L, 3);


	auto HudWin = HudWindowRegistry::Singleton->get(handle);
	auto data = HudWin->getPersistentData();

	data->floatStorage[name] = pushdata;

	return 0;
}

static int addTextWidget(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	std::string identifier = getStringFromLuaState(L, 2);
	int priority = luaL_checknumber(L, 3);

	auto hudWindow = HudWindowRegistry::Singleton->get(handle);

	hudWindow->addWidget(identifier, priority, new TextWidget());

	return 0;
}

static int addButtonWidget(lua_State* L) {
	int handle = luaL_checknumber(L, 1);
	std::string identifier = getStringFromLuaState(L, 2);
	int priority = luaL_checknumber(L, 3);

	auto hudWindow = HudWindowRegistry::Singleton->get(handle);

	hudWindow->addWidget(identifier, priority, new ButtonWidget());

	return 0;
}


template<typename T>
std::optional<T*> doWidgetAction(lua_State* L) {
	static_assert(std::is_base_of<Widget, T>::value, "T must be a derived class of Widget");
	int handle = luaL_checknumber(L, 1);
	std::string identifier = getStringFromLuaState(L, 2);

	auto hudWindow = HudWindowRegistry::Singleton->get(handle);

	std::optional<T*> widget = hudWindow->getWidget<T>(identifier);

	if (!widget.has_value()) {
		lua_pushboolean(L, 0);
		return {};
	}

	return widget.value();
}

static int setButtonOnClick(lua_State* L) {
	auto widget = doWidgetAction<ButtonWidget>(L);
	std::string* callback = new std::string(getStringFromLuaState(L, 3));

	if (!widget.has_value()) {
		return 0;
	}

	widget.value()->onClick([callback]() {
		HudWindowRegistry::Singleton->get(HudWindowRegistry::Singleton->curHandle)->addCallback(CallbackFunction{
			.callbackPath = *callback,
			.callbackSetup = [](lua_State* L) {},
			.callbackCleanup = [callback](lua_State* L) { }
			});
		});
	lua_pushboolean(L, 1);

	return 1;
}


static int setTextWidgetContent(lua_State* L) {
	auto widget = doWidgetAction<TextWidget>(L);
	std::string text = getStringFromLuaState(L, 3);

	if (!widget.has_value()) {
		return 0;
	}

	widget.value()->setText(text);
	lua_pushboolean(L, 1);

	return 1;
}

static int setButtonWidgetLabel(lua_State* L) {
	auto widget = doWidgetAction<ButtonWidget>(L);
	std::string text = getStringFromLuaState(L, 3);

	if (!widget.has_value()) {
		return 0;
	}

	widget.value()->setText(text);
	lua_pushboolean(L, 1);

	return 1;
}


static int setWidgetX(lua_State* L) {
	auto widget = doWidgetAction<Widget>(L);
	float x = luaL_checknumber(L, 3);

	if (!widget.has_value()) {
		return 0;
	}

	widget.value()->setPosX(x);
	lua_pushboolean(L, 1);

	return 1;
}

static int setWidgetY(lua_State* L) {
	auto widget = doWidgetAction<Widget>(L);
	float y = luaL_checknumber(L, 3);

	if (!widget.has_value()) {
		return 0;
	}

	widget.value()->setPosY(y);
	lua_pushboolean(L, 1);

	return 1;
}

static int setWidgetWidth(lua_State* L) {
	auto widget = doWidgetAction<TextWidget>(L);
	float x = luaL_checknumber(L, 3);

	if (!widget.has_value()) {
		return 0;
	}

	widget.value()->setWidth(x);
	lua_pushboolean(L, 1);

	return 1;
}

static int setWidgetHeight(lua_State* L) {
	auto widget = doWidgetAction<TextWidget>(L);
	float y = luaL_checknumber(L, 3);

	if (!widget.has_value()) {
		return 0;
	}

	widget.value()->setHeight(y);
	lua_pushboolean(L, 1);

	return 1;
}

static int getWidgetX(lua_State* L) {
	auto widget = doWidgetAction<TextWidget>(L);

	if (!widget.has_value()) {
		return 0;
	}

	lua_pushnumber(L, widget.value()->getPosX());

	return 1;
}

static int getWidgetY(lua_State* L) {
	auto widget = doWidgetAction<TextWidget>(L);

	if (!widget.has_value()) {
		return 0;
	}

	lua_pushnumber(L, widget.value()->getPosY());


	return 1;
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

	// likely soon to be deprecated
	functionMap["getX"] = getX;
	functionMap["getY"] = getY;
	functionMap["setX"] = setX;
	functionMap["setY"] = setY;

	functionMap["isKeyPressed"] = isKeyPressed;

	functionMap["getDeltaTime"] = [](lua_State* L) { lua_pushnumber(L, std::round(HudWindowRegistry::Singleton->timekeeper->deltaTime * 1000) / 1000); return 1; };

	functionMap["setPersistentBoolean"] = setPersistentBoolean;
	functionMap["getPersistentBoolean"] = getPersistentBoolean;
	functionMap["setPersistentFloat"] = setPersistentFloat;
	functionMap["getPersistentFloat"] = getPersistentFloat;

	functionMap["addTextWidget"] = addTextWidget;
	functionMap["setTextWidgetContent"] = setTextWidgetContent;
	functionMap["setButtonWidgetLabel"] = setButtonWidgetLabel;

	functionMap["addButtonWidget"] = addButtonWidget;
	functionMap["setButtonOnClick"] = setButtonOnClick;


	functionMap["setWidgetX"] = setWidgetX;
	functionMap["setWidgetY"] = setWidgetY;
	functionMap["getWidgetX"] = getWidgetX;
	functionMap["getWidgetY"] = getWidgetY;
	functionMap["setWidgetWidth"] = setWidgetWidth;
	functionMap["setWidgetHeight"] = setWidgetHeight;


	for (const auto& pair : functionMap) {
		lua_register(L, pair.first.c_str(), pair.second);
	}
}