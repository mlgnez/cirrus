#include "CumuloServer.hpp"
#include <future>
#include "crow_all.h"


static bool shouldCloseThread = false;
static crow::App<crow::CORSHandler> app;
static std::future<void> runner;

void initCumuloServer() {

	CROW_ROUTE(app, "/addonList")([]() {
		auto addons = HudWindowRegistry::Singleton->getAddons();

		std::vector<json> strs;

		for (const auto& addon : addons) {
			strs.push_back(addon->serialize());
		}

		json j;

		j["addons"] = strs;

		return crow::json::wvalue(crow::json::load(j.dump()));
	});

	runner = app.port(64733).run_async();
}

void endCumuloServer() {
	app.stop();
}