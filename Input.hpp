#pragma once
#include "includes.hpp"

class InputHelper {
private:
	std::unordered_map<int, bool> keyStates;
	std::unordered_map<int, bool> prevKeyStates;

public:
	inline void update() {
		for (int key = 0; key < 256; key++) {
			prevKeyStates[key] = keyStates[key];
			keyStates[key] = (GetAsyncKeyState(key) & 0x8000) != 0;
		}
	}

	inline bool isKeyPressed(int key) {
		return keyStates[key];
	}

	inline bool wasKeyPressed(int key) {
		return !prevKeyStates[key] && keyStates[key];
	}

	inline bool wasKeyReleased(int key) {
		return prevKeyStates[key] && !keyStates[key];
	}
};