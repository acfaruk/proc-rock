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

  foliationGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Enabled", "Enable foliation in the texture."}, &foliation.enabled});
  foliationGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Size", "Size of the foliation."}, &foliation.size, 0, 1});
  foliationGroup.float3s.push_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Rotation", "Modify the main foliation direction."},
      &foliation.rotation,
      {0, 0, 0},
      {180, 180, 180}});
  foliationGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Wavyness", "Controls how strong the \"waves\" in the foliation are."},
      &foliation.wavyness,
      0,
      1});

  foliationGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Scalyness", "Gives the foliation a \"scaly\" look."}, &foliation.scalyness, 0, 1});

  formGeneratorExtender.addOwnGroups(config, "Form");
  config.insertToConfigGroups("Preset", presetGroup);
  config.insertToConfigGroups("Texture", grainGroup);
  config.insertToConfigGroups("Texture", foliationGroup);
  textureExtrasExtender.addOwnGroups(config, "Texture");
}

void MetamorphicPipeline::setupPipeline() {
  formGeneratorExtender.setupPipeline(pipeline);

  auto texgen = std::make_unique<NoiseTextureGenerator>();
  this->textureGenerator = texgen.get();
  this->pipeline->setTextureGenerator(std::move(texgen));

  textureExtrasExtender.setupPipeline(pipeline);
}

void MetamorphicPipeline::updatePipeline() {
  if (usePreset) {
    setParametersFromPreset();
  }

  formGeneratorExtender.updatePipeline(pipeline);
  updateTextureGenerator();
  textureExtrasExtender.updatePipeline(pipeline);
}

int MetamorphicPipeline::createFoliationBaseNoise(NoiseGraph* noise) {
  auto ridgedMultiNoiseNode = std::make_unique<RidgedMultiNoiseNode>();
  auto ridgedMultiNoiseNodePtr = ridgedMultiNoiseNode.get();
  auto ridgedMultiNoiseNodeId = noise->addNode(std::move(ridgedMultiNoiseNode), false, {250, 200});

  auto scalePointNoiseNode = std::make_unique<ScalePointNoiseNode>();
  auto scalePointNoiseNodePtr = scalePointNoiseNode.get();
  auto scalePointNoiseNodeId = noise->addNode(std::move(scalePointNoiseNode), false, {500, 200});

  ridgedMultiNoiseNodePtr->frequency = 9;
  scalePointNoiseNodePtr->xScale = 0.1;
  scalePointNoiseNodePtr->zScale = 0.1;

  noise->addEdge(ridgedMultiNoiseNodeId, scalePointNoiseNodeId);

  return scalePointNoiseNodeId;
}

int MetamorphicPipeline::createFoliationWaveNoise(NoiseGraph* noise) {
  auto perlinNoiseNode = std::make_unique<PerlinNoiseNode>();
  auto perlinNoiseNodePtr = perlinNoiseNode.get();
  auto perlinNoiseNodeId = noise->addNode(std::move(perlinNoiseNode), false, {0, 0});

  perlinNoiseNodePtr->frequency = math::lerp(0, 9, foliation.wavyness);

  auto scalePointNoiseNode = std::make_unique<ScalePointNoiseNode>();
  auto scalePointNoiseNodePtr = scalePointNoiseNode.get();
  auto scalePointNoiseNodeId = noise->addNode(std::move(scalePointNoiseNode), false, {250, 0});

  scalePointNoiseNodePtr->yScale = 0;

  auto scaleBiasNoiseNode = std::make_unique<ScaleBiasNoiseNode>();
  auto scaleBiasNoiseNodePtr = scaleBiasNoiseNode.get();
  auto scaleBiasNoiseNodeId = noise->addNode(std::move(scaleBiasNoiseNode), false, {500, 0});

  scaleBiasNoiseNodePtr->scale = 0.1;

  noise->addEdge(perlinNoiseNodeId, scalePointNoiseNodeId);
  noise->addEdge(scalePointNoiseNodeId, scaleBiasNoiseNodeId);

  return scaleBiasNoiseNodeId;
}

int MetamorphicPipeline::createMetamorhpicBaseNoise(NoiseGraph* noise) {
  auto foliationBaseNodeId = createFoliationBaseNoise(noise);
  auto foliationWaveNodeId = createFoliationWaveNoise(noise);

  auto displaceNoiseNode = std::make_unique<DisplaceNoiseNode>();
  auto displaceNoiseNodePtr = displaceNoiseNode.get();
  auto displaceNoiseNodeId = noise->addNode(std::move(displaceNoiseNode), false, {750, 0});

  auto rotatePointNoiseNode = std::make_unique<RotatePointNoiseNode>();
  auto rotatePointNoiseNodePtr = rotatePointNoiseNode.get();
  auto rotatePointNoiseNodeId = noise->addNode(std::move(rotatePointNoiseNode), false, {1000, 0});

  rotatePointNoiseNodePtr->xAngle = foliation.rotation.x();
  rotatePointNoiseNodePtr->yAngle = foliation.rotation.y();
  rotatePointNoiseNodePtr->zAngle = foliation.rotation.z();

  auto voronoiNoiseNode = std::make_unique<VoronoiNoiseNode>();
  auto voronoiNoiseNodePtr = voronoiNoiseNode.get();
  auto voronoiNoiseNodeId = noise->addNode(std::move(voronoiNoiseNode), false, {1000, 200});

  voronoiNoiseNodePtr->frequency = 10;
  voronoiNoiseNodePtr->displacement = math::lerp(0.1, 0.7, foliation.scalyness);

  auto minNoiseNode = std::make_unique<MinNoiseNode>();
  auto minNoiseNodePtr = minNoiseNode.get();
  auto minNoiseNodeId = noise->addNode(std::move(minNoiseNode), false, {1250, 0});

  auto scalePointNoiseNode = std::make_unique<ScalePointNoiseNode>();
  auto scalePointNoiseNodePtr = scalePointNoiseNode.get();
  auto scalePointNoiseNodeId = noise->addNode(std::move(scalePointNoiseNode), false, {1500, 0});

  scalePointNoiseNodePtr->xScale = math::lerp(1, 40, 1 - foliation.size);
  scalePointNoiseNodePtr->yScale = math::lerp(1, 40, 1 - foliation.size);
  scalePointNoiseNodePtr->zScale = math::lerp(1, 40, 1 - foliation.size);

  noise->addEdge(foliationBaseNodeId, displaceNoiseNodeId, 3);
  noise->addEdge(foliationWaveNodeId, displaceNoiseNodeId, 1);
  noise->addEdge(displaceNoiseNodeId, rotatePointNoiseNodeId);

  if (foliation.scalyness >= 0.01) {
    noise->addEdge(rotatePointNoiseNodeId, minNoiseNodeId, 0);
    noise->addEdge(voronoiNoiseNodeId, minNoiseNodeId, 1);
    noise->addEdge(minNoiseNodeId, scalePointNoiseNodeId, 0);
  } else {
    noise->addEdge(rotatePointNoiseNodeId, scalePointNoiseNodeId);
  }

  return scalePointNoiseNodeId;
}

int MetamorphicPipeline::createGrainsBaseNoise(NoiseGraph* noise) {
  auto billowNoiseNode = std::make_unique<BillowNoiseNode>();
  auto billowNoiseNodePtr = billowNoiseNode.get();
  auto billowNoiseNodeId = noise->addNode(std::move(billowNoiseNode), false, {1000, 400});

  billowNoiseNodePtr->frequency = 1000;

  auto voronoiNoiseNode = std::make_unique<VoronoiNoiseNode>();
  auto voronoiNoiseNodePtr = voronoiNoiseNode.get();
  auto voronoiNoiseNodeId = noise->addNode(std::move(voronoiNoiseNode), false, {1000, 600});

  voronoiNoiseNodePtr->frequency = math::lerp(40, 100, 1 - baseGrainSize);
  if (baseGrainSize <= 0.01) {
    voronoiNoiseNodePtr->frequency = 0;
  }

  auto addNoiseNode = std::make_unique<AddNoiseNode>();
  auto addNoiseNodePtr = addNoiseNode.get();
  auto addNoiseNodeId = noise->addNode(std::move(addNoiseNode), false, {1250, 400});

  auto scaleBiasNoiseNode = std::make_unique<ScaleBiasNoiseNode>();
  auto scaleBiasNoiseNodePtr = scaleBiasNoiseNode.get();
  auto scaleBiasNoiseNodeId = noise->addNode(std::move(scaleBiasNoiseNode), false, {1500, 400});

  scaleBiasNoiseNodePtr->scale = 0.5;

  noise->addEdge(billowNoiseNodeId, addNoiseNodeId, 0);
  noise->addEdge(voronoiNoiseNodeId, addNoiseNodeId, 1);
  noise->addEdge(addNoiseNodeId, scaleBiasNoiseNodeId);

  return scaleBiasNoiseNodeId;
}

void MetamorphicPipeline::updateTextureGenerator() {
  auto texGenConfigs = textureGenerator->getConfiguration();

  // Texture: Noise
  auto& noise = textureGenerator->noiseGraph;
  noise.clear();

  int grainsBaseId = createGrainsBaseNoise(&noise);
  int metaBaseId = createMetamorhpicBaseNoise(&noise);

  auto addNoiseNode = std::make_unique<AddNoiseNode>();
  auto addNoiseNodePtr = addNoiseNode.get();
  auto addNoiseNodeId = noise.addNode(std::move(addNoiseNode), false, {2000, 0});

  int output = noise.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {2250, 0});

  if (foliation.enabled) {
    noise.addEdge(metaBaseId, addNoiseNodeId, 0);
  }
  noise.addEdge(grainsBaseId, addNoiseNodeId, 1);
  noise.addEdge(addNoiseNodeId, output);

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

void MetamorphicPipeline::setParametersFromPreset() {
  switch (presetChoice) {
    case 0: {  // Slate
      baseGrainSize = 0;
      foliation.enabled = true;
      foliation.size = 0;
      foliation.wavyness = 0;
      foliation.scalyness = 0;

      baseColor = Eigen::Vector3f{0, 0, 0};
      secondaryColor = Eigen::Vector3f{0.5, 0.58, 0.59};
      tertiaryColor = Eigen::Vector3f{1, 1, 1};
      break;
    }
    case 1: {  // Phyllite
      baseGrainSize = 0;
      foliation.enabled = true;
      foliation.size = 0.2;
      foliation.wavyness = 0.3;
      foliation.scalyness = 0;

      baseColor = Eigen::Vector3f{0.49, 0.46, 0.27};
      secondaryColor = Eigen::Vector3f{0.4, 0.38, 0.24};
      tertiaryColor = Eigen::Vector3f{0.9, 0.9, 0.9};
      break;
    }
    case 2: {  // Schist
      baseGrainSize = 0.2;
      foliation.enabled = true;
      foliation.size = 0.4;
      foliation.wavyness = 0.05;
      foliation.scalyness = 0.1;

      baseColor = Eigen::Vector3f{0.1, 0.1, 0.1};
      secondaryColor = Eigen::Vector3f{0.96, 0.96, 0.96};
      tertiaryColor = Eigen::Vector3f{0.24, 0.24, 0.24};
      break;
    }
    case 3: {  // Gneiss
      baseGrainSize = 0.3;
      foliation.enabled = true;
      foliation.size = 0.95;
      foliation.wavyness = 0.1;
      foliation.scalyness = 0;

      baseColor = Eigen::Vector3f{0.1, 0.1, 0.1};
      secondaryColor = Eigen::Vector3f{0.96, 0.96, 0.96};
      tertiaryColor = Eigen::Vector3f{0.24, 0.24, 0.24};
      break;
      break;
    }
    case 4: {  // Marble
      baseGrainSize = 0.05;
      foliation.enabled = false;

      baseColor = Eigen::Vector3f{0.67, 0.67, 0.67};
      secondaryColor = Eigen::Vector3f{0.96, 0.96, 0.96};
      tertiaryColor = Eigen::Vector3f{0.77, 0.77, 0.77};
      break;
    }
    case 5: {  // Hornfels
      baseGrainSize = 0;
      foliation.enabled = false;

      baseColor = Eigen::Vector3f{0.35, 0.35, 0.35};
      secondaryColor = Eigen::Vector3f{0.67, 0.67, 0.67};
      tertiaryColor = Eigen::Vector3f{0.58, 0.58, 0.58};
      break;
    }
  }
}

}  // namespace procrock
