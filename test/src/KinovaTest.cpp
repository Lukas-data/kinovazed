#include "IntegrationSuite.h"
#include "KinovaArmSuite.h"
#include "MatrixSuite.h"
#include "PositionHandlingSuite.h"
#include "SequenceSuite.h"

#include <cute/cute.h>
#include <cute/cute_runner.h>
#include <cute/ide_listener.h>
#include <cute/xml_listener.h>

#include <lyra/arg.hpp>
#include <lyra/cli_parser.hpp>
#include <lyra/help.hpp>
#include <lyra/opt.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using suite_list = std::vector<std::pair<cute::suite, std::string>>;

auto get_test_selectors(suite_list const &suites) -> std::vector<std::string> {
	auto selectors = std::vector<std::string>{};

	for_each(cbegin(suites), cend(suites), [&](auto descriptor) {
		auto const &[suite, name] = descriptor;
		transform(cbegin(suite), cend(suite), std::back_inserter(selectors), [&, name = name](auto test) {
			return name + "#" + test.name();
		});
	});

	return selectors;
}

auto do_run_tests(suite_list const &suites, int argc, char **argv) -> bool {
	auto xunit_file = cute::xml_file_opener{argc, argv};
	auto listener = cute::xml_listener<cute::ide_listener<>>{xunit_file.out};
	auto runner = cute::makeRunner(listener, argc, argv);

	return accumulate(cbegin(suites), cend(suites), true, [&](auto accumulator, auto const &descriptor) {
		auto const &[suite, name] = descriptor;
		return accumulator && runner(suite, name.c_str());
	});
}

int main(int argc, char **argv) {
	using namespace std::string_literals;

	auto suites = std::vector{
	    std::pair{make_suite_MatrixSuite(), "Matrix Operations"s},
	    std::pair{make_suite_PositionHandlingSuite(), "Position Handling"s},
	    std::pair{make_suite_SequenceSuite(), "Sequencing"s},
	    std::pair{make_suite_IntegrationSuite(), "Integration Tests"s},
	    std::pair{make_suite_KinovaArmSuite(), "Kinova Arm"s},
	};

	auto selectors = get_test_selectors(suites);

	auto list_tests{false};
	auto list_suites{false};
	auto show_help{false};
	auto selected_tests = std::vector<std::string>{};

	auto cli = lyra::cli_parser() |                                                         //
	    lyra::opt(list_tests)["-t"]["--tests"]("List all registered tests") |               //
	    lyra::opt(list_suites)["-s"]["--suites"]("List all registered suites") |            //
	    lyra::arg(selected_tests, "test selector")("A pattern to select a specific test") | //
	    lyra::help(show_help);
	auto result = cli.parse({argc, argv});

	if (list_tests) {
		copy(cbegin(selectors), cend(selectors), std::ostream_iterator<std::string>{std::cout, "\n"});
	}
	if (list_suites) {
		transform(
		    cbegin(suites), cend(suites), std::ostream_iterator<std::string>{std::cout, "\n"}, [](auto descriptor) {
			    auto const &[_, name] = descriptor;
			    return name;
		    });
	} else if (!result || show_help) {
		std::cout << cli;
	} else {
		return do_run_tests(suites, argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
	}
}