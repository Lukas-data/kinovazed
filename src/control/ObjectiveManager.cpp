#include "control/ObjectiveManager.h"

#include "control/Sequence.h"
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
    : logger{logger} {
	if (in.peek() != std::remove_reference_t<decltype(in)>::traits_type::eof()) {
		for (auto &objective : loadObjectives(in, logger)) {
			objectives.emplace(objective.getId(), objective);
		}
	}
}

auto ObjectiveManager::getObjective(Objective::Id id) const -> Objective const & {
	return objectives.at(id);
}

auto ObjectiveManager::getObjective(Objective::Id id) -> Objective & {
	return objectives.at(id);
}

auto ObjectiveManager::loadObjectives(std::istream &in, Logger logger) -> std::vector<Objective> {
	auto json = nlohmann::json::parse(in);

	if (!json.empty() && !json.is_array()) {
		logger->error("ObjectiveManager::loadObjectives: expected an array as the rool element!");
		return {};
	}

	auto objectives = std::vector<Objective>{};
	objectives.reserve(json.size());

	transform(cbegin(json), cend(json), back_inserter(objectives), [&](auto const &element) {
		return Objective{element, logger};
	});

	return objectives;
}

} // namespace KinovaZED::Control