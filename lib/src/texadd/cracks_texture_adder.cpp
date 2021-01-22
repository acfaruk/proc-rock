#include "texadd/cracks_texture_adder.h"

#include <CImg.h>

#include <iostream>

namespace procrock {
CracksTextureAdder::CracksTextureAdder() : TextureAdder(true) {
  Configuration::ConfigurationGroup mainGroup;

  mainGroup.entry = {"Cracks", "Basic settings for the added cracks."};
  mainGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Amount/Frequency of cracks"}, &frequency, 0, 10});
  mainGroup.floats.emplace_back(
      Configuration::BoundedEntry<float>{{"Strength", "Strengthen the cracks"}, &strength, 1, 50});
  mainGroup.ints.emplace_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed the randomness"}, &seed, 0, 100000});

  config.insertToConfigGroups("General", mainGroup);
}

std::shared_ptr<Mesh> CracksTextureAdder::generate(Mesh* before) {
  using namespace cimg_library;

  auto result = std::make_shared<Mesh>(*before);

  noise::module::Voronoi voronoi;
  voronoi.SetFrequency(frequency);
  voronoi.SetSeed(seed);

  auto colorFunction = [&](Eigen::Vector3f worldPos) {
    float value = (voronoi.GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    int colValue = value * 255;
    return colValue;
  };

  auto texGroup = createAddTexture(*result, colorFunction);
  CImg<float> image(texGroup.displacementData.data(), 1, texGroup.width, texGroup.height);
  image.permute_axes("YZCX");
  auto gradients = image.get_gradient("xy", 3);
  int index = 0;
  for (auto& pixel : result->textures.worldMap) {
    Eigen::Vector2f value;
    value.x() = std::abs(gradients[0](index));
    value.y() = std::abs(gradients[1](index));

    int relValue = ((value.x() + value.y()) / 2.0);
    relValue = std::min(255, (int)(relValue * strength));
    texGroup.displacementData[index] = relValue / 255.0;
    index++;
  }

  GradientAlphaAlbedoGenerator albedoGen;
  auto& gradient = albedoGen.coloring.colorGradient;
  gradient.clear();
  gradient.insert({0, {1, 1, 1, 0}});
  gradient.insert({100, {1, 1, 1, 1}});

  albedoGen.modify(texGroup);

  GradientNormalsGenerator normalsGen;
  normalsGen.modify(texGroup);
  GreyscaleAmbientOcclusionGenerator ambientOccGen;
  ambientOccGen.modify(texGroup);

  displacementProportion = 1;
  ambientOccProportion = 1;
  normalProportion = 1;
  addTextures(*result, texGroup);
  return result;
}

PipelineStageInfo& CracksTextureAdder::getInfo() { return info; }

}  // namespace procrock