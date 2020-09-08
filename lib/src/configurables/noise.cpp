#include "configurables/noise.h"

namespace procrock {

// Perlin
void PerlinNoiseModule::addOwnGroups(Configuration& config, std::string& groupBaseName) {
  Configuration::ConfigurationGroup noiseGroup;

  noiseGroup.entry = {"Perlin Noise", "Set the various parameters of the noise function(s)."};
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.1f, 100.0f});
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Persistence", "Roughness of the Perlin Noise"}, &persistence, 0.0f, 1.0f});
  noiseGroup.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  noiseGroup.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the perlin Noise function."}, &seed, 1, 100000});

  noiseGroup.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});
  config.configGroups[groupBaseName].push_back(noiseGroup);
}
noise::module::Module* PerlinNoiseModule::getModule() {
  module.SetFrequency(frequency);
  module.SetLacunarity(lacunarity);
  module.SetPersistence(persistence);
  module.SetOctaveCount(octaveCount);
  module.SetSeed(seed);
  module.SetNoiseQuality(static_cast<noise::NoiseQuality>(qualityChoice));
  return &module;
}

void BillowNoiseModule::addOwnGroups(Configuration& config, std::string& groupBaseName) {
  Configuration::ConfigurationGroup noiseGroup;

  noiseGroup.entry = {"Billow Noise", "Set the various parameters of the noise function(s)."};
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.1f, 100.0f});
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Persistence", "Roughness of the Billow Noise"}, &persistence, 0.0f, 1.0f});
  noiseGroup.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  noiseGroup.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the Billow Noise function."}, &seed, 1, 100000});

  noiseGroup.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});
  config.configGroups[groupBaseName].push_back(noiseGroup);
}
noise::module::Module* BillowNoiseModule::getModule() {
  module.SetFrequency(frequency);
  module.SetLacunarity(lacunarity);
  module.SetPersistence(persistence);
  module.SetOctaveCount(octaveCount);
  module.SetSeed(seed);
  module.SetNoiseQuality(static_cast<noise::NoiseQuality>(qualityChoice));
  return &module;
}

void RidgedMultiNoiseModule::addOwnGroups(Configuration& config, std::string& groupBaseName) {
  Configuration::ConfigurationGroup noiseGroup;

  noiseGroup.entry = {"Ridged Multi Noise", "Set the various parameters of the noise function(s)."};
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.1f, 100.0f});
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  noiseGroup.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  noiseGroup.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the Ridged Multi Noise function."}, &seed, 1, 100000});

  noiseGroup.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});
  config.configGroups[groupBaseName].push_back(noiseGroup);
}
noise::module::Module* RidgedMultiNoiseModule::getModule() {
  module.SetFrequency(frequency);
  module.SetLacunarity(lacunarity);
  module.SetOctaveCount(octaveCount);
  module.SetSeed(seed);
  module.SetNoiseQuality(static_cast<noise::NoiseQuality>(qualityChoice));
  return &module;
}

void VoronoiNoiseModule::addOwnGroups(Configuration& config, std::string& groupBaseName) {
  Configuration::ConfigurationGroup noiseGroup;

  noiseGroup.entry = {"Voronoi Cells", "Set the various parameters of the noise function(s)."};
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the seed points."}, &frequency, 0.1f, 100.0f});
  noiseGroup.floats.push_back(Configuration::BoundedEntry<float>{
      {"Displacement", "Set frequency of the seed points."}, &displacement, 0.1f, 1.0f});
  noiseGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Distance", "Change value based on distance to center of cell."}, &distance});
  noiseGroup.ints.push_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed for the Vornoi Cells."}, &seed, 1, 100000});

  config.configGroups[groupBaseName].push_back(noiseGroup);
}
noise::module::Module* VoronoiNoiseModule::getModule() {
  module.SetFrequency(frequency);
  module.SetSeed(seed);
  module.EnableDistance(distance);
  module.SetDisplacement(displacement);
  return &module;
}

void SingleNoiseModule::addOwnGroups(Configuration& config, std::string& groupBaseName) {
  Configuration::ConfigurationGroup selectionGroup;
  selectionGroup.entry = {"Select Noise", "Choose noise method."};
  selectionGroup.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Type", "Which type of noise function to use."},
                                       {{"Perlin", "Perlin Noise"},
                                        {"Billow", "Billow Noise"},
                                        {"RidgedMulti", "Ridged Multi Noise"},
                                        {"Voronoi", "Voronoi Cells"}},
                                       &selection});
  config.configGroups[groupBaseName].push_back(selectionGroup);

  switch (selection) {
    case 0:
      perlinModule.addOwnGroups(config, groupBaseName);
      break;
    case 1:
      billowModule.addOwnGroups(config, groupBaseName);
      break;
    case 2:
      ridgedMultiModule.addOwnGroups(config, groupBaseName);
      break;
    case 3:
      voronoiModule.addOwnGroups(config, groupBaseName);
      break;
    default:
      assert(0 && "You forgot a case!");
      break;
  }
}
noise::module::Module* SingleNoiseModule::getModule() {
  switch (selection) {
    case 0:
      return perlinModule.getModule();
    case 1:
      return billowModule.getModule();
    case 2:
      return ridgedMultiModule.getModule();
    case 3:
      return voronoiModule.getModule();
    default:
      assert(0 && "You forgot a case!");
      break;
  }
}
}  // namespace procrock