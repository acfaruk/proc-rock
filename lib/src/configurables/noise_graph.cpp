#include "configurables/noise_graph.h"

#include <stack>

namespace procrock {
Configuration::ConfigurationGroup NoiseNode::getConfig() { return config; }

void NoiseGraph::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup group;
  group.entry = {"Noise", "Modify the noise."};
  group.noiseGraphs.push_back(Configuration::SimpleEntry<NoiseGraph>{
      {"Noise Graph", "Modify the graph that creates the final noise."}, this});

  config.insertToConfigGroups(newGroupName + ": Noise", group);
}

noise::module::Module* evaluateGraph(const NoiseGraph& noiseGraph) {
  auto& graph = noiseGraph.graph;
  if (graph.get_root_node_id() == -1) return nullptr;

  std::stack<int> postOrder;
  dfs_traverse(graph, graph.get_root_node_id(),
               [&postOrder](const int nodeId) -> void { postOrder.push(nodeId); });
  std::stack<noise::module::Module*> modulesStack;

  while (!postOrder.empty()) {
    const int id = postOrder.top();
    postOrder.pop();
    NoiseNode* node = graph.node(id);
    auto module = node->getModule();

    if (node->placeholder && graph.num_edges_from_node(id) != 0) continue;
    for (int i = 0; i < module->GetSourceModuleCount(); i++) {
      auto sourceModule = modulesStack.top();
      modulesStack.pop();
      module->SetSourceModule(i, *sourceModule);
    }
    modulesStack.push(module);
  }

  return modulesStack.top();
}

OutputNoiseNode::OutputNoiseNode() {
  module = std::make_unique<noise::module::TranslatePoint>();
  config.entry = {"Output Node", "This will be the final value."};
}
noise::module::Module* const OutputNoiseNode::getModule() { return module.get(); }

ConstNoiseNode::ConstNoiseNode() {
  module = std::make_unique<noise::module::Const>();
  config.entry = {"Const Value", "Returns the same value everywhere."};

  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"Value", "Value to return."}, &value, -1.0f, 1.0f});
}

noise::module::Module* const ConstNoiseNode::getModule() {
  module->SetConstValue(value);
  return module.get();
}

PerlinNoiseNode::PerlinNoiseNode() {
  module = std::make_unique<noise::module::Perlin>();

  config.entry = {"Perlin Noise", "Set the various parameters of the noise function(s)."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.1f, 100.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Persistence", "Roughness of the Perlin Noise"}, &persistence, 0.0f, 1.0f});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the perlin Noise function."}, &seed, 1, 100000});

  config.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});
}
noise::module::Module* const PerlinNoiseNode::getModule() {
  module->SetFrequency(frequency);
  module->SetLacunarity(lacunarity);
  module->SetPersistence(persistence);
  module->SetOctaveCount(octaveCount);
  module->SetSeed(seed);
  return module.get();
}
}  // namespace procrock