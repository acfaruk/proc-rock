#include "texgen/checkerboard_texture_generator.h"

#include <igl/barycentric_coordinates.h>

namespace procrock {
std::shared_ptr<Mesh> CheckerboardTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  TextureGroup textures;
  textures.albedoData.reserve(textures.width * textures.height * 3);

  switch (mode) {
    case 0:  // Normal UV based
    {
      unsigned int squareWidth = textures.width / squares;
      for (int x = 0; x < textures.width; x++) {
        for (int y = 0; y < textures.height; y++) {
          int rx = std::floor((x / (float)squareWidth));
          int ry = std::floor((y / (float)squareWidth));
          if ((rx + ry) % 2 == 0) {
            textures.albedoData.emplace_back(255);
            textures.albedoData.emplace_back(255);
            textures.albedoData.emplace_back(255);
          } else {
            textures.albedoData.emplace_back(0);
            textures.albedoData.emplace_back(0);
            textures.albedoData.emplace_back(0);
          }
        }
      }

    } break;
    case 1:  // Global Mesh based
    {
      auto colorFunction = [=](Eigen::Vector3d worldPos) {
        int xI = (worldPos(0) / (1.0 / squares)) + 1000;
        int yI = (worldPos(1) / (1.0 / squares)) + 1000;
        int zI = (worldPos(2) / (1.0 / squares)) + 1000;

        int value = (zI % 2) ? ((xI + yI) % 2 ? 255 : 0) : ((xI + yI) % 2 ? 0 : 255);
        return Eigen::Vector3i{value, value, value};
      };

      fillTexture(result->textures, result->textures.albedoData, colorFunction);
    } break;
  }
  result->textures = textures;
  return result;
}

Configuration CheckerboardTextureGenerator::getConfiguration() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General Settings", "Set various parameters of the checkerboard."};
  group.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Squares", "How many squares per dimension"}, &squares, 2, 100});

  group.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Type", "Select how the Checkerboard should be generated"},
      {{"Simple UV based", "Creates a 2D checkerboard texture."},
       {"Global Mesh based", "Projects a 3D checkerboard to the texture."}},
      &mode});

  Configuration result;
  result.configGroups.push_back(group);
  return result;
}
PipelineStageInfo& CheckerboardTextureGenerator::getInfo() { return info; }

}  // namespace procrock
