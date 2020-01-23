#include "control/ObjectiveManager.h"

#include "control/Objective.h"
#include "support/Constants.h"
#include "support/Logging.h"
#include "support/Paths.h"

#include <nlohmann/json.hpp>
#include <spdlog/fmt/ostr.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace KinovaZED::Control {

ObjectiveManager::ObjectiveManager(std::istream &in, Logger logger)
    : LoggingMixin{logger, "ObjectiveManager"} {
	if (in.peek() != std::remove_reference_t<decltype(in)>::traits_type::eof()) {
		logInfo("<ctor>", "loading objectives");
		loadObjectives(in);
	}
}

auto ObjectiveManager::getObjective(Objective::Id id) const -> Objective const & {
	return objectives.at(id);
}

auto ObjectiveManager::getObjective(Objective::Id id) -> Objective & {
	return objectives.at(id);
}

auto ObjectiveManager::loadObjectives(std::istream &in) -> void {
	auto json = nlohmann::json::parse(in);

	if (!json.empty() && !json.is_array()) {
		logError("loadObjectives", "expected an array as the rool element!");
		return;
	}

	for_each(cbegin(json), cend(json), [&](auto const &element) {
		auto objective = Objective{element, getLogger()};
		objectives.emplace(objective.getId(), objective);
	});

	logInfo("loadObjectives", "loaded {} objectives", objectives.size());
}

} // namespace KinovaZED::Control