#ifndef INCLUDE_SUPPORT_PREFIX_H_
#define INCLUDE_SUPPORT_PREFIX_H_

namespace KinovaZED::Literals {

constexpr auto operator"" _K(unsigned long long int value) {
	return value * 1024;
}

constexpr auto operator"" _M(unsigned long long int value) {
	return operator""_K(value)*1024;
}

constexpr auto operator"" _G(unsigned long long int value) {
	return operator""_M(value)*1024;
}

} // namespace KinovaZED::Literals

#endif /* INCLUDE_PREFIX_H_ */
