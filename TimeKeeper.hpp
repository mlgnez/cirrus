#include "includes.hpp"

struct TimeKeeper {
	std::chrono::steady_clock::time_point previousTime = std::chrono::steady_clock::now();
	float deltaTime = 0;

	void update() {
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - previousTime).count();
		previousTime = currentTime;
	}
};