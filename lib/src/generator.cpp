#include "generator.h"

namespace procrock {
std::shared_ptr<Mesh> Generator::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate();
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

// Generators can be neither moved nor removed from the pipeline
bool Generator::isMoveable() const { return false; }
bool Generator::isRemovable() const { return false; }
}  // namespace procrock