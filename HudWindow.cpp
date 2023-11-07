#include "includes.hpp"

class HudWindow {
public:
	void render(InputHelper input) {

	}

	void awake() {

	}
};

class HudWindowRegistry {
private:
	std::unordered_map<int, HudWindow*> windows;

public:
	std::tuple<int, HudWindow*> registerWindow() {
		auto window = new HudWindow();

		int location = windows.size();
		windows[location] = window;

		window->awake();

		return std::make_tuple(location, window);
	}
};