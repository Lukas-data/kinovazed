#ifndef INCLUDE_COMM_NOTIFICATION_H_
#define INCLUDE_COMM_NOTIFICATION_H_

#include "support/ToString.h"

#include <string>

namespace KinovaZED::Comm {

struct Notification {
	enum struct Id {
		Initialized,
		ModeSet,
		SequenceDone,

		// End Marker
		END_OF_ENUM
	};

	explicit Notification(Id id);

	auto getId() const noexcept -> Id;

  private:
	Id id;
};

auto isKnownNotificationId(int candidate) -> bool;

} // namespace KinovaZED::Comm

namespace KinovaZED {

template<>
auto toString(Comm::Notification const &notification) -> std::string;

template<>
auto toString(Comm::Notification::Id const &id) -> std::string;

} // namespace KinovaZED

#endif