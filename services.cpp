#include "services.hpp"


void WifiService::tick() {
    WifiData currentData = getCurrentWifiData(); // Gather current stats
    currentData.timestamp = std::chrono::system_clock::now();

    if (prev_raw.shouldDiscard) {
        prev_raw = currentData;
    }

    // Add to historical data
    historicalData.push_back(currentData);

    // Remove outdated data
    while (!historicalData.empty() &&
        std::chrono::duration_cast<std::chrono::seconds>(
            currentData.timestamp - historicalData.front().timestamp).count() > timespanSeconds) {
        historicalData.pop_front();
    }

    // Calculate accumulated data over the last X seconds
    WifiData accumulatedData = {};
    if (historicalData.size() >= 2) {
        // Calculate accumulated data over the last X seconds
        const auto& oldestData = historicalData.front();
        const auto& newestData = historicalData.back();

        accumulatedData.uploadBytesXSeconds = newestData.uploadBytes - oldestData.uploadBytes;
        accumulatedData.downloadBytesXSeconds = newestData.downloadBytes - oldestData.downloadBytes;
    }
    else {
        accumulatedData.uploadBytesXSeconds = 0;
        accumulatedData.downloadBytesXSeconds = 0;
    }

    accumulatedData.downloadBytes = currentData.downloadBytes - prev_raw.downloadBytes;
    accumulatedData.uploadBytes = currentData.uploadBytes - prev_raw.uploadBytes;

    prev_raw = currentData;

    data = accumulatedData;
}

WifiData WifiService::getCurrentWifiData() {
    // Initialize the WifiData struct
    WifiData data = { false, 0, 0, 0, 0, std::chrono::system_clock::now() };

    // Get the network table
    PMIB_IFTABLE ifTable;
    DWORD dwSize = sizeof(MIB_IFTABLE);
    ifTable = (MIB_IFTABLE*)malloc(dwSize);
    if (GetIfTable(ifTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        free(ifTable);
        ifTable = (MIB_IFTABLE*)malloc(dwSize);
    }

    // Retrieve the table data
    if (GetIfTable(ifTable, &dwSize, FALSE) == NO_ERROR) {
        for (int i = 0; i < (int)ifTable->dwNumEntries; i++) {
            MIB_IFROW row = ifTable->table[i];
            // Check for a wireless interface; this check may need to be adjusted
            if (row.dwType == IF_TYPE_IEEE80211) {
                data.uploadBytes = static_cast<long long>(row.dwOutOctets);
                data.downloadBytes = static_cast<long long>(row.dwInOctets);

                // Add additional statistics as needed
                break;
            }
        }
    }

    // Clean up
    free(ifTable);

    return data;
}

WifiService::WifiService() {
    data = {};
    prev_raw = { .shouldDiscard = true };
}

WifiService::~WifiService() {
    historicalData.clear();
}