#include "texgen/checkerboard_texture_generator.h"

#include <igl/barycentric_coordinates.h>

namespace procrock {
CheckerboardTextureGenerator::CheckerboardTextureGenerator() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General Settings", "Set various parameters of the checkerboard."};
  group.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Squares", "How many squares per dimension"}, &squares, 2, 100});

  group.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Type", "Select how the Checkerboard should be generated"},
      {{"Simple UV based", "Creates a 2D checkerboard texture."},
       {"Global Mesh based", "Projects a 3D checkerboard to the texture."}},
      &mode});

  config.insertToConfigGroups("General", group);

  normalsGenerator.addOwnGroups(config, "Normals");
  roughnessGenerator.addOwnGroups(config, "Roughness");
  metalnessGenerator.addOwnGroups(config, "Metalness");
  ambientOccGenerator.addOwnGroups(config, "Ambient Occlusion");
}

std::shared_ptr<Mesh> CheckerboardTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  switch (mode) {
    case 0:  // Normal UV based
    {
      result->textures.albedoData.clear();
      result->textures.albedoData.resize(result->textures.width * result->textures.height * 3);
      unsigned int squareWidth = result->textures.width / squares;
      for (int x = 0; x < result->textures.width; x++) {
        for (int y = 0; y < result->textures.height; y++) {
          int rx = std::floor((x / (float)squareWidth));
          int ry = std::floor((y / (float)squareWidth));
          int i = 3 * (x + result->textures.width * y);
          if ((rx + ry) % 2 == 0) {
            result->textures.albedoData[i] = 255;
            result->textures.albedoData[i + 1] = 255;
            result->textures.albedoData[i + 2] = 255;
          } else {
            result->textures.albedoData[i] = 0;
            result->textures.albedoData[i + 1] = 0;
            result->textures.albedoData[i + 2] = 0;
          }
        }
      }
    } break;
    case 1:  // Global Mesh based
    {
      auto colorFunction = [=](Eigen::Vector3f worldPos) {
        int xI = (worldPos(0) / (1.0 / squares)) + 1000;
        int yI = (worldPos(1) / (1.0 / squares)) + 1000;
        int zI = (worldPos(2) / (1.0 / squares)) + 1000;

        int value = (zI % 2) ? ((xI + yI) % 2 ? 255 : 0) : ((xI + yI) % 2 ? 0 : 255);
        return Eigen::Vector3i{value, value, value};
      };

      fillTexture(result->textures, colorFunction);
    } break;
  }

  normalsGenerator.modify(result->textures);
  roughnessGenerator.modify(result->textures);
  metalnessGenerator.modify(result->textures);
  ambientOccGenerator.modify(result->textures);

  return result;
}
PipelineStageInfo& CheckerboardTextureGenerator::getInfo() { return info; }

}  // namespace procrock
