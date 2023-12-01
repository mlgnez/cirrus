#pragma once
#include "includes.hpp"
#include "HudWindow.hpp"

class Addon;


class AnyService {
public:
    float timeSinceLastTick;
	std::vector<Addon*> subscribedAddons;
    virtual inline void tick() {};
    virtual inline float getTickrate() { return 0; }
};


template <typename T>
class Service : public AnyService {
private:
	
protected:
	T data;
public:
	inline void listen(Addon* addon) {
        subscribedAddons.push_back(addon);
    }
	bool isEnabled() {
        return subscribedAddons.size() > 0;
    }
	inline void clearListeners() { subscribedAddons.clear(); }
	inline virtual T getData() { return data; }
};


struct WifiData {
    bool shouldDiscard = false;
	long long uploadBytes;
	long long downloadBytes;
	long long uploadBytesXSeconds;
	long long downloadBytesXSeconds;
	std::chrono::system_clock::time_point timestamp;

    inline void toLuaTable(lua_State* L) {
        lua_newtable(L); // Create a new table and push it onto the stack

        // uploadBytes
        lua_pushstring(L, "uploadBytes");
        lua_pushinteger(L, uploadBytes);
        lua_settable(L, -3);

        // downloadBytes
        lua_pushstring(L, "downloadBytes");
        lua_pushinteger(L, downloadBytes);
        lua_settable(L, -3);

        // uploadBytesXSeconds
        lua_pushstring(L, "uploadBytesXSeconds");
        lua_pushinteger(L, uploadBytesXSeconds);
        lua_settable(L, -3);

        // downloadBytesXSeconds
        lua_pushstring(L, "downloadBytesXSeconds");
        lua_pushinteger(L, downloadBytesXSeconds);
        lua_settable(L, -3);

        // timestamp (as an example, converting to seconds since epoch)
        lua_pushstring(L, "timestamp");
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
        lua_pushinteger(L, static_cast<lua_Integer>(seconds));
        lua_settable(L, -3);

        // The table is now at the top of the Lua stack
    }
};

static std::string WIFI_SERVICE_IDENTIFIER = "builtin.monitoring.wifi";

class WifiService : public Service<WifiData> {
private:
	float timespanSeconds = 90;
	std::deque<WifiData> historicalData;
    WifiData prev_raw;

public:
	inline float getTickrate() override { return 1; }
	void tick();
	WifiData getCurrentWifiData();
    WifiService();
    ~WifiService();
};