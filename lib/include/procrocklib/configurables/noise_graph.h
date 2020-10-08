#pragma once

#include <noise/noise.h>
#include <procrocklib/configurables/configurable_extender.h>
#include <procrocklib/configurables/graph.h>

#include <memory>
#include <vector>

namespace procrock {

const int NoiseNodeTypeId_Output = 0;

const int NoiseNodeTypeId_Add = 1;
const int NoiseNodeTypeId_Max = 2;
const int NoiseNodeTypeId_Min = 3;
const int NoiseNodeTypeId_Multiply = 4;
const int NoiseNodeTypeId_Power = 5;

const int NoiseNodeTypeId_Blend = 100;
const int NoiseNodeTypeId_Select = 101;

const int NoiseNodeTypeId_Abs = 200;
const int NoiseNodeTypeId_Clamp = 201;
const int NoiseNodeTypeId_Exponent = 202;
const int NoiseNodeTypeId_Invert = 203;
const int NoiseNodeTypeId_ScaleBias = 204;
const int NoiseNodeTypeId_Terrace = 205;
const int NoiseNodeTypeId_Curve = 206;

const int NoiseNodeTypeId_Displace = 300;
const int NoiseNodeTypeId_RotatePoint = 301;
const int NoiseNodeTypeId_ScalePoint = 302;
const int NoiseNodeTypeId_TranslatePoint = 303;
const int NoiseNodeTypeId_Turbulence = 304;

const int NoiseNodeTypeId_Const = 400;
const int NoiseNodeTypeId_Perlin = 401;
const int NoiseNodeTypeId_Billow = 402;
const int NoiseNodeTypeId_Ridged = 403;
const int NoiseNodeTypeId_Voronoi = 404;
const int NoiseNodeTypeId_Spheres = 405;
const int NoiseNodeTypeId_Cylinders = 406;

class NoiseNode : public GroupConfigurable {
 public:
  int id = -1;
  bool placeholder = false;
  Eigen::Vector2f position{0, 0};

  virtual Configuration::ConfigurationGroup getConfig() = 0;
  virtual noise::module::Module* const getModule() = 0;

  inline int getNodeTypeId() { return nodeTypeId; }

 protected:
  int nodeTypeId = -1;
};

std::unique_ptr<NoiseNode> createNoiseNodeFromTypeId(int noiseNodeTypeId);

struct NoiseGraph : public ConfigurableExtender {
  NoiseGraph(bool empty = false);

  Graph<NoiseNode*> graph;
  std::vector<std::unique_ptr<NoiseNode>> nodes;

  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;

  int addNode(std::unique_ptr<NoiseNode> node, bool rootNode = false,
              Eigen::Vector2f position = {0.0f, 0.0f});

  void addEdge(int fromNode, int toNode, int input = 0);

  void clear();
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

  float lowerBound = -1.0f;
  float upperBound = 1.0f;
  float edgeFalloff = 0.0f;

 private:
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

  float lowerBound = -1.0f;
  float upperBound = 1.0f;

 private:
  std::unique_ptr<noise::module::Clamp> module;
};

class ExponentNoiseNode : public NoiseNode {
 public:
  ExponentNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float exponent = 1.0f;

 private:
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

  float bias = 0.0f;
  float scale = 1.0f;

 private:
  std::unique_ptr<noise::module::ScaleBias> module;
};

class TerraceNoiseNode : public NoiseNode {
 public:
  TerraceNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  ConfigurationList<float> controlPoints = {std::set<float>{0, 1}, 2, 100};
  bool invertTerraces = false;

 private:
  std::unique_ptr<noise::module::Terrace> module;
};

class CurveNoiseNode : public NoiseNode {
 public:
  CurveNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  ConfigurationCurve configCurve = std::map<float, float>{{0, 0}, {0.1, 0.2}, {0.6, 0.15}, {1, 1}};

 private:
  std::unique_ptr<noise::module::Curve> module;
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

  float xAngle = 0.0f, yAngle = 0.0f, zAngle = 0.0f;

 private:
  std::unique_ptr<noise::module::RotatePoint> module;
};

class ScalePointNoiseNode : public NoiseNode {
 public:
  ScalePointNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float xScale = 1.0f, yScale = 1.0f, zScale = 1.0f;

 private:
  std::unique_ptr<noise::module::ScalePoint> module;
};

class TranslatePointNoiseNode : public NoiseNode {
 public:
  TranslatePointNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float xTranslation = 0.0f, yTranslation = 0.0f, zTranslation = 0.0f;

 private:
  std::unique_ptr<noise::module::TranslatePoint> module;
};

class TurbulenceNoiseNode : public NoiseNode {
 public:
  TurbulenceNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float frequency = 31.0f, power = 1.0f;
  int roughness = 2, seed = 0;

 private:
  std::unique_ptr<noise::module::Turbulence> module;
};

// Generators
class ConstNoiseNode : public NoiseNode {
 public:
  ConstNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float value = 0.0f;

 private:
  std::unique_ptr<noise::module::Const> module;
};

class PerlinNoiseNode : public NoiseNode {
 public:
  PerlinNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float frequency = 31.0f;
  float lacunarity = 2.5f;
  float persistence = 0.6f;
  int octaveCount = 3;
  int seed = 0;
  int qualityChoice = 2;

 private:
  std::unique_ptr<noise::module::Perlin> module;
};

class BillowNoiseNode : public NoiseNode {
 public:
  BillowNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float frequency = 31.0f;
  float lacunarity = 2.5f;
  float persistence = 0.6f;
  int octaveCount = 3;
  int seed = 0;
  int qualityChoice = 2;

 private:
  std::unique_ptr<noise::module::Billow> module;
};

class RidgedMultiNoiseNode : public NoiseNode {
 public:
  RidgedMultiNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float frequency = 31.0f;
  float lacunarity = 2.5f;
  int octaveCount = 3;
  int seed = 0;
  int qualityChoice = 2;

 private:
  std::unique_ptr<noise::module::RidgedMulti> module;
};

class VoronoiNoiseNode : public NoiseNode {
 public:
  VoronoiNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float frequency = 31.0f;
  float displacement = 1.0f;
  bool enableDistance = false;
  int seed = 0;

 private:
  std::unique_ptr<noise::module::Voronoi> module;
};

class SpheresNoiseNode : public NoiseNode {
 public:
  SpheresNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float frequency = 31.0f;

 private:
  std::unique_ptr<noise::module::Spheres> module;
};

class CylindersNoiseNode : public NoiseNode {
 public:
  CylindersNoiseNode();
  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() override;

  float frequency = 31.0f;

 private:
  std::unique_ptr<noise::module::Cylinders> module;
};

}  // namespace procrock