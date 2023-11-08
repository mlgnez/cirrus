#pragma once
#include "includes.hpp"

struct HudWinScripts {
	std::string render;
	std::string prerender;
	std::string init;
};

class HudWindowRegistry;

// Dictionary<int, List<Elements>> priorityQueue;

class HudWindow {
private:
	HudWinScripts scripts;
	HudWindowRegistry* registry;
	lua_State* lua_state;
	ImVec2 size;
public:
	InputHelper* input;
	std::string name;
	LONG_PTR exStyle;
	HWND hwnd;
	ImVec2 pos;

	HudWindow(HudWinScripts lua);

	void render();

	void awake();

	void setRegistry(HudWindowRegistry* registry);

	void setWidth(float w);

	void setHeight(float h);

	inline ImVec2 getSize() { return size; }
};

class HudWindowRegistry {
private:
	std::unordered_map<int, HudWindow*> windows;

public:
	int curHandle;
	static HudWindowRegistry* Singleton;
	bool isSingletonInstance = false;
	LONG_PTR exStyle;
	HWND hwnd;
	InputHelper* input;
	TimeKeeper* timekeeper;

	HudWindowRegistry(InputHelper* input, LONG_PTR exStyle, HWND hwnd, TimeKeeper* timekeeper);

	std::tuple<int, HudWindow*> registerWindow(HudWinScripts lua);


	void renderAll();

	HudWindow* get(int handle);
};