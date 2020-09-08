#pragma once

#include <noise/noise.h>
#include <procrocklib/configurable.h>

#include <memory>

#include "configurable_extender.h"

namespace procrock {
class NoiseModule : public ConfigurableExtender {
 public:
  virtual noise::module::Module* getModule() = 0;
};

class PerlinNoiseModule : public NoiseModule {
 public:
  float frequency = 31.0f;
  float lacunarity = 2.5f;
  float persistence = 0.6f;
  int octaveCount = 3;
  int seed = 0;

  int qualityChoice = 2;

  noise::module::Perlin module;

  virtual void addOwnGroups(Configuration& config, std::string& groupBaseName) override;
  virtual noise::module::Module* getModule() override;
};

class BillowNoiseModule : public NoiseModule {
 public:
  float frequency = 8.0f;
  float lacunarity = 2.0f;
  float persistence = 0.5f;
  int octaveCount = 6;
  int seed = 0;

  int qualityChoice = 2;

  noise::module::Billow module;

  virtual void addOwnGroups(Configuration& config, std::string& groupBaseName) override;
  virtual noise::module::Module* getModule() override;
};

class RidgedMultiNoiseModule : public NoiseModule {
 public:
  float frequency = 8.0f;
  float lacunarity = 2.0f;
  int octaveCount = 6;
  int seed = 0;

  int qualityChoice = 2;

  noise::module::RidgedMulti module;

  virtual void addOwnGroups(Configuration& config, std::string& groupBaseName) override;
  virtual noise::module::Module* getModule() override;
};

class VoronoiNoiseModule : public NoiseModule {
 public:
  float frequency = 8.0f;
  float displacement = 1.0f;

  int seed = 0;
  bool distance = false;

  noise::module::Voronoi module;

  virtual void addOwnGroups(Configuration& config, std::string& groupBaseName) override;
  virtual noise::module::Module* getModule() override;
};

class SingleNoiseModule : public NoiseModule {
 public:
  int selection = 0;  // 0 = Perlin, 1 = Billow, 2 = RidgedMulti, 3 = Voronoi

  PerlinNoiseModule perlinModule;
  BillowNoiseModule billowModule;
  RidgedMultiNoiseModule ridgedMultiModule;
  VoronoiNoiseModule voronoiModule;

  bool useAbsModule;
  noise::module::Abs absModule;

  bool useClampModule;
  float clampLowerBound = -0.5f;
  float clampUpperBound = 0.5f;
  noise::module::Clamp clampModule;

  bool useExponentModule;
  float exponentExponent = 1.0f;
  noise::module::Exponent exponentModule;

  bool useInvertModule;
  noise::module::Invert invertModule;

  bool useScaleBiasModule;
  float scaleBiasBias = 0.0f;
  float scaleBiasScale = 1.0f;
  noise::module::ScaleBias scaleBiasModule;

  virtual void addOwnGroups(Configuration& config, std::string& groupBaseName) override;
  virtual noise::module::Module* getModule() override;
};
}  // namespace procrock