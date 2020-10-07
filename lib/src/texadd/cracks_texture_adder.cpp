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
  auto noiseModule = evaluateGraph(noiseGraph);

  noise::module::Voronoi voronoi;
  voronoi.SetFrequency(frequency);
  voronoi.SetSeed(seed);

  auto colorFunction = [&](Eigen::Vector3d worldPos) {
    float value = (voronoi.GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    int colValue = value * 255;
    return Eigen::Vector4i{colValue, colValue, colValue, 255};
  };

  auto texGroup = createAddTexture(*result, colorFunction);
  CImg<unsigned char> image(texGroup.albedoData.data(), texGroup.albedoChannels, texGroup.width,
                            texGroup.height);
  image.permute_axes("YZCX");
  auto gradients = image.get_gradient("xy", 3);
  int index = 0;
  for (auto& pixel : result->textures.worldMap) {
    if (pixel.positions.size() > 0) {
      Eigen::Vector2f value;
      value.x() = std::abs(gradients[0](index));
      value.y() = std::abs(gradients[1](index));

      int relValue = ((value.x() + value.y()) / 2.0);
      relValue = std::min(255, (int)(relValue * strength));
      texGroup.albedoData[(4 * index)] = relValue;
      texGroup.albedoData[(4 * index) + 1] = relValue;
      texGroup.albedoData[(4 * index) + 2] = relValue;
      texGroup.albedoData[(4 * index) + 3] = relValue;
    }
    index++;
  }

  GradientNormalsGenerator normalsGen;
  normalsGen.modify(texGroup);
  GreyscaleAmbientOcclusionGenerator ambientOccGen;
  ambientOccGen.modify(texGroup);

  ambientOccProportion = 1;
  normalProportion = 1;
  addTextures(*result, texGroup);
  return result;
}

PipelineStageInfo& CracksTextureAdder::getInfo() { return info; }

}  // namespace procrock