#include "generator.h"

namespace procrock {
std::shared_ptr<Mesh> Generator::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate();
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}
}  // namespace procrock