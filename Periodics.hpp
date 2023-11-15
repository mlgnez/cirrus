#pragma once
#include "includes.hpp"
#include <wlanapi.h>
#pragma comment(lib, "wlanapi.lib")

template <typename T>
class CircularBuffer {
public:
    explicit CircularBuffer(size_t size);
    void add(const T& item);
    std::vector<T> getData();

private:
    std::vector<T> buffer;
    size_t head;
    size_t tail;
    size_t bufferSize;
    std::mutex mutex;
};

struct WLANData {
    unsigned long long bytesSent;
    unsigned long long bytesReceived;
};

static CircularBuffer<WLANData> wlanBuffer[60];

static std::thread* periodicThread;
static HANDLE hClient;