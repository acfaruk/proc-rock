#include "texture_extras.h"

#include "../../math.h"

namespace procrock {
void TextureExtrasExtender::addOwnGroups(Configuration& config, std::string newGroupName,
                                         std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup textureExtrasGroup;
  textureExtrasGroup.entry = {"Extras", "Other texture settings and additions."};

  textureExtrasGroup.singleChoices.emplace_back(
      Configuration::SingleChoiceEntry{{"Size", "Choose the size of the textures for the model."},
                                       {{"128x128", "Very Low Quality"},
                                        {"256x256", "Low Quality"},
                                        {"512x512", "Medium Quality"},
                                        {"1024x1024", "High Quality"},
                                        {"2048x2048", "Very High Quality"},
                                        {"4096x4096", "Extreme Quality"}},
                                       &textureSizeChoice});

  textureExtrasGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Variance", "Add some variance to the texture to make it appear more natural."},
      &textureVariance});
  textureExtrasGroup.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Moss", "Add moss to the rock."}, &textureMoss});
  textureExtrasGroup.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Veins", "Add veins to the rock."}, &textureVeins});
  textureExtrasGroup.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Spots", "Add veins to the rock."}, &textureSpots});

  Configuration::ConfigurationGroup veinsExtrasGroup;
  auto veinsEnabledFunc = [&]() { return textureVeins; };
  veinsExtrasGroup.entry = {"Veins", "Add veins to the rock texture", veinsEnabledFunc};

  veinsExtrasGroup.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Big", "Show big veins"}, &veins.bigVeins});
  veinsExtrasGroup.bools.push_back(
      Configuration::SimpleEntry<bool>{{"Small", "Show small veins"}, &veins.smallVeins});
  veinsExtrasGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Size", "Set the size of the veins."}, &veins.size, 0, 1});
  veinsExtrasGroup.colors.push_back(Configuration::SimpleEntry<Eigen::Vector3f>{
      {"Color", "Set the color of the veins."}, &veins.color});

  Configuration::ConfigurationGroup spotsExtraGroup;
  auto spotsEnabledFunc = [&]() { return textureSpots; };
  spotsExtraGroup.entry = {"Spots", "Add spots of different color.", spotsEnabledFunc};

  spotsExtraGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Size", "Set the size and frequency of the spots"}, &spots.size, 0, 1});
  spotsExtraGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Strength", "Set the strength of the spots"}, &spots.strength, 0, 1});
  spotsExtraGroup.colors.push_back(Configuration::SimpleEntry<Eigen::Vector3f>{
      {"Color", "Set the color of the spots"}, &spots.color});

  config.insertToConfigGroups(newGroupName, textureExtrasGroup);
  config.insertToConfigGroups(newGroupName, spotsExtraGroup);
  config.insertToConfigGroups(newGroupName, veinsExtrasGroup);
}

void TextureExtrasExtender::setupPipeline(Pipeline* pipeline) {
  auto texadd0 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderVariance = texadd0.get();
  pipeline->addTextureAdder(std::move(texadd0));

  auto texadd1 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderSpots = texadd1.get();
  pipeline->addTextureAdder(std::move(texadd1));

  auto texadd2 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderVeins = texadd2.get();
  pipeline->addTextureAdder(std::move(texadd2));

  auto texadd3 = std::make_unique<NoiseTextureAdder>();
  this->textureAdderMoss = texadd3.get();
  pipeline->addTextureAdder(std::move(texadd3));
}
void TextureExtrasExtender::updatePipeline(Pipeline* pipeline) {
  pipeline->getParameterizer().textureSizeChoice = textureSizeChoice;
  updateTextureAdderVariance();
  updateTextureAdderSpots();
  updateTextureAdderVeins();
  updateTextureAdderMoss();
}

void TextureExtrasExtender::updateTextureAdderVariance() {
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

void TextureExtrasExtender::updateTextureAdderSpots() {
  textureAdderSpots->setDisabled(!textureSpots);

  auto config = textureAdderSpots->getConfiguration();
  auto& noise = textureAdderSpots->noiseGraph;
  noise.clear();

  auto ridgedMultiNoiseNode = std::make_unique<RidgedMultiNoiseNode>();
  auto ridgedMultiNoiseNodePtr = ridgedMultiNoiseNode.get();
  auto ridgedMultiNoiseNodeId = noise.addNode(std::move(ridgedMultiNoiseNode), false, {0, 0});

  ridgedMultiNoiseNodePtr->frequency = math::lerp(1, 20, 1 - spots.size);

  int outputNoiseNodeId = noise.addNode(std::make_unique<OutputNoiseNode>(), true, {400, 0});
  noise.addEdge(ridgedMultiNoiseNodeId, outputNoiseNodeId);

  auto coloring = config.getConfigGroup("Albedo", "Gradient Alpha Coloring")
                      .getGradientAlphaColoring("Gradient");
  coloring->clear();

  (*coloring)[0] =
      Eigen::Vector4f{spots.color.x(), spots.color.y(), spots.color.z(), spots.strength};
  (*coloring)[30] = Eigen::Vector4f{0, 0, 0, 0};
  (*coloring)[100] = Eigen::Vector4f{0, 0, 0, 0};
}

void TextureExtrasExtender::updateTextureAdderVeins() {
  textureAdderVeins->setDisabled(!textureVeins);

  auto config = textureAdderVeins->getConfiguration();
  auto& noise = textureAdderVeins->noiseGraph;
  noise.clear();

  auto billowNoiseNodeBig = std::make_unique<BillowNoiseNode>();
  auto billowNoiseNodeBigPtr = billowNoiseNodeBig.get();
  auto billowNoiseNodeBigId = noise.addNode(std::move(billowNoiseNodeBig), false, {0, 0});

  billowNoiseNodeBigPtr->frequency = 1.25;
  billowNoiseNodeBigPtr->lacunarity = 3.5;
  billowNoiseNodeBigPtr->persistence = 0.58;
  billowNoiseNodeBigPtr->octaveCount = 7;

  auto billowNoiseNodeSmall = std::make_unique<BillowNoiseNode>();
  auto billowNoiseNodeSmallPtr = billowNoiseNodeSmall.get();
  auto billowNoiseNodeSmallId = noise.addNode(std::move(billowNoiseNodeSmall), false, {0, 300});

  billowNoiseNodeSmallPtr->frequency = 6;
  billowNoiseNodeSmallPtr->lacunarity = 3.5;
  billowNoiseNodeSmallPtr->persistence = 0.58;
  billowNoiseNodeSmallPtr->octaveCount = 7;
  billowNoiseNodeSmallPtr->seed = 10;

  auto minNoiseNodeId = noise.addNode(std::make_unique<MinNoiseNode>(), false, {300, 0});

  auto scalePointNoiseNode = std::make_unique<ScalePointNoiseNode>();
  auto scalePointNoiseNodePtr = scalePointNoiseNode.get();
  auto scalePointNoiseNodeId = noise.addNode(std::move(scalePointNoiseNode), false, {500, 0});

  scalePointNoiseNodePtr->xScale = math::lerp(1, 10, 1 - veins.size);
  scalePointNoiseNodePtr->yScale = math::lerp(1, 10, 1 - veins.size);
  scalePointNoiseNodePtr->zScale = math::lerp(1, 10, 1 - veins.size);

  int outputNoiseNodeId = noise.addNode(std::make_unique<OutputNoiseNode>(), true, {700, 0});
  if (veins.bigVeins) {
    noise.addEdge(billowNoiseNodeBigId, minNoiseNodeId, 0);
  }

  if (veins.smallVeins) {
    noise.addEdge(billowNoiseNodeSmallId, minNoiseNodeId, 1);
  }

  noise.addEdge(minNoiseNodeId, scalePointNoiseNodeId);
  noise.addEdge(scalePointNoiseNodeId, outputNoiseNodeId);

  auto coloring = config.getConfigGroup("Albedo", "Gradient Alpha Coloring")
                      .getGradientAlphaColoring("Gradient");
  coloring->clear();

  (*coloring)[0] = Eigen::Vector4f{veins.color.x(), veins.color.y(), veins.color.z(), 1};
  (*coloring)[10] = Eigen::Vector4f{0, 0, 0, 0};
  (*coloring)[100] = Eigen::Vector4f{0, 0, 0, 0};
}

void TextureExtrasExtender::updateTextureAdderMoss() {
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