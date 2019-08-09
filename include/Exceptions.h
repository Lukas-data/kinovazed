#ifndef KINOVA_EXCEPTIONS_H_
#define KINOVA_EXCEPTIONS_H_

#include <ostream>
#include <sstream>

namespace Kinova {

template <typename...Args>
void streamArguments(std::ostream & out, Args const &...args) {
	(out << ... << args);
}

template <typename ExceptionType, typename...Args>
auto composeException(Args const &...args) {
	std::ostringstream errorMessage{};
	streamArguments(errorMessage, args...);
	return ExceptionType{errorMessage.str()};
}

}


#endif /* INCLUDE_EXCEPTIONS_H_ */
