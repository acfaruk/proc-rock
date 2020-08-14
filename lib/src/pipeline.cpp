#include "pipeline.h"

namespace procrock {

void Pipeline::setGenerator(std::unique_ptr<Generator> generator) {
  this->generator = std::move(generator);
}
Generator& Pipeline::getGenerator() const { return *this->generator.get(); }

const std::shared_ptr<Mesh> Pipeline::getCurrentMesh() { return this->generator->getMesh(); }
}  // namespace procrock