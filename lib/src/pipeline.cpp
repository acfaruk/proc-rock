#include "pipeline.h"

#include <algorithm>

#include "mod/displace_along_normals_modifier.h"
#include "mod/subdivision_modifier.h"

namespace procrock {

void Pipeline::setGenerator(std::unique_ptr<Generator> generator) {
  this->generator = std::move(generator);
}
Generator& Pipeline::getGenerator() const { return *this->generator.get(); }

int Pipeline::getModifierCount() { return modifiers.size(); }

void Pipeline::addModifier(std::unique_ptr<Modifier> modifier) {
  this->modifiers.push_back(std::move(modifier));
}

void Pipeline::addModifierFromId(unsigned int id) {
  switch (id) {
    case 0:
      addModifier(std::make_unique<SubdivisionModifier>());
      break;
    case 1:
      addModifier(std::make_unique<DisplaceAlongNormalsModifier>());
      break;
    default:
      assert(false);
      break;
  }
}

void Pipeline::moveModifierUp(PipelineStage* modifier) {
  for (int i = 0; i < modifiers.size(); i++) {
    if (modifiers[i]->getId() == modifier->getId() && i != 0) {
      std::iter_swap(modifiers.begin() + i, modifiers.begin() + i - 1);

      modifiers[i]->setChanged(true);
      generator->setChanged(true);
      return;
    }
  }
}

void Pipeline::moveModifierDown(PipelineStage* modifier) {
  for (int i = 0; i < modifiers.size(); i++) {
    if (modifiers[i]->getId() == modifier->getId() && i != modifiers.size() - 1) {
      std::iter_swap(modifiers.begin() + i, modifiers.begin() + i + 1);

      modifiers[i]->setChanged(true);
      modifiers[i + 1]->setChanged(true);
      generator->setChanged(true);
      return;
    }
  }
}

void Pipeline::removePipelineStage(PipelineStage* stage) {
  for (auto& mod : modifiers) {
    if (static_cast<PipelineStage*>(mod.get()) == stage) {
      modifiers.erase(std::find(modifiers.begin(), modifiers.end(), mod));
      break;
    }
  }

  for (auto& mod : modifiers) {
    mod->setChanged(true);
  }
  generator->setChanged(true);
}

Modifier& Pipeline::getModifier(int index) { return *this->modifiers[index]; }

const std::shared_ptr<Mesh> Pipeline::getCurrentMesh() {
  auto mesh = this->generator->run();

  bool changed = generator->isChanged();

  for (auto& mod : modifiers) {
    mod->setChanged(mod->isChanged() || changed);
    changed = mod->isChanged();
    mesh = mod->run(mesh.get());
  }
  return mesh;
}
}  // namespace procrock