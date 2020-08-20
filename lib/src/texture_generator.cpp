#include "texture_generator.h"

namespace procrock {
std::shared_ptr<Mesh> TextureGenerator::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate(before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

// Texture Generators can be neither moved nor removed from the pipeline
bool TextureGenerator::isMoveable() const { return false; }
bool TextureGenerator::isRemovable() const { return false; }
}  // namespace procrock