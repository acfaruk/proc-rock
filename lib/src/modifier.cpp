#include "modifier.h"

namespace procrock {
std::shared_ptr<Mesh> Modifier::run(Mesh* before) {
  if (isChanged()) {
    mesh = modify(*before);
  }
  return mesh;
}
}  // namespace procrock