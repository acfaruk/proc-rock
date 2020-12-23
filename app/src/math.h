#pragma once

namespace procrock {
namespace math {
inline float lerp(float a, float b, float f) { return (a * (1.0 - f)) + (b * f); }
}  // namespace math
}  // namespace procrock
