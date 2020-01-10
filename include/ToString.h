#ifndef INCLUDE_TO_STRING_H_
#define INCLUDE_TO_STRING_H_

#include <string>

namespace Kinova {

template<typename Type>
auto toString(Type const &object) -> std::string = delete;

}

#endif