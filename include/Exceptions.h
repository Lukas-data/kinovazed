#ifndef KINOVA_EXCEPTIONS_H_
#define KINOVA_EXCEPTIONS_H_

#include <ostream>
#include <sstream>

namespace Kinova {


#if __cplusplus >= 201703L
// C++17
template<typename... Args>
void streamArguments(std::ostream &out, Args const &... args) {
	(out << ... << args);
}

#else

inline void streamArguments(std::ostream &) {
}

template<typename First, typename... Args>
void streamArguments(std::ostream &out, First const &first, Args const &... args) {
	out << first;
	streamArguments(out, args...);
}

#endif

template<typename ExceptionType, typename... Args>
auto composeException(Args const &... args) {
	std::ostringstream errorMessage{};
	streamArguments(errorMessage, args...);
	return ExceptionType{errorMessage.str()};
}

} // namespace Kinova


#endif /* INCLUDE_EXCEPTIONS_H_ */
