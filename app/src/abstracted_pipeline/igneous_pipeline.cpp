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

  Configuration::ConfigurationGroup textureExtrasGroup;
  textureExtrasGroup.entry = {"Extras", "Extras to add to the texture."};
  textureExtrasGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Variance", "Add some variance to the texture to make it appear more natural."},
      &textureVariance});
  textureExtrasGroup.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Moss", "Add moss to the rock."}, &textureMoss});

  config.insertToConfigGroups("Form", group);
  config.insertToConfigGroups("Texture", grainGroup);
  config.insertToConfigGroups("Texture", textureExtrasGroup);
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

  auto texadd0 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderVariance = texadd0.get();
  this->pipeline->addTextureAdder(std::move(texadd0));

  auto texadd1 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderMoss = texadd1.get();
  this->pipeline->addTextureAdder(std::move(texadd1));
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

  updateTextureGenerator();
  updateTextureAdderVariance();
  updateTextureAdderMoss();
}

void IgneousPipeline::updateTextureGenerator() {
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
    case 1: {  // Coarse grained
      auto billowNoiseNode = std::make_unique<BillowNoiseNode>();
      auto billowNoiseNodePtr = billowNoiseNode.get();
      auto billowNoiseNodeId = noise.addNode(std::move(billowNoiseNode));

      billowNoiseNodePtr->frequency = 1000;
      billowNoiseNodePtr->lacunarity = 3.5;
      billowNoiseNodePtr->persistence = 0.18;
      billowNoiseNodePtr->octaveCount = 1;

      auto perlinNoiseNode = std::make_unique<PerlinNoiseNode>();
      auto perlinNoiseNodePtr = perlinNoiseNode.get();
      auto perlinNoiseNodeId = noise.addNode(std::move(perlinNoiseNode), false, {0, 400});

      perlinNoiseNodePtr->frequency = 60;

      auto addNoiseNode = std::make_unique<AddNoiseNode>();
      auto addNoiseNodePtr = addNoiseNode.get();
      auto addNoiseNodeId = noise.addNode(std::move(addNoiseNode), false, {200, 200});

      int outputNodeId =
          noise.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {400, 0});

      noise.addEdge(billowNoiseNodeId, addNoiseNodeId);
      noise.addEdge(perlinNoiseNodeId, addNoiseNodeId, 1);
      noise.addEdge(addNoiseNodeId, outputNodeId);
    } break;
    case 2: {  // Two grain sizes
      auto billowNoiseNode = std::make_unique<BillowNoiseNode>();
      auto billowNoiseNodePtr = billowNoiseNode.get();
      auto billowNoiseNodeId = noise.addNode(std::move(billowNoiseNode));

      billowNoiseNodePtr->frequency = 1000;
      billowNoiseNodePtr->lacunarity = 3.5;
      billowNoiseNodePtr->persistence = 0.18;
      billowNoiseNodePtr->octaveCount = 1;

      auto perlinNoiseNode = std::make_unique<PerlinNoiseNode>();
      auto perlinNoiseNodePtr = perlinNoiseNode.get();
      auto perlinNoiseNodeId = noise.addNode(std::move(perlinNoiseNode), false, {0, 400});

      perlinNoiseNodePtr->frequency = 60;

      auto addNoiseNode = std::make_unique<AddNoiseNode>();
      auto addNoiseNodePtr = addNoiseNode.get();
      auto addNoiseNodeId = noise.addNode(std::move(addNoiseNode), false, {200, 200});

      auto perlinNoiseNode2 = std::make_unique<PerlinNoiseNode>();
      auto perlinNoiseNode2Ptr = perlinNoiseNode2.get();
      auto perlinNoiseNode2Id = noise.addNode(std::move(perlinNoiseNode2), false, {200, 400});

      perlinNoiseNode2Ptr->frequency = 20;

      auto maxNoiseNode = std::make_unique<MaxNoiseNode>();
      auto maxNoiseNodePtr = maxNoiseNode.get();
      auto maxNoiseNodeId = noise.addNode(std::move(maxNoiseNode), false, {400, 200});

      int outputNodeId =
          noise.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {400, 0});

      noise.addEdge(billowNoiseNodeId, addNoiseNodeId);
      noise.addEdge(perlinNoiseNodeId, addNoiseNodeId, 1);
      noise.addEdge(addNoiseNodeId, maxNoiseNodeId);
      noise.addEdge(perlinNoiseNode2Id, maxNoiseNodeId, 1);
      noise.addEdge(maxNoiseNodeId, outputNodeId);
    } break;
    default:
      assert(0 && "handle all cases");
  }

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

  (*coloring)[0] = Eigen::Vector3f{1.0, 1.0, 1.0};
  (*coloring)[100] = Eigen::Vector3f{0.0, 0.0, 0.0};
  (*coloring)[65] = Eigen::Vector3f(0.4, 0.2, 0.1);  // TODO: User choice
}

void IgneousPipeline::updateTextureAdderVariance() {
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

void IgneousPipeline::updateTextureAdderMoss() {
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
