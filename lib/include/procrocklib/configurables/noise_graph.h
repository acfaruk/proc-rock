#pragma once

#include <noise/noise.h>
#include <procrocklib/configurables/configurable_extender.h>
#include <procrocklib/configurables/graph.h>

#include <memory>
#include <vector>

namespace procrock {

class NoiseNode : public GroupConfigurable {
 public:
  int id = -1;
  bool placeholder = false;
  Eigen::Vector2f position{0, 0};

  virtual Configuration::ConfigurationGroup getConfig() = 0;
  virtual noise::module::Module* const getModule() = 0;
};

struct NoiseGraph : public ConfigurableExtender {
  NoiseGraph();

  Graph<NoiseNode*> graph;
  std::vector<std::unique_ptr<NoiseNode>> nodes;

  virtual void addOwnGroups(Configuration& config, std::string newGroupName) override;

  int addNode(std::unique_ptr<NoiseNode> node, bool rootNode = false,
              Eigen::Vector2f position = {0.0f, 0.0f});
};

noise::module::Module* evaluateGraph(const NoiseGraph& noiseGraph);

class OutputNoiseNode : public NoiseNode {
 public:
  OutputNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::TranslatePoint> module;
};

// Combiners
class AddNoiseNode : public NoiseNode {
 public:
  AddNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Add> module;
};

class MaxNoiseNode : public NoiseNode {
 public:
  MaxNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Max> module;
};

class MinNoiseNode : public NoiseNode {
 public:
  MinNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Min> module;
};

class MultiplyNoiseNode : public NoiseNode {
 public:
  MultiplyNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Multiply> module;
};

class PowerNoiseNode : public NoiseNode {
 public:
  PowerNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Power> module;
};

// Selectors
class BlendNoiseNode : public NoiseNode {
 public:
  BlendNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Blend> module;
};

class SelectNoiseNode : public NoiseNode {
 public:
  SelectNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float lowerBound = -1.0f;
  float upperBound = 1.0f;
  float edgeFalloff = 0.0f;

  std::unique_ptr<noise::module::Select> module;
};

// Modifiers
class AbsNoiseNode : public NoiseNode {
 public:
  AbsNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Abs> module;
};

class ClampNoiseNode : public NoiseNode {
 public:
  ClampNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float lowerBound = -1.0f;
  float upperBound = 1.0f;

  std::unique_ptr<noise::module::Clamp> module;
};

class ExponentNoiseNode : public NoiseNode {
 public:
  ExponentNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float exponent = 1.0f;
  std::unique_ptr<noise::module::Exponent> module;
};

class InvertNoiseNode : public NoiseNode {
 public:
  InvertNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Invert> module;
};

class ScaleBiasNoiseNode : public NoiseNode {
 public:
  ScaleBiasNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float bias = 0.0f;
  float scale = 1.0f;
  std::unique_ptr<noise::module::ScaleBias> module;
};

class TerraceNoiseNode : public NoiseNode {
 public:
  TerraceNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  ConfigurationList<float> controlPoints = {std::set<float>{0, 1}, 2, 100};
  bool invertTerraces = false;

  std::unique_ptr<noise::module::Terrace> module;
};

// Transformers
class DisplaceNoiseNode : public NoiseNode {
 public:
  DisplaceNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::Displace> module;
};

class RotatePointNoiseNode : public NoiseNode {
 public:
  RotatePointNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float xAngle = 0.0f, yAngle = 0.0f, zAngle = 0.0f;

  std::unique_ptr<noise::module::RotatePoint> module;
};

class ScalePointNoiseNode : public NoiseNode {
 public:
  ScalePointNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float xScale = 1.0f, yScale = 1.0f, zScale = 1.0f;

  std::unique_ptr<noise::module::ScalePoint> module;
};

class TranslatePointNoiseNode : public NoiseNode {
 public:
  TranslatePointNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float xTranslation = 0.0f, yTranslation = 0.0f, zTranslation = 0.0f;

  std::unique_ptr<noise::module::TranslatePoint> module;
};

class TurbulenceNoiseNode : public NoiseNode {
 public:
  TurbulenceNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float frequency = 31.0f, power = 1.0f;
  int roughness = 2, seed = 0;

  std::unique_ptr<noise::module::Turbulence> module;
};

// Generators
class ConstNoiseNode : public NoiseNode {
 public:
  ConstNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float value = 0.0f;

  std::unique_ptr<noise::module::Const> module;
};

class PerlinNoiseNode : public NoiseNode {
 public:
  PerlinNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float frequency = 31.0f;
  float lacunarity = 2.5f;
  float persistence = 0.6f;
  int octaveCount = 3;
  int seed = 0;
  int qualityChoice = 2;

  std::unique_ptr<noise::module::Perlin> module;
};

class BillowNoiseNode : public NoiseNode {
 public:
  BillowNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float frequency = 31.0f;
  float lacunarity = 2.5f;
  float persistence = 0.6f;
  int octaveCount = 3;
  int seed = 0;
  int qualityChoice = 2;

  std::unique_ptr<noise::module::Billow> module;
};

class RidgedMultiNoiseNode : public NoiseNode {
 public:
  RidgedMultiNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float frequency = 31.0f;
  float lacunarity = 2.5f;
  int octaveCount = 3;
  int seed = 0;
  int qualityChoice = 2;

  std::unique_ptr<noise::module::RidgedMulti> module;
};

class VoronoiNoiseNode : public NoiseNode {
 public:
  VoronoiNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float frequency = 31.0f;
  float displacement = 1.0f;
  bool enableDistance = false;
  int seed = 0;

  std::unique_ptr<noise::module::Voronoi> module;
};

class SpheresNoiseNode : public NoiseNode {
 public:
  SpheresNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float frequency = 31.0f;

  std::unique_ptr<noise::module::Spheres> module;
};

class CylindersNoiseNode : public NoiseNode {
 public:
  CylindersNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

 private:
  float frequency = 31.0f;

  std::unique_ptr<noise::module::Cylinders> module;
};

}  // namespace procrock