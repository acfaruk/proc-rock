#include "igneous_pipeline.h"

#include <math.h>

namespace procrock {
IgneousPipeline::IgneousPipeline() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General", "General Settings to change the form of the rock."};
  group.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Change the form drastically."}, &seed, 0, 1000000});
  group.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Cut Ground", "Should the ground be cut from the rock?"}, &cutGround});

  Configuration::ConfigurationGroup grainGroup;
  grainGroup.entry = {"Grains", "Change settings related to grain sizes and colors."};
  grainGroup.singleChoices.push_back(Configuration::SingleChoiceEntry{
      {"Grain Type", "Select which kind of grain texture the igneous rock should have."},
      {{"Aphanitic", "Fine-grained texture"},
       {"Phaneritic", "Coarse-grained texture"},
       {"Porphyritic", "Two distinct grain sizes."}},
      &grainChoice});

  config.insertToConfigGroups("Form", group);
  config.insertToConfigGroups("Texture", grainGroup);
}

void IgneousPipeline::setupPipeline() {
  auto gen = std::make_unique<SkinSurfaceGenerator>();
  this->generator = gen.get();
  this->pipeline->setGenerator(std::move(gen));

  auto mod0 = std::make_unique<CutPlaneModifier>();
  this->modCutGround = mod0.get();
  this->pipeline->addModifier(std::move(mod0));

  auto mod1 = std::make_unique<DecimateModifier>();
  this->modDecimate = mod1.get();
  this->pipeline->addModifier(std::move(mod1));

  auto mod2 = std::make_unique<TransformationModifier>();
  this->modTransform = mod2.get();
  this->pipeline->addModifier(std::move(mod2));

  auto texgen = std::make_unique<NoiseTextureGenerator>();
  this->textureGenerator = texgen.get();
  this->pipeline->setTextureGenerator(std::move(texgen));
}

void IgneousPipeline::updatePipeline() {
  // Form
  generator->seed = this->seed + 10;
  generator->pointAmount = 100;
  generator->pointSize = 0.08;
  generator->setChanged(true);

  modCutGround->setDisabled(!cutGround);
  modCutGround->rotation = Eigen::Vector3f(0, 0, (6.0 / 4.0) * M_PI);

  modDecimate->relativeValue = 0.3;

  if (cutGround) {
    modTransform->translation = Eigen::Vector3f(0, -0.5, 0);
  } else {
    modTransform->translation = Eigen::Vector3f(0, 0, 0);
  }

  auto texGenConfigs = textureGenerator->getConfiguration();

  // Texture: Noise
  auto& noise = textureGenerator->noiseGraph;
  noise.clear();

  switch (grainChoice) {
    case 0:  // Fine grained
    {
      auto billowNoiseNode = std::make_unique<BillowNoiseNode>();
      auto billowNoiseNodePtr = billowNoiseNode.get();
      auto billowNoiseNodeId = noise.addNode(std::move(billowNoiseNode));

      billowNoiseNodePtr->frequency = 1000;
      billowNoiseNodePtr->lacunarity = 3.5;
      billowNoiseNodePtr->persistence = 0.18;
      billowNoiseNodePtr->octaveCount = 1;

      int output = noise.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {400, 0});
      noise.addEdge(billowNoiseNodeId, output);
    } break;
    case 1:
      break;
    case 2:
      break;
    default:
      assert(0 && "handle all cases");
  }

  // Roughness = 255 everywhere
  auto roughnessBias =
      texGenConfigs.getConfigGroup("Roughness", "Greyscale Based Roughness").getInt("Bias");
  *roughnessBias = 255;

  // Texture: Coloring
  auto coloring =
      texGenConfigs.getConfigGroup("Albedo", "Gradient Coloring").getGradientColoring("Gradient");
  coloring->clear();

  (*coloring)[0] = Eigen::Vector3f{1.0, 1.0, 1.0};
  (*coloring)[100] = Eigen::Vector3f{0.0, 0.0, 0.0};
  (*coloring)[65] = Eigen::Vector3f(0.4, 0.2, 0.1);  // TODO: User choice
}
}  // namespace procrock
