#include "generator.h"

namespace procrock {
std::shared_ptr<Mesh> Generator::getMesh() {
  if (isChanged()) {
    mesh = generate();
  }
  return mesh;
}
}  // namespace procrock