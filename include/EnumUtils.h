#ifndef INCLUDE_ENUM_UTILS_H_
#define INCLUDE_ENUM_UTILS_H_

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace Kinova {

auto constexpr constexprStringEquals(char const *lhs, char const *rhs) {
	while (*lhs && *rhs) {
		if (*lhs != *rhs) {
			return false;
		}
		++lhs, ++rhs;
	}
	return *lhs == *rhs;
}

template<typename KeyType, typename ValueType, std::size_t Size>
auto constexpr enumNameMappingsAreUnique(std::array<std::pair<KeyType, ValueType>, Size> const &map) -> bool {
	static_assert(std::is_enum_v<KeyType>, "The key type of the map must be an enum!");

	if constexpr (!Size) {
		return true;
	}

	for (auto const &entryToCheck : map) {
		auto enumOccurrences{0};
		for (auto const &entry : map) {
			enumOccurrences += static_cast<decltype(enumOccurrences)>(entryToCheck.first == entry.first);
		}
		if (enumOccurrences > 1) {
			return false;
		}
	}

	for (auto const &entryToCheck : map) {
		auto nameOccurrences{0};
		for (auto const &entry : map) {
			if constexpr (std::is_same_v<char const *, std::remove_cv_t<ValueType>>) {
				nameOccurrences +=
				    static_cast<decltype(nameOccurrences)>(constexprStringEquals(entryToCheck.second, entry.second));
			} else {
				nameOccurrences += static_cast<decltype(nameOccurrences)>(entryToCheck.second == entry.second);
			}
		}
		if (nameOccurrences > 1) {
			return false;
		}
	}

	return true;
}

template<typename KeyType, typename ValueType, std::size_t Size>
auto constexpr enumNameMapHasAllEntries(std::array<std::pair<KeyType, ValueType>, Size> const &, KeyType first)
    -> bool {
	static_assert(std::is_enum_v<KeyType>, "The key type of the map must be an enum!");

	auto beginValue = static_cast<std::underlying_type_t<KeyType>>(first);
	auto endValue = static_cast<std::underlying_type_t<KeyType>>(KeyType::END_OF_ENUM);

	return Size == (endValue - beginValue);
}

} // namespace Kinova

#endif