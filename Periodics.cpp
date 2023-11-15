// CircularBuffer.cpp
/*
#include "Periodics.hpp"

template <typename T>
CircularBuffer<T>::CircularBuffer(size_t size) : bufferSize(size), buffer(size), head(0), tail(0) {}

template <typename T>
void CircularBuffer<T>::add(const T& item) {
    std::lock_guard<std::mutex> lock(mutex);
    buffer[tail] = item;
    tail = (tail + 1) % bufferSize;
    if (head == tail) {
        head = (head + 1) % bufferSize; // Overwrite the oldest data
    }
}

template <typename T>
std::vector<T> CircularBuffer<T>::getData() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<T> data;
    size_t current = head;
    while (current != tail) {
        data.push_back(buffer[current]);
        current = (current + 1) % bufferSize;
    }
    return data;
}


// Example of a function that periodically updates the buffer
void initPeriodic() {
    hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient); // remember to close this

    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "Error opening WLAN handle." << std::endl;
    }

    periodicThread = new std::thread([]() {
        while (true) {
            WLANData data = {};

            PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
            DWORD res = WlanEnumInterfaces(hClient, NULL, &pIfList);


            if (res != ERROR_SUCCESS) {
                std::cerr << "Error enumerating interfaces." << std::endl;
                // Handle error...
            }


            wlanBuffer->add(data);


            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }); // remember to delete this from ram

    periodicThread->detach(); // remember to join thread later

}*/