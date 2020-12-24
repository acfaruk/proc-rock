#include "sedimentary_pipeline.h"

#include <math.h>

#include "../math.h"

namespace procrock {
SedimentaryPipeline::SedimentaryPipeline() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General", "General Settings to change the form of the rock."};
  group.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Change the form drastically."}, &seed, 0, 1000000});
  group.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Cut Ground", "Should the ground be cut from the rock?"}, &cutGround});

  group.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Layered", "Approximate layers of sediment."}, &layered});

  Configuration::ConfigurationGroup layeringGroup;
  auto layeringFunc = [&]() { return layered; };
  layeringGroup.entry = {"Layering", "Settings affecting the layered structure.", layeringFunc};
  layeringGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Layering frequency / density"}, &layerFrequency, 1, 12});
  layeringGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Strength", "Amount of extrusion/intrusion for the layers."}, &layerStrength, 0.1, 0.4});

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
  grainGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Secondary Grains", "Use secondary grains."}, &useSecondaryGrains});
  grainGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Tertiary Grains", "Use tertiary grains."}, &useTertiaryGrains});
  grainGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Quaternary Grains", "Use quaternary grains."}, &useQuaternaryGrains});

  Configuration::ConfigurationGroup secondaryGrainsGroup;
  auto secondaryGrainsFunc = [&]() { return useSecondaryGrains; };
  secondaryGrainsGroup.entry = {"Secondary Grains", "Modify Grains", secondaryGrainsFunc};
  secondaryGrainsGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Grain Size", "Set the size of the grains."}, &secondaryGrainSize, 0, 1});
  secondaryGrainsGroup.colors.push_back(Configuration::SimpleEntry<Eigen::Vector3f>{
      {"Color", "Color of the grains."}, &secondaryGrainsColor});

  Configuration::ConfigurationGroup tertiaryGrainsGroup;
  auto tertiaryGrainsFunc = [&]() { return useTertiaryGrains; };
  tertiaryGrainsGroup.entry = {"Tertiary Grains", "Modify Grains", tertiaryGrainsFunc};
  tertiaryGrainsGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Grain Size", "Set the size of the grains."}, &tertiaryGrainSize, 0, 1});
  tertiaryGrainsGroup.colors.push_back(Configuration::SimpleEntry<Eigen::Vector3f>{
      {"Color", "Color of the grains."}, &tertiaryGrainsColor});

  Configuration::ConfigurationGroup quaternaryGrainsGroup;
  auto quaternaryGrainsFunc = [&]() { return useQuaternaryGrains; };
  quaternaryGrainsGroup.entry = {"Quaternary Grains", "Modify Grains", quaternaryGrainsFunc};
  quaternaryGrainsGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Grain Size", "Set the size of the grains."}, &quaternaryGrainSize, 0, 1});
  quaternaryGrainsGroup.colors.push_back(Configuration::SimpleEntry<Eigen::Vector3f>{
      {"Color", "Color of the grains."}, &quaternaryGrainsColor});

  Configuration::ConfigurationGroup textureExtrasGroup;
  textureExtrasGroup.entry = {"Extras", "Extras to add to the texture."};
  textureExtrasGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Variance", "Add some variance to the texture to make it appear more natural."},
      &textureVariance});
  textureExtrasGroup.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Moss", "Add moss to the rock."}, &textureMoss});

  config.insertToConfigGroups("Form", group);
  config.insertToConfigGroups("Form", layeringGroup);
  config.insertToConfigGroups("Texture", grainGroup);
  config.insertToConfigGroups("Texture", secondaryGrainsGroup);
  config.insertToConfigGroups("Texture", tertiaryGrainsGroup);
  config.insertToConfigGroups("Texture", quaternaryGrainsGroup);
  config.insertToConfigGroups("Texture", textureExtrasGroup);
}

void SedimentaryPipeline::setupPipeline() {
  auto gen = std::make_unique<SkinSurfaceGenerator>();
  this->generator = gen.get();
  this->pipeline->setGenerator(std::move(gen));

  auto mod0 = std::make_unique<CutPlaneModifier>();
  this->modCutGround = mod0.get();
  this->pipeline->addModifier(std::move(mod0));

  auto mod1 = std::make_unique<SubdivisionModifier>();
  this->modSubdivision = mod1.get();
  this->pipeline->addModifier(std::move(mod1));

  auto mod2 = std::make_unique<DisplaceAlongNormalsModifier>();
  this->modDisplaceAlongNormals = mod2.get();
  this->pipeline->addModifier(std::move(mod2));

  auto mod3 = std::make_unique<DecimateModifier>();
  this->modDecimate = mod3.get();
  this->pipeline->addModifier(std::move(mod3));

  auto mod4 = std::make_unique<TransformationModifier>();
  this->modTransform = mod4.get();
  this->pipeline->addModifier(std::move(mod4));

  auto texgen = std::make_unique<NoiseTextureGenerator>();
  this->textureGenerator = texgen.get();
  this->pipeline->setTextureGenerator(std::move(texgen));

  auto texadd0 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderGrainsSecondary = texadd0.get();
  this->pipeline->addTextureAdder(std::move(texadd0));

  auto texadd1 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderGrainsTertiary = texadd1.get();
  this->pipeline->addTextureAdder(std::move(texadd1));

  auto texadd2 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderGrainsQuaternary = texadd2.get();
  this->pipeline->addTextureAdder(std::move(texadd2));

  auto texadd3 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderVariance = texadd3.get();
  this->pipeline->addTextureAdder(std::move(texadd3));

  auto texadd4 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderMoss = texadd4.get();
  this->pipeline->addTextureAdder(std::move(texadd4));
}

void SedimentaryPipeline::updatePipeline() {
  // Form
  generator->seed = this->seed + 10;
  generator->pointAmount = 100;
  generator->pointSize = 0.08;
  generator->shrinkFactor = 0.4;
  generator->setChanged(true);

  modCutGround->setDisabled(!cutGround);
  modCutGround->rotation = Eigen::Vector3f(0, 0, (6.0 / 4.0) * M_PI);

  modDecimate->relativeValue = 0.3;

  // pipeline->getParameterizer().textureSizeChoice = 3;

  if (cutGround) {
    modTransform->translation = Eigen::Vector3f(0, -0.5, 0);
  } else {
    modTransform->translation = Eigen::Vector3f(0, 0, 0);
  }

  modSubdivision->setDisabled(!layered);

  updateModifierDisplaceAlongNormals();

  modDecimate->relativeValue = 0.25;

  updateTextureGenerator();
  updateTextureAdderGrainsSecondary();
  updateTextureAdderGrainsTertiary();
  updateTextureAdderGrainsQuaternary();
  updateTextureAdderVariance();
  updateTextureAdderMoss();
}

int SedimentaryPipeline::createLayerNoise(NoiseGraph* noise) {
  auto ridgedMultiNode = std::make_unique<RidgedMultiNoiseNode>();
  auto ridgetMultiNodePtr = ridgedMultiNode.get();
  auto ridgetMultiNodeId = noise->addNode(std::move(ridgedMultiNode), false, {0, 700});

  ridgetMultiNodePtr->frequency = layerFrequency;
  ridgetMultiNodePtr->lacunarity = 2.5;
  ridgetMultiNodePtr->octaveCount = 3;

  auto scalePointNode = std::make_unique<ScalePointNoiseNode>();
  auto scalePointNodePtr = scalePointNode.get();
  auto scalePointNodeId = noise->addNode(std::move(scalePointNode), false, {300, 700});

  scalePointNodePtr->xScale = 0.05;
  scalePointNodePtr->yScale = 1;
  scalePointNodePtr->zScale = 0.05;
  noise->addEdge(ridgetMultiNodeId, scalePointNodeId);

  return scalePointNodeId;
}

void SedimentaryPipeline::updateModifierDisplaceAlongNormals() {
  modDisplaceAlongNormals->setDisabled(!layered);
  modDisplaceAlongNormals->factor = layerStrength;
  modDisplaceAlongNormals->selection = 1;  // noise based

  auto modConfigs = modDisplaceAlongNormals->getConfiguration();
  auto noise = modConfigs.getConfigGroup("Noise", "Noise").getNoiseGraph("Noise Graph");
  noise->clear();

  int layerNoiseId = createLayerNoise(noise);

  int outputNodeId =
      noise->addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {400, 0});

  noise->addEdge(layerNoiseId, outputNodeId);
}

void SedimentaryPipeline::updateTextureGenerator() {
  auto texGenConfigs = textureGenerator->getConfiguration();

  // Texture: Noise
  auto& noise = textureGenerator->noiseGraph;
  noise.clear();

  auto billowNoiseNode = std::make_unique<BillowNoiseNode>();
  auto billowNoiseNodePtr = billowNoiseNode.get();
  auto billowNoiseNodeId = noise.addNode(std::move(billowNoiseNode), false);

  billowNoiseNodePtr->frequency = 1000;
  billowNoiseNodePtr->lacunarity = 3.5;
  billowNoiseNodePtr->persistence = 0.18;
  billowNoiseNodePtr->octaveCount = 1;

  auto voronoiNoiseNode = std::make_unique<VoronoiNoiseNode>();
  auto voronoiNoiseNodePtr = voronoiNoiseNode.get();
  auto voronoiNoiseNodeId = noise.addNode(std::move(voronoiNoiseNode), false, {0, 350});

  voronoiNoiseNodePtr->frequency = math::lerp(1, 100, (1 - baseGrainSize));
  voronoiNoiseNodePtr->displacement = 0.33;

  auto addNoiseNode0 = std::make_unique<AddNoiseNode>();
  auto addNoiseNode0Ptr = addNoiseNode0.get();
  auto addNoiseNode0Id = noise.addNode(std::move(addNoiseNode0), false, {300, 200});

  auto addNoiseNode1 = std::make_unique<AddNoiseNode>();
  auto addNoiseNode1Ptr = addNoiseNode1.get();
  auto addNoiseNode1Id = noise.addNode(std::move(addNoiseNode1), false, {500, 200});

  auto layerNoiseId = createLayerNoise(&noise);

  int outputNodeId =
      noise.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {700, 200});

  noise.addEdge(billowNoiseNodeId, addNoiseNode0Id);
  noise.addEdge(voronoiNoiseNodeId, addNoiseNode0Id, 1);
  noise.addEdge(addNoiseNode0Id, addNoiseNode1Id);
  if (layered) {
    noise.addEdge(layerNoiseId, addNoiseNode1Id, 1);
  }
  noise.addEdge(addNoiseNode1Id, outputNodeId);

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
}  // namespace procrock

void SedimentaryPipeline::updateTextureAdderGrainsSecondary() {
  textureAdderGrainsSecondary->setDisabled(!useSecondaryGrains);

  auto config = textureAdderGrainsSecondary->getConfiguration();
  auto& noise = textureAdderGrainsSecondary->noiseGraph;
  noise.clear();

  auto voronoiNoiseNode = std::make_unique<VoronoiNoiseNode>();
  auto voronoiNoiseNodePtr = voronoiNoiseNode.get();
  auto voronoiNoiseNodeId = noise.addNode(std::move(voronoiNoiseNode));

  voronoiNoiseNodePtr->frequency = math::lerp(1, 100, (1 - secondaryGrainSize));
  voronoiNoiseNodePtr->seed = 10;

  int outputNoiseNodeId = noise.addNode(std::make_unique<OutputNoiseNode>(), true, {400, 0});
  noise.addEdge(voronoiNoiseNodeId, outputNoiseNodeId);

  auto coloring = config.getConfigGroup("Albedo", "Gradient Alpha Coloring")
                      .getGradientAlphaColoring("Gradient");
  coloring->clear();

  auto col = secondaryGrainsColor;
  (*coloring)[0] = Eigen::Vector4f{0, 0, 0, 0};
  (*coloring)[40] = Eigen::Vector4f{col.x(), col.y(), col.z(), 0};
  (*coloring)[50] = Eigen::Vector4f{col.x(), col.y(), col.z(), 1};
  (*coloring)[60] = Eigen::Vector4f{col.x(), col.y(), col.z(), 0};
  (*coloring)[100] = Eigen::Vector4f{0, 0, 0, 0};
}

void SedimentaryPipeline::updateTextureAdderGrainsTertiary() {
  textureAdderGrainsTertiary->setDisabled(!useTertiaryGrains);

  auto config = textureAdderGrainsTertiary->getConfiguration();
  auto& noise = textureAdderGrainsTertiary->noiseGraph;
  noise.clear();

  auto voronoiNoiseNode = std::make_unique<VoronoiNoiseNode>();
  auto voronoiNoiseNodePtr = voronoiNoiseNode.get();
  auto voronoiNoiseNodeId = noise.addNode(std::move(voronoiNoiseNode));

  voronoiNoiseNodePtr->frequency = math::lerp(1, 100, (1 - tertiaryGrainSize));
  voronoiNoiseNodePtr->seed = 20;

  int outputNoiseNodeId = noise.addNode(std::make_unique<OutputNoiseNode>(), true, {400, 0});
  noise.addEdge(voronoiNoiseNodeId, outputNoiseNodeId);

  auto coloring = config.getConfigGroup("Albedo", "Gradient Alpha Coloring")
                      .getGradientAlphaColoring("Gradient");
  coloring->clear();

  auto col = tertiaryGrainsColor;
  (*coloring)[0] = Eigen::Vector4f{0, 0, 0, 0};
  (*coloring)[40] = Eigen::Vector4f{col.x(), col.y(), col.z(), 0};
  (*coloring)[50] = Eigen::Vector4f{col.x(), col.y(), col.z(), 1};
  (*coloring)[60] = Eigen::Vector4f{col.x(), col.y(), col.z(), 0};
  (*coloring)[100] = Eigen::Vector4f{0, 0, 0, 0};
}

void SedimentaryPipeline::updateTextureAdderGrainsQuaternary() {
  textureAdderGrainsQuaternary->setDisabled(!useQuaternaryGrains);

  auto config = textureAdderGrainsQuaternary->getConfiguration();
  auto& noise = textureAdderGrainsQuaternary->noiseGraph;
  noise.clear();

  auto voronoiNoiseNode = std::make_unique<VoronoiNoiseNode>();
  auto voronoiNoiseNodePtr = voronoiNoiseNode.get();
  auto voronoiNoiseNodeId = noise.addNode(std::move(voronoiNoiseNode));

  voronoiNoiseNodePtr->frequency = math::lerp(1, 100, (1 - quaternaryGrainSize));
  voronoiNoiseNodePtr->seed = 30;

  int outputNoiseNodeId = noise.addNode(std::make_unique<OutputNoiseNode>(), true, {400, 0});
  noise.addEdge(voronoiNoiseNodeId, outputNoiseNodeId);

  auto coloring = config.getConfigGroup("Albedo", "Gradient Alpha Coloring")
                      .getGradientAlphaColoring("Gradient");
  coloring->clear();

  auto col = quaternaryGrainsColor;
  (*coloring)[0] = Eigen::Vector4f{0, 0, 0, 0};
  (*coloring)[40] = Eigen::Vector4f{col.x(), col.y(), col.z(), 0};
  (*coloring)[50] = Eigen::Vector4f{col.x(), col.y(), col.z(), 1};
  (*coloring)[60] = Eigen::Vector4f{col.x(), col.y(), col.z(), 0};
  (*coloring)[100] = Eigen::Vector4f{0, 0, 0, 0};
}

void SedimentaryPipeline::updateTextureAdderVariance() {
  textureAdderVariance->setDisabled(!textureVariance);

  auto config = textureAdderVariance->getConfiguration();
  auto& noise = textureAdderVariance->noiseGraph;
  noise.clear();

  auto perlinNoiseNode = std::make_unique<PerlinNoiseNode>();
  auto perlinNoiseNodePtr = perlinNoiseNode.get();
  auto perlinNoiseNodeId = noise.addNode(std::move(perlinNoiseNode));

  perlinNoiseNodePtr->frequency = 0.7;
  perlinNoiseNodePtr->lacunarity = 3.5;
  perlinNoiseNodePtr->persistence = 0.4;

  int outputNoiseNodeId = noise.addNode(std::make_unique<OutputNoiseNode>(), true, {400, 0});
  noise.addEdge(perlinNoiseNodeId, outputNoiseNodeId);

  auto coloring = config.getConfigGroup("Albedo", "Gradient Alpha Coloring")
                      .getGradientAlphaColoring("Gradient");
  coloring->clear();

  (*coloring)[30] = Eigen::Vector4f{0, 0, 0, 0};
  (*coloring)[70] = Eigen::Vector4f{0, 0, 0, 0.5};
  (*coloring)[100] = Eigen::Vector4f{0, 0, 0, 0};
}

void SedimentaryPipeline::updateTextureAdderMoss() {
  textureAdderMoss->setDisabled(!textureMoss);
  auto config = textureAdderMoss->getConfiguration();
  auto& noise = textureAdderMoss->noiseGraph;
  noise.clear();

  auto perlinNoiseNode = std::make_unique<PerlinNoiseNode>();
  auto perlinNoiseNodePtr = perlinNoiseNode.get();
  auto perlinNoiseNodeId = noise.addNode(std::move(perlinNoiseNode));

  perlinNoiseNodePtr->frequency = 6;
  perlinNoiseNodePtr->lacunarity = 2.5;
  perlinNoiseNodePtr->persistence = 0.35;

  int outputNoiseNodeId = noise.addNode(std::make_unique<OutputNoiseNode>(), true, {400, 0});
  noise.addEdge(perlinNoiseNodeId, outputNoiseNodeId);

  auto coloring = config.getConfigGroup("Albedo", "Gradient Alpha Coloring")
                      .getGradientAlphaColoring("Gradient");
  coloring->clear();

  (*coloring)[0] = Eigen::Vector4f{0.1, 0.5, 0, 0};
  (*coloring)[100] = Eigen::Vector4f{0, 0.2, 0, 0.5};

  auto useDirection =
      config.getConfigGroup("Preferred Normal Direction", "Prefer specific normal directions.")
          .getBool("Use preferred normal directions.");
  *useDirection = true;
}
}  // namespace procrock
