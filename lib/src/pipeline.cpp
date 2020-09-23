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

void Pipeline::movePipelineStageUp(PipelineStage* stage) {
  auto modifier = dynamic_cast<Modifier*>(stage);
  if (modifier != nullptr) {
    for (int i = 0; i < modifiers.size(); i++) {
      if (modifiers[i]->getId() == modifier->getId() && i != 0) {
        std::iter_swap(modifiers.begin() + i, modifiers.begin() + i - 1);

        // modifiers[i]->setChanged(true);
        generator->setChanged(true);
        return;
      }
    }
  }
  auto textureAdder = dynamic_cast<TextureAdder*>(stage);
  if (textureAdder != nullptr) {
    for (int i = 0; i < textureAdders.size(); i++) {
      if (textureAdders[i]->getId() == textureAdder->getId() && i != 0) {
        std::iter_swap(textureAdders.begin() + i, textureAdders.begin() + i - 1);

        // textureAdders[i]->setChanged(true);
        generator->setChanged(true);
        return;
      }
    }
  }
}

void Pipeline::movePipelineStageDown(PipelineStage* stage) {
  auto modifier = dynamic_cast<Modifier*>(stage);
  if (modifier != nullptr) {
    for (int i = 0; i < modifiers.size(); i++) {
      if (modifiers[i]->getId() == modifier->getId() && i != modifiers.size() - 1) {
        std::iter_swap(modifiers.begin() + i, modifiers.begin() + i + 1);

        // modifiers[i]->setChanged(true);
        // modifiers[i + 1]->setChanged(true);
        generator->setChanged(true);
        return;
      }
    }
  }

  auto textureAdder = dynamic_cast<TextureAdder*>(stage);
  if (textureAdder != nullptr) {
    for (int i = 0; i < textureAdders.size(); i++) {
      if (textureAdders[i]->getId() == textureAdder->getId() && i != textureAdders.size() - 1) {
        std::iter_swap(textureAdders.begin() + i, textureAdders.begin() + i + 1);

        // textureAdders[i]->setChanged(true);
        generator->setChanged(true);
        return;
      }
    }
  }
}

void Pipeline::removePipelineStage(PipelineStage* stage) {
  for (auto& mod : modifiers) {
    if (dynamic_cast<PipelineStage*>(mod.get()) == stage) {
      modifiers.erase(std::find(modifiers.begin(), modifiers.end(), mod));
      break;
    }
  }

  for (auto& texadd : textureAdders) {
    if (dynamic_cast<PipelineStage*>(texadd.get()) == stage) {
      textureAdders.erase(std::find(textureAdders.begin(), textureAdders.end(), texadd));
      break;
    }
  }

  // for (auto& mod : modifiers) {
  //  mod->setChanged(true);
  //}
  generator->setChanged(true);
}

Modifier& Pipeline::getModifier(int index) { return *this->modifiers[index]; }

void Pipeline::setParameterizer(std::unique_ptr<Parameterizer> parameterizer) {
  this->parameterizer = std::move(parameterizer);
}

Parameterizer& Pipeline::getParameterizer() const { return *this->parameterizer; }

void Pipeline::setTextureGenerator(std::unique_ptr<TextureGenerator> textureGenerator) {
  this->textureGenerator = std::move(textureGenerator);
}

TextureGenerator& Pipeline::getTextureGenerator() const { return *this->textureGenerator; }

int Pipeline::getTextureAdderCount() { return textureAdders.size(); }
void Pipeline::addTextureAdder(std::unique_ptr<TextureAdder> textureAdder) {
  this->textureAdders.push_back(std::move(textureAdder));
}

TextureAdder& Pipeline::getTextureAdder(int index) { return *this->textureAdders[index]; }

const std::shared_ptr<Mesh> Pipeline::getCurrentMesh() {
  bool changed = generator->isChanged() || generator->isFirstRun();
  auto mesh = this->generator->run();
  generator->setChanged(false);

  for (auto& mod : modifiers) {
    mod->setChanged(mod->isChanged() || mod->isFirstRun() || changed);
    changed = mod->isChanged();
    mesh = mod->run(mesh.get());
    mod->setChanged(false);
  }

  parameterizer->setChanged(parameterizer->isChanged() || parameterizer->isFirstRun() || changed);
  changed = parameterizer->isChanged();
  mesh = parameterizer->run(mesh.get());
  parameterizer->setChanged(false);

  textureGenerator->setChanged(textureGenerator->isChanged() || textureGenerator->isFirstRun() ||
                               changed);
  changed = textureGenerator->isChanged();
  mesh = textureGenerator->run(mesh.get());
  textureGenerator->setChanged(false);

  for (auto& texadd : textureAdders) {
    texadd->setChanged(texadd->isChanged() || texadd->isFirstRun() || changed);
    changed = texadd->isChanged();
    mesh = texadd->run(mesh.get());
    texadd->setChanged(false);
  }

  return mesh;
}
}  // namespace procrock