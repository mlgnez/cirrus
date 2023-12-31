#include "luahttp.hpp"
#include "HudWindow.hpp"

static int getResourceAt(lua_State* L) {
    std::string url = getStringFromLuaState(L, 1);
    std::string path = getStringFromLuaState(L, 2);
    std::string callback_path = getStringFromLuaState(L, 3);

    int handle = HudWindowManager::Singleton->curHandle;

    // i dont know how to tell this story, so imma 4chan greentext it:
    /*
    > be me
    > needs to write c++ http client code
    > need to make it so the program doesnt hang every request
    > put request in thread
    > never used c++ threads before, so look it up
    > looks easy
    > smug.png
    > write in boilerplate http client code
    > add it to lua interpreter
    > run
    > crashes on thread
    > hmm.jpg
    > remove everything from thread except cout
    > still crash
    > ask chatgpt
    > find detach()
    > still not work
    > repeat for 1.5 hrs
    > decide lua interpreter doesnt like multithread
    > let's refactor code to not use lua interpreter in thread
    > need to learn how to use lambda captures
    > create callback system
    > massive pain
    > try
    > still crash
    > find out its not thread crashing. httplib is just throwing an error
    > bruh.jpg
    > ididallofthisrefactoringfornothing
    > want to die
    > realize its because my url has https in it
    > change https to http
    > it all works
    > omfg
    > wonder why https doesnt work. it shows it in example code for httplib
    > find #define CPPHTTPLIB_OPENSSL_SUPPORT
    > https. doesn't. work. because. of. one. define. statement. missing.
    > try to compile
    > not a runtime error anymore. its a compile error.
    > whatthefuck.png
    > check documentation
    > requires libssl for https
    > hope to god that libssl is a header only library
    > its not
    > look for precompiled binaries
    > find!
    > check documentation again
    > only v1.1.1 and v3.0 are compatible
    > check versions of precompiled binaries
    > v1.1.0 & 3.0.2
    > guess i have to compile them myself.
    > check documentation, install perl and nasm
    > no problems
    > follow instructions exactly as written
    > compile fatal error
    > it is 12:45. I want to sleep
    > ask chatgpt
    > it says to use a special command prompt
    > cant find it
    > check online
    > chatgpt gave me the wrong place to check
    > use special command prompt
    > drag include folder into includes directory
    > compile
    > unlinked libraries
    > sigh
    > ask chatgpt
    > says to check /libs folder
    > no /libs folder
    > look using eyes
    > find .lib and .dll in root
    > import into project
    > compile
    > cant find libssl.lib
    > add as Additional Dependancies
    > compiles finally
    > libssl-1_1-x64.dll not found
    > i've seen this show already
    > tell vsc to copy dlls to build directory
    > everything works
    > what a night
    */

    std::thread([](std::string url, std::string path, std::string callbackPath, int handle) {
        try {
            httplib::Client client(url);

            auto result = client.Get(path);



            if (result.error() == httplib::Error::Success) {
                std::string* body = new std::string(result.value().body);
                HudWindowManager::Singleton->get(handle)->addCallback(CallbackFunction{ .callbackPath = callbackPath, .callbackSetup = [body](lua_State* L) {
                    lua_pushstring(L, body->c_str());
                    lua_setglobal(L, "response");

                    delete body;

                    return 0;
                }, .callbackCleanup = [](lua_State* L) {
                    lua_pushnil(L);
                    lua_setglobal(L, "response");
                } });
            }
        }
        catch (httplib::Error err) {
            std::cerr << err << std::endl;
        }
        }, url, path, callback_path, handle).detach();

        return 0;
}


static int postResourceAt(lua_State* L) {
    std::string url = getStringFromLuaState(L, 1);
    std::string path = getStringFromLuaState(L, 2);
    std::string body = getStringFromLuaState(L, 3);
    std::string callback_path = getStringFromLuaState(L, 4);

    int handle = HudWindowManager::Singleton->curHandle;

    std::thread([](std::string url, std::string path, std::string callbackPath, std::string body, int handle) {
        try {
            httplib::Client client(url);

            auto result = client.Post(path, body, "application/json");

            if (result.error() == httplib::Error::Success) {
                std::string* body = new std::string(result.value().body);
                HudWindowManager::Singleton->get(handle)->addCallback(CallbackFunction{ .callbackPath = callbackPath, .callbackSetup = [body](lua_State* L) {
                    lua_pushstring(L, body->c_str());
                    lua_setglobal(L, "response");

                    delete body;

                    return 0;
                }, .callbackCleanup = [](lua_State* L) {
                    lua_pushnil(L);
                    lua_setglobal(L, "response");
                } });
            }
        }
        catch (httplib::Error err) {
            std::cerr << err << std::endl;
        }
        }, url, path, callback_path, body, handle).detach();

        return 0;
}

// thanks chatgpt!
void json_to_lua_table(lua_State* L, const nlohmann::json& j) {
    if (j.is_object()) {
        lua_newtable(L); // Create a new table on the Lua stack
        for (auto& el : j.items()) { 
            lua_pushstring(L, el.key().c_str()); // Push the key
            json_to_lua_table(L, el.value()); // Recursively process the value
            lua_settable(L, -3); // Set the key-value pair in the table
        }
    }
    else if (j.is_array()) {
        lua_newtable(L); // Create a new table on the Lua stack
        int index = 1;
        for (const auto& el : j) {
            lua_pushnumber(L, index); // Push the index
            json_to_lua_table(L, el); // Recursively process the value
            lua_settable(L, -3); // Set the key-value pair in the table
            index++;
        }
    }
    else if (j.is_string()) {
        lua_pushstring(L, j.get<std::string>().c_str()); // Push a string
    }
    else if (j.is_number_integer()) {
        lua_pushinteger(L, j.get<int>()); // Push an integer
    }
    else if (j.is_number()) {
        lua_pushnumber(L, j.get<double>()); // Push a number
    }
    else if (j.is_boolean()) {
        lua_pushboolean(L, j.get<bool>()); // Push a boolean
    }
    else if (j.is_null()) {
        lua_pushnil(L); // Push nil
    }
}

nlohmann::json lua_table_to_json(lua_State* L, int index) {
    nlohmann::json j;
    // Normalize the index to handle positive indices as well.
    if (index < 0) {
        index = lua_gettop(L) + index + 1;
    }

    lua_pushnil(L); // first key

    while (lua_next(L, index) != 0) {
        // 'key' is at index -2 and 'value' at index -1
        std::string key = lua_tostring(L, -2); // Convert the key to string
        if (lua_isstring(L, -1)) {
            j[key] = lua_tostring(L, -1);
        }
        else if (lua_isnumber(L, -1)) {
            j[key] = lua_tonumber(L, -1);
        }
        else if (lua_istable(L, -1)) {
            // Use the correct index for the nested table.
            j[key] = lua_table_to_json(L, -1);
        }
        lua_pop(L, 1); // Remove 'value', keep 'key' for the next iteration
    }
    return j;
}


static int lua_json_stringify(lua_State* L) {
    // Check if the top of the stack is a table
    if (!lua_istable(L, -1)) {
        lua_pushstring(L, "Error: argument is not a table");
        lua_error(L);
        return 1;
    }

    // Convert the Lua table to JSON
    nlohmann::json j = lua_table_to_json(L, lua_gettop(L));

    // Create the Lua string representation
    std::string luaStr = j.dump(2);

    // Push the result string onto the stack
    lua_pushstring(L, luaStr.c_str());
    return 1; // Number of return values
}

int jsonParse(lua_State* L) {
    std::string json = getStringFromLuaState(L, 1);

    if (json.empty()) {
        lua_pushnil(L);
        return 1;
    }

    json_to_lua_table(L, json::parse(json));

    return 1;
}


void IncludeLuaHttp(lua_State* L) {
    std::unordered_map<std::string, LuaCFunction> functionMap;
    
    functionMap["getResourceAt"] = getResourceAt;
    functionMap["postResourceAt"] = postResourceAt;
    functionMap["jsonParse"] = jsonParse;
    functionMap["jsonStringify"] = lua_json_stringify;

    std::thread([]() {
        std::cout << "Hello, World!" << std::endl;
    }).detach();

    for (const auto& pair : functionMap) {
        lua_register(L, pair.first.c_str(), pair.second);
    }
}