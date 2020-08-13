#pragma once
#include <procrocklib/mesh.h>

#include "../drawable_shape.h"

namespace procrock {
class DrawableMesh : public DrawableShape {
 public:
  DrawableMesh(Mesh& mesh);
};
}  // namespace procrock