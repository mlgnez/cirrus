#pragma once
#include "includes.hpp"
#include "widget.hpp"
#include "LuaSL.hpp"
#include "services.hpp"

class AnyService;


struct HudWinScripts {
	std::string render;
	std::string prerender;
	std::string init;
};

class HudWindowManager;

// Dictionary<int, List<Elements>> priorityQueue;

struct PersistentDataStore {
	std::unordered_map<std::string, int> intStorage;
	std::unordered_map<std::string, float> floatStorage;
	std::unordered_map<std::string, std::string> stringStorage;
	std::unordered_map<std::string, bool> flagStorage;
};

struct CallbackFunction {
	std::string callbackPath;
	std::function<void(lua_State*)> callbackSetup;
	std::function<void(lua_State*)> callbackCleanup;
};

class Addon;

class HudWindow {
private:
	HudWinScripts* scripts;
	HudWindowManager* registry;
	lua_State* lua_state;
	ImVec2 size;
	PersistentDataStore* persistentData;
	std::unordered_map<int, std::vector<Widget*>> widgetList;
	std::unordered_map<std::string, Widget*> widgetIdentifiers;
	std::vector<CallbackFunction> queuedCallbackRunners;
	std::mutex callbackmutex;
	int framesRendererd = 0;
public:
	InputHelper* input;
	std::string name;
	LONG_PTR exStyle;
	HWND hwnd;
	ImVec2 pos;
	Addon* addon;

	HudWindow(HudWinScripts* lua);
	~HudWindow();

	bool isCursorOverBox(int cursorX, int cursorY, int boxX, int boxY, int boxWidth, int boxHeight);

	void render(bool slotMode);

	void awake();

	void setRegistry(HudWindowManager* registry);

	void setWidth(float w);

	void setHeight(float h);

	inline ImVec2 getSize() { return size; }

	inline PersistentDataStore* getPersistentData() { return persistentData; }

	void addWidget(std::string identifier, int renderPriority, Widget* widget);

	void addCallback(CallbackFunction callbackfunc);
	void savePersistentVariables();
	void loadPersistentVariables();

	inline bool scriptsEqual(HudWinScripts* scripts) {
		return scripts == this->scripts;
	}

	template <typename T>
	std::optional<T*> getWidget(std::string identifier);
};

class Addon {
public:
	HudWinScripts* scripts;
	std::string version;
	std::string identifier;
	std::string display_name;
	std::string folderPath;

	inline json serialize() {
		return {
			{"version", this->version},
			{"identifier", this->identifier}
		};
	}
};

class HudWindowManager {
private:
	std::unordered_map<int, HudWindow*> windows;
	std::vector<Addon*> addons;
	std::unordered_map<std::string, AnyService*> registered_services;
public:
	int curHandle;
	static HudWindowManager* Singleton;
	bool isSingletonInstance = false;
	LONG_PTR exStyle;
	HWND hwnd;
	InputHelper* input;
	TimeKeeper* timekeeper;
	bool scaredMode;
	bool slotMode;

	HudWindowManager(InputHelper* input, LONG_PTR exStyle, HWND hwnd, TimeKeeper* timekeeper);

	std::tuple<int, HudWindow*> registerWindow(Addon* lua);

	void initLua();

	void renderAll();

	inline void registerService(std::string ident, AnyService* service) { registered_services[ident] = service; }

	template <typename T>
	inline std::optional<T*> getService(std::string ident) {
		if (!registered_services.contains(ident)) {
			return std::optional<T*>(nullptr);
		}

		AnyService* service = registered_services[ident];

		T* casted = static_cast<T*>(service);

		if (casted == nullptr) {
			return std::optional<T*>(nullptr);
		}

		return casted;
	}
	bool isListeningTo(std::string serviceIdent, Addon* addon);

	HudWindow* get(int handle);
	std::optional<int> gethandle(std::string name);
	Addon* getAddonFromWindow(HudWindow* window);
	inline std::vector<Addon*> getAddons() { return addons; }
};