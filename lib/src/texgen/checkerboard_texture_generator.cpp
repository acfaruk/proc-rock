#include "texgen/checkerboard_texture_generator.h"

namespace procrock {
std::shared_ptr<Mesh> CheckerboardTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  Texture albedo;
  albedo.data.reserve(albedo.width * albedo.height * 3);

  unsigned int squareWidth = albedo.width / squares;
  for (int x = 0; x < albedo.width; x++) {
    for (int y = 0; y < albedo.height; y++) {
      int rx = std::floor((x / (float)squareWidth));
      int ry = std::floor((y / (float)squareWidth));
      if ((rx + ry) % 2 == 0) {
        albedo.data.emplace_back(255);
        albedo.data.emplace_back(255);
        albedo.data.emplace_back(255);
      } else {
        albedo.data.emplace_back(0);
        albedo.data.emplace_back(0);
        albedo.data.emplace_back(0);
      }
    }
  }

  result->albedo = albedo;

  return result;
}

Configuration CheckerboardTextureGenerator::getConfiguration() {
  Configuration result;
  result.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Squares", "How many squares per dimension"}, &squares, 2, 100});

  return result;
}
PipelineStageInfo& CheckerboardTextureGenerator::getInfo() { return info; }

}  // namespace procrock
