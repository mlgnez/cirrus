#include "luahttp.hpp"
#include "HudWindow.hpp"

static int getResourceAt(lua_State* L) {
    std::string url = getStringFromLuaState(L, 1);
    std::string path = getStringFromLuaState(L, 2);
    std::string callback_path = getStringFromLuaState(L, 3);

    int handle = HudWindowRegistry::Singleton->curHandle;

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
                HudWindowRegistry::Singleton->get(handle)->addCallback(CallbackFunction{ .callbackPath = callbackPath, .callbackSetup = [body](lua_State* L) {
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

void IncludeLuaHttp(lua_State* L) {
    std::unordered_map<std::string, LuaCFunction> functionMap;
    
    functionMap["getResourceAt"] = getResourceAt;

    std::thread([]() {
        std::cout << "Hello, World!" << std::endl;
    }).detach();

    for (const auto& pair : functionMap) {
        lua_register(L, pair.first.c_str(), pair.second);
    }
}