#ifndef INCLUDE_SUPPORT_STRING_UTILS_H_
#define INCLUDE_SUPPORT_STRING_UTILS_H_

#include <algorithm>
#include <cctype>
#include <iterator>
#include <string>

namespace KinovaZED {

// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
auto inline trim(std::string &s) -> void {
	s.erase(begin(s), find_if(begin(s), end(s), [](int ch) { return !std::isspace(ch); }));
	s.erase(find_if(rbegin(s), rend(s), [](int ch) { return !std::isspace(ch); }).base(), end(s));
}

} // namespace KinovaZED

#endif