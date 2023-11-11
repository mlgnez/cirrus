#pragma once
#include "includes.hpp"

void init_url_protocol();
std::wstring GetExecutablePath();
bool isAdmin();
void elevateAndExit(int argc, char* argv[]);
std::wstring getAppDataFolder();
void manageURLProtocolRequest(std::string url);