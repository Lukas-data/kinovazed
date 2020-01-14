#ifndef INCLUDE_HW_KINOVA_STATUS_H_
#define INCLUDE_HW_KINOVA_STATUS_H_

namespace KinovaZED::Hw {

enum struct RetractionMode {
	NormalToReady,
	ReadyToStandby,
	ReadyToRetract,
	RetractToStandby,
	RetractToReady,
	Normal,
	NoInitToReady,
	Error = 25000,
};

} // namespace KinovaZED::Hw

#endif
