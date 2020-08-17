#include "modifier.h"

namespace procrock {
std::shared_ptr<Mesh> Modifier::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = modify(*before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}
}  // namespace procrock