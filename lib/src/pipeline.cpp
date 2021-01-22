#include "pipeline.h"

#include <algorithm>
#include <fstream>

#include "configurable.h"
#include "export.h"
#include "mod/displace_along_normals_modifier.h"
#include "mod/subdivision_modifier.h"
#include "pipeline_stage_factory.h"
#include "serialization.h"

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
  if (outputEnabled)
    *outputStream << "Running Generator: " << generator->getInfo().name << std::endl;
  bool changed = generator->isChanged() || generator->isFirstRun();
  auto mesh = this->generator->run();
  generator->setChanged(false);
  if (outputEnabled) *outputStream << "Generator Finished" << std::endl << std::endl;

  for (auto& mod : modifiers) {
    if (outputEnabled) *outputStream << "Running Modifier: " << mod->getInfo().name << std::endl;
    mod->setChanged(mod->isChanged() || mod->isFirstRun() || changed);
    changed = mod->isChanged();
    mesh = mod->run(mesh.get());
    mod->setChanged(false);
    if (outputEnabled) *outputStream << "Modifier Finished." << std::endl << std::endl;
  }

  if (outputEnabled) *outputStream << "All Modifiers done." << std::endl << std::endl;

  if (outputEnabled)
    *outputStream << "Running Parameterizer: " << parameterizer->getInfo().name << std::endl;
  parameterizer->setChanged(parameterizer->isChanged() || parameterizer->isFirstRun() || changed);
  changed = parameterizer->isChanged();
  mesh = parameterizer->run(mesh.get());
  parameterizer->setChanged(false);
  if (outputEnabled) *outputStream << "Parameterizer Finished" << std::endl << std::endl;

  if (outputEnabled)
    *outputStream << "Running Texture Generator: " << textureGenerator->getInfo().name << std::endl;
  textureGenerator->setChanged(textureGenerator->isChanged() || textureGenerator->isFirstRun() ||
                               changed);
  changed = textureGenerator->isChanged();
  mesh = textureGenerator->run(mesh.get());
  textureGenerator->setChanged(false);
  if (outputEnabled) *outputStream << "Texture Generator Finished" << std::endl << std::endl;

  for (auto& texadd : textureAdders) {
    if (outputEnabled)
      *outputStream << "Running Texture Adder: " << texadd->getInfo().name << std::endl;
    texadd->setChanged(texadd->isChanged() || texadd->isFirstRun() || changed);
    changed = texadd->isChanged();
    mesh = texadd->run(mesh.get());
    texadd->setChanged(false);
    if (outputEnabled) *outputStream << "Texture Adder Finished" << std::endl << std::endl;
  }
  if (outputEnabled)
    *outputStream << "All Texture Adders done." << std::endl
                  << std::endl
                  << "Pipeline Done" << std::endl
                  << "------------------------" << std::endl;

  currentMesh = mesh;
  return mesh;
}

void Pipeline::clear() {
  this->generator = std::make_unique<CuboidGenerator>();
  this->modifiers.clear();
  this->parameterizer = std::make_unique<XAtlasParameterizer>();
  this->textureGenerator = std::make_unique<NoiseTextureGenerator>();
  this->textureAdders.clear();
}

bool Pipeline::isChanged() {
  bool result = generator->isChanged() || generator->isFirstRun();

  for (auto& mod : modifiers) {
    result |= (mod->isChanged() || mod->isFirstRun());
  }

  result |= (parameterizer->isChanged() || parameterizer->isFirstRun());
  result |= (textureGenerator->isChanged() || textureGenerator->isFirstRun());

  for (auto& texAdd : textureAdders) {
    result |= (texAdd->isChanged() || texAdd->isFirstRun());
  }

  return result;
}

void Pipeline::enableOutput(bool enable) { this->outputEnabled = enable; }

void Pipeline::setOutputStream(std::ostream* stream) { this->outputStream = stream; }

void Pipeline::saveToFile(const std::string filePath) {
  nlohmann::json finalJson;

  nlohmann::json genJson =
      nlohmann::json{{"_id", generator->getInfo().id}, {"config", generator->getConfiguration()}};
  finalJson.update({{"generator", genJson}});

  finalJson.update({{"modifiers", nlohmann::json::array()}});
  for (auto& mod : modifiers) {
    nlohmann::json modJson = nlohmann::json{{"_id", mod->getInfo().id},
                                            {"disabled", mod->isDisabled()},
                                            {"config", mod->getConfiguration()}};
    finalJson.at("modifiers").push_back(modJson);
  }

  nlohmann::json parJson = nlohmann::json{{"_id", parameterizer->getInfo().id},
                                          {"config", parameterizer->getConfiguration()}};
  finalJson.update({{"parameterizer", parJson}});

  nlohmann::json texGenJson = nlohmann::json{{"_id", textureGenerator->getInfo().id},
                                             {"config", textureGenerator->getConfiguration()}};
  finalJson.update({{"textureGenerator", texGenJson}});

  finalJson.update({{"textureAdders", nlohmann::json::array()}});
  for (auto& texAdder : textureAdders) {
    nlohmann::json texAddJson = nlohmann::json{{"_id", texAdder->getInfo().id},
                                               {"disabled", texAdder->isDisabled()},
                                               {"config", texAdder->getConfiguration()}};
    finalJson.at("textureAdders").push_back(texAddJson);
  }

  std::ofstream file;
  file.open(filePath);
  file << finalJson.dump(1);
  file.close();
}

void Pipeline::loadFromFile(const std::string filePath) {
  std::ifstream file;
  file.open(filePath);
  auto json = nlohmann::json::parse(file);
  file.close();

  auto& genJson = json.at("generator");
  setGenerator(createGeneratorFromId(genJson.at("_id").get<int>()));
  fillConfigFromJson(genJson.at("config"), generator->getConfiguration());

  modifiers.clear();
  auto& modsJson = json.at("modifiers");
  for (auto& modJson : modsJson) {
    addModifier(createModifierFromId(modJson.at("_id").get<int>()));
    auto& mod = getModifier(getModifierCount() - 1);
    if (modJson.find("disabled") != modJson.end()) {  // backwards compatible...
      mod.setDisabled(modJson.at("disabled").get<bool>());
    }
    fillConfigFromJson(modJson.at("config"), mod.getConfiguration());
  }

  auto& parJson = json.at("parameterizer");
  setParameterizer(createParameterizerFromId(parJson.at("_id").get<int>()));
  fillConfigFromJson(parJson.at("config"), getParameterizer().getConfiguration());

  auto& texGenJson = json.at("textureGenerator");
  setTextureGenerator(createTextureGeneratorFromId(texGenJson.at("_id").get<int>()));
  fillConfigFromJson(texGenJson.at("config"), getTextureGenerator().getConfiguration());

  textureAdders.clear();
  auto& texAddsJson = json.at("textureAdders");
  for (auto& texAddJson : texAddsJson) {
    addTextureAdder(createTextureAdderFromId(texAddJson.at("_id").get<int>()));
    auto& texAdd = getTextureAdder(getTextureAdderCount() - 1);
    if (texAddJson.find("disabled") != texAddJson.end()) {  // backwards compatible...
      texAdd.setDisabled(texAddJson.at("disabled").get<bool>());
    }
    fillConfigFromJson(texAddJson.at("config"), texAdd.getConfiguration());
  }
}

void Pipeline::exportCurrent(const std::string filePath, ExportSettings settings) {
  if (outputEnabled) *outputStream << "Exporting rock..." << std::endl;
  if (!settings.exportLODs) {
    exportMesh(*currentMesh, filePath, settings.exportAlbedo, settings.exportNormals,
               settings.exportRoughness, settings.exportMetal, settings.exportDisplacement,
               settings.exportAmbientOcc);
  } else {
    int originalTextureSizeChoice = parameterizer->textureSizeChoice;

    auto decimateMod = std::make_unique<DecimateModifier>();
    auto* mod = decimateMod.get();

    addModifier(std::move(decimateMod));

    for (int i = settings.lodCount - 1; i >= 0; i--) {
      if (outputEnabled) *outputStream << "Working on LOD " << i << "..." << std::endl;
      mod->relativeValue = std::pow(0.5, settings.lodCount - i - 1);

      auto oldOutput = outputEnabled;
      outputEnabled = false;
      generator->setChanged(true);
      getCurrentMesh();
      outputEnabled = oldOutput;

      const size_t period_idx = filePath.rfind('.');
      std::string changedPath = filePath;
      changedPath.insert(period_idx, "-lod" + std::to_string(i));
      exportMesh(*currentMesh, changedPath, settings.exportAlbedo, settings.exportNormals,
                 settings.exportRoughness, settings.exportMetal, settings.exportDisplacement,
                 settings.exportAmbientOcc);
      if (outputEnabled) *outputStream << "Exported LOD " << i << "..." << std::endl;
      if (settings.lodTextures) {
        parameterizer->textureSizeChoice = std::max(0, parameterizer->textureSizeChoice - 1);
      }
    }

    parameterizer->textureSizeChoice = originalTextureSizeChoice;
    removePipelineStage(mod);
  }

  if (outputEnabled) *outputStream << "Export finished..." << std::endl;
}

}  // namespace procrock