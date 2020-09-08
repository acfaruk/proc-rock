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

  Configuration::ConfigurationGroup modifierGroup;
  modifierGroup.entry = {"Modifiers", "Select modifiers to apply to the noise."};
  modifierGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Abs", "Return the absolute value of the noise."}, &useAbsModule});
  modifierGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Clamp", "Clamp the noise between two values."}, &useClampModule});
  modifierGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Exponent", "Return the exponent value of the noise."}, &useExponentModule});
  modifierGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Invert", "Return the inverted value of the noise."}, &useInvertModule});
  modifierGroup.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Scale & Bias", "Add a scale and bias to the noise."}, &useScaleBiasModule});

  config.configGroups[groupBaseName].push_back(modifierGroup);

  if (useClampModule || useExponentModule || useScaleBiasModule) {
    Configuration::ConfigurationGroup modifierSettings;
    modifierSettings.entry = {"Modifiers Configuration",
                              "Configure modifiers to apply to the noise."};
    if (useClampModule) {
      modifierSettings.floats.push_back(Configuration::BoundedEntry<float>{
          {"Lower Bound", "Set the lower Bound of the clamping."},
          &clampLowerBound,
          -1.0f,
          clampUpperBound});
      modifierSettings.floats.push_back(Configuration::BoundedEntry<float>{
          {"Upper Bound", "Set the upper Bound of the clamping."},
          &clampUpperBound,
          clampLowerBound,
          1.0f});
    }

    if (useExponentModule) {
      modifierSettings.floats.push_back(Configuration::BoundedEntry<float>{
          {"Exponent", "The exponent to apply."}, &exponentExponent, 0.0f, 5.0f});
    }

    if (useScaleBiasModule) {
      modifierSettings.floats.push_back(Configuration::BoundedEntry<float>{
          {"Bias", "Bias to apply to the noise."}, &scaleBiasBias, -2.0f, 2.0f});
      modifierSettings.floats.push_back(Configuration::BoundedEntry<float>{
          {"Scale", "Scale to apply to the noise."}, &scaleBiasScale, -2.0f, 2.0f});
    }
    config.configGroups[groupBaseName].push_back(modifierSettings);
  }
}
noise::module::Module* SingleNoiseModule::getModule() {
  noise::module::Module* current;
  switch (selection) {
    case 0:
      current = perlinModule.getModule();
      break;
    case 1:
      current = billowModule.getModule();
      break;
    case 2:
      current = ridgedMultiModule.getModule();
      break;
    case 3:
      current = voronoiModule.getModule();
      break;
    default:
      assert(0 && "You forgot a case!");
      break;
  }

  noise::module::Module* result = current;
  if (useAbsModule) {
    absModule.SetSourceModule(0, *result);
    result = &absModule;
  }

  if (useClampModule) {
    clampModule.SetSourceModule(0, *result);
    clampModule.SetBounds(clampLowerBound, clampUpperBound);
    result = &clampModule;
  }

  if (useExponentModule) {
    exponentModule.SetSourceModule(0, *result);
    exponentModule.SetExponent(exponentExponent);
    result = &exponentModule;
  }

  if (useInvertModule) {
    invertModule.SetSourceModule(0, *result);
    result = &invertModule;
  }

  if (useScaleBiasModule) {
    scaleBiasModule.SetSourceModule(0, *result);
    scaleBiasModule.SetBias(scaleBiasBias);
    scaleBiasModule.SetScale(scaleBiasScale);
    result = &scaleBiasModule;
  }

  return result;
}
void CombinedNoiseModule::addOwnGroups(Configuration& config, std::string& groupBaseName) {
  Configuration::ConfigurationGroup selectionGroup;
  selectionGroup.entry = {"Select Combination Method", "How to combine the two noise functions."};
  selectionGroup.singleChoices.push_back(Configuration::SingleChoiceEntry{
      {"Method", "Which method to use for combining the noises."},
      {{"Add", "Add the two input noise values."},
       {"Max", "Get the maximum of the two input noise values."},
       {"Min", "Get the minimum of the two input noise values."},
       {"Multiply", "Multiply the two input noise values."},
       {"Power", "Raise on noise value to the power of the other one."}},
      &selection});
  config.configGroups[groupBaseName].push_back(selectionGroup);

  std::string first = "First Noise Function";
  firstModule.addOwnGroups(config, first);
  std::string second = "Second Noise Function";
  secondModule.addOwnGroups(config, second);
}
noise::module::Module* CombinedNoiseModule::getModule() {
  noise::module::Module* result;

  switch (selection) {
    case 0:
      result = &addModule;
      break;
    case 1:
      result = &maxModule;
      break;
    case 2:
      result = &minModule;
      break;
    case 3:
      result = &multiplyModule;
      break;
    case 4:
      result = &powerModule;
      break;
    default:
      assert(0 && "Handle all cases!");
      break;
  }
  result->SetSourceModule(0, *firstModule.getModule());
  result->SetSourceModule(1, *secondModule.getModule());
  return result;
}
void SelectedNoiseModule::addOwnGroups(Configuration& config, std::string& groupBaseName) {
  Configuration::ConfigurationGroup selectionGroup;
  selectionGroup.entry = {"Choose Selection Method", "How to select from the two noise functions."};
  selectionGroup.singleChoices.push_back(Configuration::SingleChoiceEntry{
      {"Method", "Which method to use for selecting the noises."},
      {{"Blend", "Blend between two noise values with the help of a third one."},
       {"Select", "Select either noise function."}},
      &selection});

  if (selection == 1) {
    selectionGroup.floats.push_back(Configuration::BoundedEntry<float>{
        {"Lower Bound", "Choose the lower bound of the selection range."},
        &selectLowerBound,
        -1.0f,
        selectUpperBound});
    selectionGroup.floats.push_back(Configuration::BoundedEntry<float>{
        {"Upper Bound", "Choose the upper bound of the selection range."},
        &selectUpperBound,
        selectLowerBound,
        1.0f});
    selectionGroup.floats.push_back(Configuration::BoundedEntry<float>{
        {"Edge Falloff", "Falloff value at edge transition."}, &selectEdgeFalloff, 0.0f, 1.0f});
  }

  config.configGroups[groupBaseName].push_back(selectionGroup);

  std::string first = "First Noise Function";
  firstModule.addOwnGroups(config, first);
  std::string second = "Second Noise Function";
  secondModule.addOwnGroups(config, second);
  std::string control = "Control Noise Function";
  selecterModule.addOwnGroups(config, control);
}
noise::module::Module* SelectedNoiseModule::getModule() {
  noise::module::Module* result;

  switch (selection) {
    case 0:
      result = &blendModule;
      break;
    case 1:
      selectModule.SetBounds(selectLowerBound, selectUpperBound);
      selectModule.SetEdgeFalloff(selectEdgeFalloff);
      result = &selectModule;
      break;
    default:
      assert(0 && "Handle all cases!");
      break;
  }
  result->SetSourceModule(0, *firstModule.getModule());
  result->SetSourceModule(1, *secondModule.getModule());
  result->SetSourceModule(2, *selecterModule.getModule());
  return result;
}
}  // namespace procrock