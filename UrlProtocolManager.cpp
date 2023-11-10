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
