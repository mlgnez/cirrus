#include "UrlProtocolManager.h"

// Function to set the registry keys for custom URL protocol
bool SetURLProtocol(const std::wstring& protocolName, const std::wstring& applicationPath) {
    HKEY hKey;
    std::wstring keyPath = L"SOFTWARE\\Classes\\" + protocolName;
    LONG lRes = RegCreateKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    if (lRes != ERROR_SUCCESS) {
        std::wcerr << L"Failed to create registry key." << std::endl;
        return false;
    }

    // Set the default key value
    std::wstring protocolDescription = protocolName + L" Protocol Handler";
    RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)protocolDescription.c_str(), (protocolDescription.size() + 1) * sizeof(wchar_t));

    // Set URL Protocol key value
    std::wstring empty = L"";
    RegSetValueEx(hKey, L"URL Protocol", 0, REG_SZ, (BYTE*)empty.c_str(), (empty.size() + 1) * sizeof(wchar_t));

    // Set command executable path
    std::wstring commandPath = applicationPath + L" \"%1\"";
    HKEY hKeyCommand;
    lRes = RegCreateKeyEx(hKey, L"shell\\open\\command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyCommand, NULL);

    if (lRes != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        std::wcerr << L"Failed to create command registry key." << std::endl;
        return false;
    }

    RegSetValueEx(hKeyCommand, NULL, 0, REG_SZ, (BYTE*)commandPath.c_str(), (commandPath.size() + 1) * sizeof(wchar_t));

    RegCloseKey(hKeyCommand);
    RegCloseKey(hKey);

    return true;
}

std::wstring GetExecutablePath() {
    std::vector<wchar_t> buffer;
    DWORD copied = 0;
    do {
        buffer.resize(buffer.size() + MAX_PATH);
        copied = GetModuleFileNameW(nullptr, &buffer.at(0), static_cast<DWORD>(buffer.size()));
    } while (copied >= buffer.size());

    buffer.resize(copied); // Shrink to fit the actual size
    return std::wstring(buffer.begin(), buffer.end());
}


void init_url_protocol() {
    std::wstring protocol = L"cirrus";
    std::wstring appPath = GetExecutablePath();

    if (SetURLProtocol(protocol, appPath)) {
        std::wcout << L"Custom URL protocol registered successfully." << std::endl;
    }
    else {
        std::wcerr << L"Failed to register custom URL protocol." << std::endl;
    }
}

void elevateAndExit(int argc, char* argv[]) {
    std::string parameters;
    std::ostringstream oss;

    for (int i = 1; i < argc; ++i) {
        // Check if the argument contains space and enclose in quotes
        std::string arg(argv[i]);
        if (arg.find(' ') != std::string::npos) {
            oss << "\"" << arg << "\"";
        }
        else {
            oss << arg;
        }

        if (i < argc - 1) {
            oss << " "; // Add space between arguments
        }
    }

    parameters = oss.str();

    ShellExecute(NULL, L"runas", GetExecutablePath().c_str(), std::wstring(parameters.begin(), parameters.end()).c_str(), NULL, SW_NORMAL);

    exit(0);
}

bool isAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    // Create a SID for the Administrators group
    if (!AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        return false; // Could not allocate SID
    }

    // Check whether the token has the admin group
    if (!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
        isAdmin = FALSE;
    }

    FreeSid(adminGroup);
    return isAdmin == TRUE;
}

std::wstring getAppDataFolder() {
    // Buffer to store the path
    wchar_t appDataPath[MAX_PATH];

    // Get the AppData folder path
    HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath);

    if (SUCCEEDED(result)) {
        auto str = std::wstring(appDataPath);
        fs::create_directories(str);
        return str;
    }

    return std::wstring();
}

void manageURLProtocolRequest(std::string url) {
    std::cout << "Received URL: " << url << std::endl;

    // Parse the URL as needed
    std::string protocolPrefix = "cirrus://";
    if (url.substr(0, protocolPrefix.size()) == protocolPrefix) {
        std::string data = url.substr(protocolPrefix.size());

        size_t pos;
        while ((pos = data.find('/')) != std::string::npos) {
            data.erase(pos, 1);
        }

        std::cout << "Extracted data: " << data << std::endl;

        httplib::Client cli("https://raw.githubusercontent.com");

        httplib::Result res;
        try {
            res = cli.Get("/Rubyboat1207/cumulohost/main/" + data + "/manifest.json");
        }
        catch (std::exception ex) {
            std::cerr << ex.what() << std::endl;
            exit(1);
        }


        if (res && res->status == 200) {
            auto manifest = json::parse(res.value().body);


            std::vector<std::string> files;

            files.push_back("init.lua");
            files.push_back("manifest.json");

            if (manifest["useRender"]) {
                files.push_back("render.lua");
            }

            if (manifest["usePrerender"]) {
                files.push_back("prerender.lua");
            }

            auto callbacks = manifest["callbacks"];

            for (auto& element : callbacks) {
                files.push_back("callbacks/" + element.get<std::string>() + ".lua");
            }
            std::string writeroot = convert_str(getAppDataFolder()) + "/Cirrus/addons/" + data + "/";

            fs::create_directories(writeroot);
            fs::create_directories(writeroot + "callbacks");

            std::cout << fs::absolute(fs::path(writeroot)) << std::endl;

            for (auto const& file : files) {
                auto fileRes = cli.Get("/Rubyboat1207/cumulohost/main/" + data + "/" + file);

                if (fileRes && res->status == 200) {
                    std::string fullPath = writeroot + file;

                    // Open a file stream in binary mode
                    std::ofstream outFile(fullPath, std::ios::binary);

                    // Check if the file stream is open
                    if (outFile.is_open()) {
                        // Write the response body to the file
                        outFile.write(fileRes->body.data(), fileRes->body.size());

                        // Close the file stream
                        outFile.close();

                        // Optionally, you can log success message
                        std::cout << "Downloaded and saved: " << fullPath << std::endl;
                    }
                    else {
                        // Log an error message if the file couldn't be opened
                        std::cerr << "Failed to open file for writing: " << fullPath << std::endl;
                    }
                }
                else {
                    // Log an error message if the GET request failed
                    std::cerr << "Failed to download file: " << file << std::endl;
                }
            }

        }
        else {
            std::cout << res->body << std::endl;
            std::cout << res->status << std::endl;
            std::cout << ("https://raw.githubusercontent.com/Rubyboat1207/cumulohost/main" + data + "/manifest.json") << std::endl;
        }
    }
}