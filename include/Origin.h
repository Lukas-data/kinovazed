#ifndef INCLUDE_ORIGIN_H_
#define INCLUDE_ORIGIN_H_

#include "Coordinates.h"

namespace Kinova {

using f2d_vec_t = std::vector<std::vector<float>>;

struct Origin {
	Origin();
	explicit Origin(Coordinates const &origin);

	auto isZero() const -> bool;
	auto getCoordinates() const -> Coordinates const &;
	auto getTransformationMatrix() const -> f2d_vec_t const &;
	auto getInvertedTransformationMatrix() const -> f2d_vec_t const &;

	auto operator==(Origin const &other) const -> bool;
	auto operator!=(Origin const &other) const -> bool;

  private:
	Coordinates origin;
	f2d_vec_t transformationMatrix;
	f2d_vec_t invertedTransformationMatrix;
};

} // namespace Kinova

#endif