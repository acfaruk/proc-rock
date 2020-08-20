#include "parameterizer.h"

namespace procrock {
std::shared_ptr<Mesh> Parameterizer::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = parameterize(before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

bool Parameterizer::isMoveable() const { return false; }
bool Parameterizer::isRemovable() const { return false; }
}  // namespace procrock
