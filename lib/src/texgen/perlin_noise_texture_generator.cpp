#include "texgen/perlin_noise_texture_generator.h"

#include <noise/noise.h>

#include "utils/colors.h"

namespace procrock {
std::shared_ptr<Mesh> PerlinNoiseTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);
  noise::module::Perlin perlinModule;
  perlinModule.SetFrequency(frequency);
  perlinModule.SetLacunarity(lacunarity);
  perlinModule.SetOctaveCount(octaveCount);
  perlinModule.SetPersistence(persistence);
  perlinModule.SetSeed(seed);
  perlinModule.SetNoiseQuality(static_cast<noise::NoiseQuality>(qualityChoice));

  auto colorFunction = [&](Eigen::Vector3d worldPos) {
    float value = perlinModule.GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1;
    return utils::computeColorGradient(colorGradient, 0, 100, value);
  };

  fillTexture(result->textures, result->textures.albedoData, colorFunction);
  return result;
}

PipelineStageInfo& PerlinNoiseTextureGenerator::getInfo() { return info; }

Configuration PerlinNoiseTextureGenerator::getConfiguration() {
  Configuration result;
  result.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.0f, 100.0f});
  result.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  result.floats.push_back(Configuration::BoundedEntry<float>{
      {"Persistence", "Roughness of the Perlin Noise"}, &persistence, 0.0f, 1.0f});
  result.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  result.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the perlin Noise function."}, &seed, 1, 100000});

  result.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});

  result.gradientColorings.push_back(Configuration::GradientColoringEntry(
      {{"Coloring",
        "Color the texture according to a user defined gradient. Color values are defined over the "
        "range 0-100."},
       &colorGradient}));

  return result;
}
}  // namespace procrock