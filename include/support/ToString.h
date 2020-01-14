#ifndef INCLUDE_SUPPORT_TO_STRING_H_
#define INCLUDE_SUPPORT_TO_STRING_H_

#include <string>

namespace KinovaZED {

template<typename Type>
auto toString(Type const &object) -> std::string = delete;

}

#endif