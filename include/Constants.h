#ifndef INCLUDE_CONSTANTS_H_
#define INCLUDE_CONSTANTS_H_


namespace Constants {

/*
 * Logging
 */
auto constexpr LOG_FILE = "logs/log.txt";
auto constexpr LOGGER_NAME = "robolog";


/*
 * path and file name to objectives in DAT file(s)
 */
auto constexpr OBJ_FILE_DAT = "CybathlonObjectives.dat";

/*
 * path and file name to objectives in JSON file(s)
 */
auto constexpr OBJ_FILE_JSON = "CybathlonObjectives.json";

/*
 * JSON keys
 */
auto constexpr OBJ_NAME = "name";
auto constexpr OBJ_ORIGIN_ABS = "is_abs";
auto constexpr OBJ_ORIGIN = "origin";
auto constexpr OBJ_SEQUENCE = "sequence";
auto constexpr OBJ_CSYS_X = "X";
auto constexpr OBJ_CSYS_Y = "Y";
auto constexpr OBJ_CSYS_Z = "Z";
auto constexpr OBJ_CSYS_PITCH = "pitch";
auto constexpr OBJ_CSYS_YAW = "yaw";
auto constexpr OBJ_CSYS_ROLL = "roll";

} // namespace Constants

#endif /* INCLUDE_CONSTANTS_H_ */
