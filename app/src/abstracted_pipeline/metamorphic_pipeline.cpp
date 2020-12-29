#include "metamorphic_pipeline.h"

#include <math.h>

#include "../math.h"

namespace procrock {
MetamorphicPipeline::MetamorphicPipeline() {
  Configuration::ConfigurationGroup presetGroup;
  presetGroup.entry = {"Presets", "Apply Metamorhic Presets"};
  presetGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Use Preset", "Set the rock parameters based on the metamorphic class."}, &usePreset});
  presetGroup.singleChoices.push_back(Configuration::SingleChoiceEntry{
      {"Preset", "Choose the preset to apply.", [&]() { return usePreset; }},
      {{"Slate",
        "Foliated metamorhic rock, resulting from low grade metamorphism on shale, mudstone or "
        "siltstone."},
       {"Phyllite",
        "Foliated metamorhic rock, resulting from low-mid grade metamorphism on shale, mudstone or "
        "siltstone."},
       {"Schist",
        "Foliated metamorhic rock, resulting from mid grade metamorphism on shale, mudstone or "
        "siltstone."},
       {"Gneiss",
        "Foliated metamorhic rock, resulting from high grade metamorphism on shale, granite or "
        "volcanic rocks."},
       {"Marble", "Nonfoliated metamorhic rock, resulting from limestone or dolostone."},
       {"Hornfels", "Nonfoliated metamorhic rock, resulting often from shale."}},
      &presetChoice});

  Configuration::ConfigurationGroup group;
  group.entry = {"General", "General Settings to change the form of the rock."};
  group.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Change the form drastically."}, &seed, 0, 1000000});
  group.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Cut Ground", "Should the ground be cut from the rock?"}, &cutGround});

  Configuration::ConfigurationGroup grainGroup;
  grainGroup.entry = {"Grains", "Change settings related to grain sizes and colors."};

  grainGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Base Size", "Size of the base grains."}, &baseGrainSize, 0, 1});
  grainGroup.colors.push_back(
      Configuration::SimpleEntry<Eigen::Vector3f>{{"Base", "Base color of grains."}, &baseColor});
  grainGroup.colors.push_back(Configuration::SimpleEntry<Eigen::Vector3f>{
      {"Secondary", "Secondary color of grains."}, &secondaryColor});
  grainGroup.colors.push_back(Configuration::SimpleEntry<Eigen::Vector3f>{
      {"Tertiary", "Tertiary color of grains."}, &tertiaryColor});

  Configuration::ConfigurationGroup foliationGroup;
  foliationGroup.entry = {"Foliation", "Settings regarding foliation."};

  foliationGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Size", "Size of the foliation."}, &foliation.size, 0, 1});
  foliationGroup.float3s.push_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Direction", "Modify the main foliation direction."},
      &foliation.direction,
      {-1, -1, -1},
      {1, 1, 1}});
  foliationGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Wavyness", "Controls how strong the \"waves\" in the foliation are."},
      &foliation.wavyness,
      0,
      1});

  foliationGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Scalyness", "Gives the foliation a \"scaly\" look."}, &foliation.scalyness, 0, 1});

  config.insertToConfigGroups("Preset", presetGroup);
  config.insertToConfigGroups("Form", group);
  config.insertToConfigGroups("Texture", grainGroup);
  config.insertToConfigGroups("Texture", foliationGroup);

  textureExtrasExtender.addOwnGroups(config, "Texture");
}

void MetamorphicPipeline::setupPipeline() {
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

  textureExtrasExtender.setupPipeline(pipeline);
}

void MetamorphicPipeline::updatePipeline() {
  // Form
  generator->seed = this->seed + 10;
  generator->pointAmount = 100;
  generator->pointSize = 0.08;
  generator->shrinkFactor = 0.4;
  generator->setChanged(true);

  modCutGround->setDisabled(!cutGround);
  modCutGround->rotation = Eigen::Vector3f(0, 0, (6.0 / 4.0) * M_PI);

  modDecimate->relativeValue = 0.3;

  if (cutGround) {
    modTransform->translation = Eigen::Vector3f(0, -0.5, 0);
  } else {
    modTransform->translation = Eigen::Vector3f(0, 0, 0);
  }

  updateTextureGenerator();
  textureExtrasExtender.updatePipeline(pipeline);
}

void MetamorphicPipeline::updateTextureGenerator() {
  auto texGenConfigs = textureGenerator->getConfiguration();

  // Texture: Noise
  auto& noise = textureGenerator->noiseGraph;
  noise.clear();

  auto billowNoiseNode = std::make_unique<BillowNoiseNode>();
  auto billowNoiseNodePtr = billowNoiseNode.get();
  auto billowNoiseNodeId = noise.addNode(std::move(billowNoiseNode));

  billowNoiseNodePtr->frequency = 1000;
  billowNoiseNodePtr->lacunarity = 3.5;
  billowNoiseNodePtr->persistence = 0.18;
  billowNoiseNodePtr->octaveCount = 1;

  int output = noise.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {400, 0});
  noise.addEdge(billowNoiseNodeId, output);

  auto roughnessBias =
      texGenConfigs.getConfigGroup("Roughness", "Greyscale Based Roughness").getInt("Bias");
  *roughnessBias = 255;

  auto trueMetal =
      texGenConfigs.getConfigGroup("Metalness", "Greyscale Based Metallness").getBool("True Metal");
  *trueMetal = true;

  auto metalBias =
      texGenConfigs.getConfigGroup("Metalness", "Greyscale Based Metallness").getInt("Bias");
  *metalBias = -255;

  // Texture: Coloring
  auto coloring =
      texGenConfigs.getConfigGroup("Albedo", "Gradient Coloring").getGradientColoring("Gradient");
  coloring->clear();

  (*coloring)[0] = secondaryColor;
  (*coloring)[100] = tertiaryColor;
  (*coloring)[65] = baseColor;
}

}  // namespace procrock
