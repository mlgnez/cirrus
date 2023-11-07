#pragma once
#include "includes.hpp"

class InputHelper {
private:
	std::unordered_map<int, bool> keyStates;
	std::unordered_map<int, bool> prevKeyStates;

public:
	void update();

	bool isKeyPressed(int key);

	bool wasKeyPressed(int key);

	bool wasKeyReleased(int key);
};