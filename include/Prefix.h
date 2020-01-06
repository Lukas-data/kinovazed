#ifndef INCLUDE_PREFIX_H_
#define INCLUDE_PREFIX_H_

namespace BytePrefix {

constexpr auto operator"" _K(unsigned long long int value) {
	return value * 1024;
}

constexpr auto operator"" _M(unsigned long long int value) {
	return operator""_K(value)*1024;
}

constexpr auto operator"" _G(unsigned long long int value) {
	return operator""_M(value)*1024;
}

} // namespace BytePrefix

#endif /* INCLUDE_PREFIX_H_ */
