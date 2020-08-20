#include "modifier.h"

namespace procrock {
std::shared_ptr<Mesh> Modifier::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = modify(*before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

// Modifiers can be moved and removed from the pipeline
bool Modifier::isMoveable() const { return true; }
bool Modifier::isRemovable() const { return true; }
}  // namespace procrock