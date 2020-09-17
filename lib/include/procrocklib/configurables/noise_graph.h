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

  virtual Configuration::ConfigurationGroup getConfig() override;
  virtual noise::module::Module* const getModule() = 0;

 protected:
  Configuration::ConfigurationGroup config;
};

struct NoiseGraph : public ConfigurableExtender {
  Graph<NoiseNode*> graph;
  std::vector<std::unique_ptr<NoiseNode>> nodes;

  virtual void addOwnGroups(Configuration& config, std::string newGroupName) override;

  int addNode(std::unique_ptr<NoiseNode> node, bool rootNode = false, Eigen::Vector2f position = {0.0f, 0.0f});
};

noise::module::Module* evaluateGraph(const NoiseGraph& noiseGraph);

class OutputNoiseNode : public NoiseNode {
 public:
  OutputNoiseNode();
  virtual noise::module::Module* const getModule() override;

 private:
  std::unique_ptr<noise::module::TranslatePoint> module;
};

class ConstNoiseNode : public NoiseNode {
 public:
  ConstNoiseNode();
  virtual noise::module::Module* const getModule() override;

 private:
  float value = 0.0f;

  std::unique_ptr<noise::module::Const> module;
};

class AddNoiseNode : public NoiseNode {
 public:
  AddNoiseNode();
  virtual noise::module::Module* const getModule() override;

  private:
  std::unique_ptr<noise::module::Add> module;
};

class PerlinNoiseNode : public NoiseNode {
 public:
  PerlinNoiseNode();
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

}  // namespace procrock