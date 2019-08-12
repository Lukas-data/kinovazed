#ifndef TEST_TIMESLITERAL_H_
#define TEST_TIMESLITERAL_H_

#include <cstddef>
#include <functional>

namespace TimesLiteral {

struct TimesFunctor {
	explicit constexpr TimesFunctor(std::size_t numberOfExecutions) : numberOfExecutions{numberOfExecutions}{}

	template <typename Function>
	void operator()(Function && function) const {
		for (auto iteration = 0u; iteration < numberOfExecutions; iteration++) {
			function();
		}
	}

	std::size_t numberOfExecutions;
};

constexpr auto operator""_times(unsigned long long numberOfExecutions) {
	return TimesFunctor{numberOfExecutions};
}

}

#endif /* TEST_TIMESLITERAL_H_ */
