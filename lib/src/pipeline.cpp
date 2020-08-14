#include "pipeline.h"

namespace procrock {

void Pipeline::setGenerator(std::unique_ptr<Generator> generator) {
  this->generator = std::move(generator);
}
Generator& Pipeline::getGenerator() const { return *this->generator.get(); }

int Pipeline::getModifierCount() { return modifiers.size(); }

void Pipeline::addModifier(std::unique_ptr<Modifier> modifier) {
  this->modifiers.push_back(std::move(modifier));
}

Modifier& Pipeline::getModifier(int index) { return *this->modifiers[index]; }

const std::shared_ptr<Mesh> Pipeline::getCurrentMesh() {
  auto test = this->generator->run();

  auto& mod = modifiers[0];
  if (this->generator->isChanged()) mod->setChanged(true);

  auto mesh = mod->run(test.get());
  return mesh;
}
}  // namespace procrock