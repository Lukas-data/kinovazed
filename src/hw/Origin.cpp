#include "hw/Origin.h"

#include <array>

namespace KinovaZED::Hw {

static auto calculateTransformationMatrix(Coordinates const &origin) -> f2d_vec_t {
	std::array<float, 3> const components{origin.x, origin.y, origin.z};
	std::array<float, 3> const cosines{std::cos(origin.pitch), std::cos(origin.yaw), std::cos(origin.roll)};
	std::array<float, 3> const sines{std::sin(origin.pitch), std::sin(origin.yaw), std::sin(origin.roll)};

	return {{cosines[1] * cosines[2], -cosines[1] * sines[2], sines[1], components[0]},
	        {cosines[0] * sines[2] + sines[0] * cosines[2] * sines[1],
	         cosines[0] * cosines[2] - sines[0] * sines[1] * sines[2],
	         -sines[0] * cosines[1],
	         components[1]},
	        {sines[0] * sines[2] - cosines[0] * cosines[2] * sines[1],
	         sines[0] * cosines[2] + cosines[0] * sines[1] * sines[2],
	         cosines[0] * cosines[1],
	         components[2]},
	        {0, 0, 0, 1}};
}

static auto invertMatrix(f2d_vec_t const &matrix) -> f2d_vec_t {
	return {{matrix[0][0],
	         matrix[1][0],
	         matrix[2][0],
	         -matrix[0][0] * matrix[0][3] - matrix[1][3] * matrix[1][0] - matrix[2][3] * matrix[2][0]},
	        {matrix[0][1],
	         matrix[1][1],
	         matrix[2][1],
	         -matrix[0][1] * matrix[0][3] - matrix[1][3] * matrix[1][1] - matrix[2][3] * matrix[2][1]},
	        {matrix[0][2],
	         matrix[1][2],
	         matrix[2][2],
	         -matrix[0][2] * matrix[0][3] - matrix[1][3] * matrix[1][2] - matrix[2][3] * matrix[2][2]},
	        {0, 0, 0, 1}};
}

Origin::Origin()
    : Origin{Coordinates{}} {
}

Origin::Origin(Coordinates const &origin)
    : origin{origin}
    , transformationMatrix{calculateTransformationMatrix(origin)}
    , invertedTransformationMatrix{invertMatrix(transformationMatrix)} {
}

auto Origin::isZero() const -> bool {
	return origin.isZero();
}

auto Origin::getCoordinates() const -> Coordinates const & {
	return origin;
}

auto Origin::getTransformationMatrix() const -> f2d_vec_t const & {
	return transformationMatrix;
}

auto Origin::getInvertedTransformationMatrix() const -> f2d_vec_t const & {
	return invertedTransformationMatrix;
}

auto Origin::operator==(Origin const &other) const -> bool {
	return origin == other.origin;
}

auto Origin::operator!=(Origin const &other) const -> bool {
	return !(*this == other);
}


} // namespace KinovaZED::Hw