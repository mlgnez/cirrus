#pragma once
#include "includes.hpp"
#include "LuaSL.hpp"

struct HudWinScripts {
	std::string render;
	std::string prerender;
	std::string init;
};

class HudWindowRegistry;

// Dictionary<int, List<Elements>> priorityQueue;

struct PersistentDataStore {
	std::unordered_map<std::string, int> intStorage;
	std::unordered_map<std::string, float> floatStorage;
	std::unordered_map<std::string, std::string> stringStorage;
	std::unordered_map<std::string, boolean> flagStorage;
};

class HudWindow {
private:
	HudWinScripts* scripts;
	HudWindowRegistry* registry;
	lua_State* lua_state;
	ImVec2 size;
	PersistentDataStore* persistentData;
public:
	InputHelper* input;
	std::string name;
	LONG_PTR exStyle;
	HWND hwnd;
	ImVec2 pos;

	HudWindow(HudWinScripts* lua);

	void render();

	void awake();

	void setRegistry(HudWindowRegistry* registry);

	void setWidth(float w);

	void setHeight(float h);

	inline ImVec2 getSize() { return size; }

	inline PersistentDataStore* getPersistentData() { return persistentData; }
};

class Addon {
public:
	HudWinScripts* scripts;
	std::string version;
	std::string identifier;
	std::string display_name;
};

class HudWindowRegistry {
private:
	std::unordered_map<int, HudWindow*> windows;
	std::vector<Addon*> addons;
public:
	int curHandle;
	static HudWindowRegistry* Singleton;
	bool isSingletonInstance = false;
	LONG_PTR exStyle;
	HWND hwnd;
	InputHelper* input;
	TimeKeeper* timekeeper;

	HudWindowRegistry(InputHelper* input, LONG_PTR exStyle, HWND hwnd, TimeKeeper* timekeeper);

	std::tuple<int, HudWindow*> registerWindow(HudWinScripts* lua);

	void initLua();

	void renderAll();

	HudWindow* get(int handle);
	std::optional<int> gethandle(std::string name);
};