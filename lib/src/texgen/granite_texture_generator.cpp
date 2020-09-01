#include "texgen/granite_texture_generator.h"

#include <noise/noise.h>

#include "utils/colors.h"

namespace procrock {
std::shared_ptr<Mesh> GraniteTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  // Based on example from libnoise library
  // http://libnoise.sourceforge.net/examples/textures/index.html

  noise::module::Billow primaryModule;
  primaryModule.SetSeed(seed);
  primaryModule.SetFrequency(frequency);
  primaryModule.SetPersistence(persistence);
  primaryModule.SetLacunarity(lacunarity);
  primaryModule.SetOctaveCount(octaveCount);

  noise::module::Voronoi secondaryModule;
  secondaryModule.SetSeed(seed + 1);
  secondaryModule.SetFrequency(2 * frequency);
  secondaryModule.EnableDistance(true);

  noise::module::ScaleBias scaledSecondary;
  scaledSecondary.SetSourceModule(0, secondaryModule);
  scaledSecondary.SetScale(-0.5);
  scaledSecondary.SetBias(0);

  noise::module::Add combinedModules;
  combinedModules.SetSourceModule(0, primaryModule);
  combinedModules.SetSourceModule(1, scaledSecondary);

  noise::module::Turbulence finalModule;
  finalModule.SetSourceModule(0, combinedModules);
  finalModule.SetSeed(seed + 2);
  finalModule.SetFrequency(frequency / 2);
  finalModule.SetPower(1.0 / frequency);
  finalModule.SetRoughness(roughness);

  auto colorFunction = [&](Eigen::Vector3d worldPos) {
    float value = finalModule.GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1;
    return utils::computeColorGradient(colorGradient, 0, 100, value);
  };

  fillTextureFaceBased(*result, result->albedo, colorFunction);

  return result;
}

PipelineStageInfo& GraniteTextureGenerator::getInfo() { return info; }

Configuration GraniteTextureGenerator::getConfiguration() {
  Configuration result;
  result.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.0f, 100.0f});
  result.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  result.floats.push_back(Configuration::BoundedEntry<float>{
      {"Persistence", "Roughness of the Noise"}, &persistence, 0.0f, 1.0f});
  result.floats.push_back(Configuration::BoundedEntry<float>{
      {"Grain Scale", "Scale the grains by this amount"}, &grainScaling, -2.0f, 2.0f});
  result.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  result.ints.push_back(Configuration::BoundedEntry<int>{
      {"Roughness", "Amount of turbulence applied."}, &roughness, 1, 10});
  result.ints.push_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed for the Noise function."}, &seed, 1, 100000});

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